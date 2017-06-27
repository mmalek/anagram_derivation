
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
        // Check if character histogram differs only by one, that is, only one character
        // is different from the current word. If so, we have anagram derivation of a current word.
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
            if (isValidWord(word)) {
                std::string curWord = word.toStdString();

                // Load only words that are potential derivations of an input word
                // This is done by checking if character histogram of an input word
                // fully contains in histogram of a dictionary entry.
                // We also inore entries shorter or equal to input word size
                Histogram curHistogram(curWord);
                if (curHistogram.contains(histogram) && curWord.size() > inputWord.size()) {
                    derivations.emplace_back(std::move(curWord), std::move(curHistogram));
                }
            }
        }
    }

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


    // We assume only latin characters are used
    const std::string startWord = wordArg.toStdString();

    // Load dictionary file. Store only these words that are potential anagram derivations
    // of an input word
    std::vector<WordHist> derivations = createDerivations(parser.value(dictionaryOption), startWord);

    // Sort loaded entries by size before putting it into the tree
    // This is needed to first put shorter words to the tree.
    std::sort(derivations.begin(), derivations.end(),
              [](const WordHist& l, const WordHist& r) { return l.first.size() < r.first.size(); });

    // Now we create a derivation tree
    Histogram histogram(startWord);
    Node root{nullptr, startWord, histogram, {}};
    for (std::size_t i = 0; i < derivations.size(); ++i) {
        std::cout << "Building dervations tree: " << i*100/derivations.size() << "%\r";
        WordHist& wh = derivations[i];
        root.insert(std::move(wh.first), std::move(wh.second));
    }

    // We retrieve the longest derivatons just by traversing the tree to the longest branches
    std::vector<const Node*> longest;
    root.longestDerivation(longest);

    // Print results
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
