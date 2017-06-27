#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "histogram.h"

#include <memory>
#include <string>
#include <vector>

class Histogram;

class Dictionary
{
public:
    struct Node;

    struct Derivation
    {
        const Node* wordEnd;
        const Node* derivationNode;
    };

    Dictionary();

    void insert(const std::string& word);

    bool contains(const std::string& word) const;

    int size() const { return m_size; }

    std::vector<std::string> anagrams(const std::string& word) const;

    std::vector<std::string> anagramDerivations(const std::string& word) const;

    std::vector<Derivation> anagramDerivations(Histogram histogram) const;

    std::vector<std::string> longestAnagramDerivations(const std::string& word) const;

    struct Node
    {
        Node* parent;
        char letter;
        std::vector<std::unique_ptr<Node>> children;
        bool isWordEnd;
        std::size_t depth;

        bool isRoot() const { return parent == nullptr; }
        bool insert(std::string::const_iterator b, std::string::const_iterator e);
        bool contains(std::string::const_iterator b, std::string::const_iterator e) const;
        void anagrams(Histogram histogram, std::vector<const Node*>& result) const;
        void anagramDerivations(Histogram histogram, const Node* derivationNode, std::vector<Derivation>& result) const;
    };

private:
    void findLongest(Histogram histogram,
                     std::vector<Derivation> path,
                     std::vector<std::vector<Derivation>>& longest) const;

private:
    Node m_root;
    int m_size;
};

#endif // DICTIONARY_H
