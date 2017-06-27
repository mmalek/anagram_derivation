#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <array>
#include <bitset>
#include <string>

class Histogram
{
public:
    static constexpr int SIZE = 'z' - 'a' + 1;

    explicit Histogram(const std::string& word);

    bool contains(char c) const;

    int occurrences(char c) const;

    void add(char c);

    bool remove(char c);

    bool isClear() const { return m_dirtyMask.none(); }

private:
    static constexpr int index(char c)
    {
        const int i = c - 'a';
        return (i >= 0 && i < SIZE) ? i : -1;
    }

private:
    std::array<int, SIZE> m_hits;
    std::bitset<SIZE> m_dirtyMask;
};

#endif // HISTOGRAM_H
