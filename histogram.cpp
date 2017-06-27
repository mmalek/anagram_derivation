#include "histogram.h"
#include <algorithm>

Histogram::Histogram(const std::string &word)
{
    std::fill(m_hits.begin(), m_hits.end(), 0);

    for (char c : word) {
        *this += c;
    }
}

void Histogram::operator+=(char c)
{
    const int i = index(c);
    if (i >= 0) {
        ++m_hits[i];
    }
}

void Histogram::operator-=(const Histogram& other)
{
    for (int i = 0; i < SIZE; ++i) {
        m_hits[i] -= other.m_hits[i];
    }
}

Histogram Histogram::operator-(const Histogram& other) const
{
    Histogram histogram = *this;
    histogram -= other;
    return histogram;
}

bool Histogram::diffByOne(const Histogram& other) const
{
    bool isDiffByOne = false;
    for (int i = 0; i < SIZE; ++i) {
        const int diff = m_hits[i] - other.m_hits[i];
        if (diff == 1 && !isDiffByOne)
            isDiffByOne = true;
        else if (diff == 1 && isDiffByOne)
            return false;
        else if (diff != 0)
            return false;
    }
    return isDiffByOne;
}
