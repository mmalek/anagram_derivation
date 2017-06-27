#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <array>
#include <string>

// Represents character histogram of a word
// i.e. stores how many times each alphabet letter repeats in a word
class Histogram
{
public:
    static constexpr int SIZE = 'z' - 'a' + 1;

    explicit Histogram(const std::string& word);

    // Checks if the other histogram fully contains in a current histogram.
    // If so, the other word is a potential distant anagram derivation of a current word.
    bool contains(const Histogram& other) const;

    // Check if character histogram differs only by one, that is, only one character
    // is different from the other word.
    // If so, the other word is an anagram derivation of a current word.
    bool diffByOne(const Histogram& other) const;

private:
    static constexpr int index(char c)
    {
        const int i = c - 'a';
        return (i >= 0 && i < SIZE) ? i : -1;
    }

private:
    std::array<int, SIZE> m_hits;
};

#endif // HISTOGRAM_H
