#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <array>
#include <string>
#include <algorithm>
#include <numeric>

class Histogram
{
public:
    static constexpr int SIZE = 'z' - 'a' + 1;

    explicit Histogram(const std::string& word);

    void operator+=(char c);

    void operator-=(const Histogram& other);

    Histogram operator-(const Histogram& other) const;

    bool isClear() const { return std::none_of(m_hits.begin(), m_hits.end(), [](int i) { return i > 0; }); }

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
