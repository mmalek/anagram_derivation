#include "histogram.h"
#include <algorithm>

Histogram::Histogram(const std::string &word)
{
    std::fill(m_hits.begin(), m_hits.end(), 0);

    for (char c : word) {
        add(c);
    }
}

bool Histogram::contains(char c) const
{
    return occurrences(c) > 0;
}

int Histogram::occurrences(char c) const
{
    const int i = index(c);
    if (i >= 0)
        return m_hits[i];
    else
        return 0;
}

void Histogram::add(char c)
{
    const int i = index(c);
    if (i >= 0) {
        ++m_hits[i];
        m_dirtyMask.set(i, m_hits[i] > 0);
    }
}

bool Histogram::remove(char c)
{
    const int i = index(c);
    if (i >= 0 && m_hits[i] > 0) {
        --m_hits[i];
        m_dirtyMask.set(i, m_hits[i] > 0);
        return true;
    } else {
        return false;
    }
}
