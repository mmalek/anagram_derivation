
#include "histogram.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QSet>

#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>

inline bool isValidWord(const QString& word)
{
    return std::all_of(word.begin(), word.end(), [](QChar c){ return c.isLetter(); });
}

struct Node
{
    Node* parent;
    const std::string& m_word;
    const Histogram& m_histogram;
    std::vector<std::unique_ptr<Node>> m_children;

    bool insert(const std::string& word, const Histogram& histogram);

    void longestDerivation(std::vector<const Node*>& result);
};

bool Node::insert(const std::string& word, const Histogram& histogram)
{
    if (word.size() > m_word.size() + 1) {
        for (const std::unique_ptr<Node>& child : m_children) {
            if (child->insert(word, histogram)) {
                return true;
            }
        }
    } else {
        if (histogram.diffByOne(m_histogram)) {
            m_children.emplace_back(new Node{this, word, histogram, {}});
            return true;
        }
    }

    return false;
}

void Node::longestDerivation(std::vector<const Node*>& result)
{
    if (result.empty() || m_word.size() > result.front()->m_word.size())
        result = {this};
    else if (result.front()->m_word.size() == m_word.size())
        result.push_back(this);

    for (const std::unique_ptr<Node>& child : m_children) {
        child->longestDerivation(result);
    }
}

using WordHist = std::pair<std::string, Histogram>;
std::vector<WordHist> createDerivations(QString fileName, std::string inputWord)
{
    std::vector<WordHist> derivations;

    if (fileName.isEmpty()) {
        fileName = ":/dictionary.txt";
    }

    const Histogram histogram(inputWord);

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            const QString word = QString::fromUtf8(file.readLine()).trimmed().toLower();
            if (word.size() >= 3 && isValidWord(word)) {
                std::string curWord = word.toStdString();
                Histogram curHistogram(curWord);
                Histogram diff = histogram - curHistogram;
                if (diff.isClear() && curWord.size() > inputWord.size()) {
                    derivations.emplace_back(std::move(curWord), std::move(curHistogram));
                }
            }
        }
    }

    std::sort(derivations.begin(), derivations.end(),
              [](const WordHist& l, const WordHist& r) { return l.first.size() < r.first.size(); });

    return derivations;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("anagram_derivation");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("word",
                                 QCoreApplication::translate("main", "3-letter input word"));
    QCommandLineOption dictionaryOption({"d", "dictionary"},
                                        QCoreApplication::translate("main", "Dictionary file."),
                                        QCoreApplication::translate("main", "file"));
    parser.addOption(dictionaryOption);
    parser.process(app);

    if (parser.positionalArguments().empty()) {
        std::cerr << "No word provided" << std::endl;
        return 1;
    }

    const QString wordArg = parser.positionalArguments().first().toLower();
    if (wordArg.size() < 1 || !isValidWord(wordArg)) {
        std::cerr << "The input word '" << wordArg.toStdString() << "' is invalid" << std::endl;
        return 1;
    }

    const std::string startWord = wordArg.toStdString();

    std::vector<WordHist> derivations = createDerivations(parser.value(dictionaryOption), startWord);
    Histogram histogram(startWord);

    Node root{nullptr, startWord, histogram, {}};
    for (std::size_t i = 0; i < derivations.size(); ++i) {
        std::cout << "Building dervations tree: " << i*100/derivations.size() << "%\r";
        WordHist& wh = derivations[i];
        root.insert(std::move(wh.first), std::move(wh.second));
    }

    std::vector<const Node*> longest;
    root.longestDerivation(longest);

    for (const Node* derivation : longest) {
        std::deque<std::string> path;
        for (; derivation != nullptr; derivation = derivation->parent) {
            path.push_front(derivation->m_word);
        }

        if (!path.empty()) {
            std::cout << path.front();
            for (auto it = std::next(path.begin()); it != path.end(); ++it) {
                std::cout << "->" << *it;
            }
            std::cout << std::endl;
        }
    }

    return 0;
}
