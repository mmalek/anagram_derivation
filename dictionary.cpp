#include "dictionary.h"
#include "histogram.h"

#include <algorithm>
#include <sstream>

namespace {

std::string intoString(const Dictionary::Node* node)
{
    std::string str(node->depth, '\0');
    while (!node->isRoot()) {
        str[node->depth-1] = node->letter;
        node = node->parent;
    }
    return str;
}

std::vector<std::string> intoStrings(const std::vector<const Dictionary::Node*>& nodes)
{
    std::vector<std::string> strings;
    for (const Dictionary::Node* node : nodes) {
        strings.push_back(intoString(node));
    }
    return strings;
}

std::vector<std::string> intoStrings(const std::vector<Dictionary::Derivation>& derivations)
{
    std::vector<std::string> strings;
    for (const Dictionary::Derivation& derivation : derivations) {
        strings.push_back(intoString(derivation.wordEnd));
    }
    return strings;
}

} // namespace

Dictionary::Dictionary()
    : m_size(0),
      m_root(Node{nullptr, '\0', {}, false, 0})
{
}

void Dictionary::insert(const std::string& word)
{
    if (m_root.insert(word.begin(), word.end())) {
        ++m_size;
    }
}

bool Dictionary::contains(const std::string &word) const
{
    return m_root.contains(word.begin(), word.end());
}

std::vector<std::string> Dictionary::anagrams(const std::string &word) const
{
    std::vector<const Node*> nodeAgs;
    for (const std::unique_ptr<Node>& child : m_root.children) {
        child->anagrams(Histogram(word), nodeAgs);
    }

    return intoStrings(nodeAgs);
}

std::vector<std::string> Dictionary::anagramDerivations(const std::string &word) const
{
    return intoStrings(anagramDerivations(Histogram(word)));
}

std::vector<Dictionary::Derivation> Dictionary::anagramDerivations(Histogram histogram) const
{
    std::vector<Derivation> derivations;
    for (const std::unique_ptr<Node>& child : m_root.children) {
        child->anagramDerivations(histogram, nullptr, derivations);
    }
    return derivations;
}

std::vector<std::string> Dictionary::longestAnagramDerivations(const std::string &word) const
{
    std::vector<std::vector<Derivation>> longest;
    findLongest(Histogram(word), {}, longest);

    std::vector<std::string> paths;
    for (const std::vector<Derivation>& path : longest) {
        std::stringstream ss;
        ss << word;
        for (const Derivation& derivation : path) {
            ss << '+' << derivation.derivationNode->letter << '=' << intoString(derivation.wordEnd);
        }
        paths.push_back(ss.str());
    }
    return paths;
}

void Dictionary::findLongest(const Histogram histogram,
                             const std::vector<Derivation> path,
                             std::vector<std::vector<Derivation>>& longest) const
{
    std::vector<Derivation> derivations = anagramDerivations(histogram);
    std::sort(derivations.begin(), derivations.end(), [](const Derivation& lhs, const Derivation& rhs) { return lhs.derivationNode->letter < rhs.derivationNode->letter; });
    derivations.erase(std::unique(derivations.begin(), derivations.end(), [](const Derivation& lhs, const Derivation& rhs) { return lhs.derivationNode->letter == rhs.derivationNode->letter; }),
                      derivations.end());

    for (const Derivation& derivation : derivations) {
        std::vector<Derivation> newPath = path;
        newPath.push_back(derivation);

        if (longest.empty() || longest.front().size() < newPath.size()) {
            longest = {newPath};
        } else if (longest.front().size() == newPath.size()) {
            longest.push_back(newPath);
        }

        Histogram newHistogram = histogram;
        newHistogram.add(derivation.derivationNode->letter);

        findLongest(newHistogram, newPath, longest);
    }
}

bool Dictionary::Node::insert(std::string::const_iterator b, std::string::const_iterator e)
{
    if (b != e) {
        auto it = std::find_if(children.begin(),
                               children.end(),
                               [b](const auto& child){ return child->letter == *b; });

        if (it != children.end()) {
            auto next = std::next(b);
            if (next != e) {
                return (*it)->insert(next, e);
            } else if (!isWordEnd) {
                isWordEnd = true;
                return true;
            } else {
                return false;
            }
        } else {
            Node* current = this;
            for (; b != e; ++b) {
                current->children.emplace_back(new Node{current, *b, {}, false, current->depth+1});
                current = current->children.back().get();
            }
            current->isWordEnd = true;
            return true;
        }
    } else {
        return false;
    }
}

bool Dictionary::Node::contains(std::string::const_iterator b, std::string::const_iterator e) const
{
    if (b != e) {
        auto it = std::find_if(children.begin(),
                               children.end(),
                               [b](const auto& child){ return child->letter == *b; });

        if (it != children.end()) {
            return (*it)->contains(std::next(b), e);
        } else {
            return false;
        }
    } else {
        return isWordEnd;
    }
}

void Dictionary::Node::anagrams(Histogram histogram, std::vector<const Node*>& result) const
{
    if (histogram.remove(letter)) {
        if (histogram.isClear()) {
            for (const std::unique_ptr<Node>& child : children) {
                child->anagrams(histogram, result);
            }
        } else if (isWordEnd) {
            result.push_back(this);
        }
    }
}

void Dictionary::Node::anagramDerivations(Histogram histogram, const Node* derivationNode, std::vector<Derivation> &result) const
{
    const bool removed = histogram.remove(letter);
    if (!removed && derivationNode) {
        return;
    } else if (!removed && !derivationNode) {
        derivationNode = this;
    }

    if (!histogram.isClear()) {
        for (const std::unique_ptr<Node>& child : children) {
            child->anagramDerivations(histogram, derivationNode, result);
        }
    } else if (isWordEnd && derivationNode) {
        result.push_back(Derivation{this, derivationNode});
    }
}
