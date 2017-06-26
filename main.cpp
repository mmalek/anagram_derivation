
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QSet>

#include <algorithm>

inline bool isValidWord(const QString& word)
{
    return std::all_of(word.begin(), word.end(), [](QChar c){ return c.isLetter(); });
}

QSet<QString> loadDictionary(QString fileName)
{
    QSet<QString> dictionary;

    if (fileName.isEmpty()) {
        fileName = ":/dictionary.txt";
    }

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            const QString word = QString::fromUtf8(file.readLine()).trimmed().toLower();
            if (word.size() >= 3 && isValidWord(word)) {
                dictionary.insert(word);
            }
        }
    }

    return dictionary;
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
        qWarning("No word provided");
        return 1;
    }

    const QString inputWord = parser.positionalArguments().first().toLower();
    if (inputWord.size() < 3 || !isValidWord(inputWord)) {
        qWarning(QString("The input word '%1' is invalid").arg(inputWord).toLocal8Bit().data());
        return 1;
    }

    qInfo("Loading dictionary...");

    QSet<QString> dictionary = loadDictionary(parser.value(dictionaryOption));
    if (dictionary.isEmpty()) {
        qWarning("Cannot load dictionary file or dictionary is empty");
        return 1;
    }

    qInfo("%d words in the dictionary", dictionary.size());

    return 0;
}
