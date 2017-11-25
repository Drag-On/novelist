/**********************************************************
 * @file   TextAnalyzer.cpp
 * @author jan
 * @date   11/25/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "TextAnalyzer.h"

namespace novelist {
    namespace TextAnalyzer {
        Stats analyze(QString const& text) noexcept
        {
            enum State {
                NoWord,
                Word,
            };

            Stats r{};
            State s = NoWord;
            for (auto const& c : text) {
                if (!c.isHighSurrogate())
                    ++r.m_charCount;
                switch (s) {
                    case NoWord:
                        if (c.isLetterOrNumber())
                            s = Word;
                        break;
                    case Word:
                        if (!c.isLetterOrNumber()) {
                            ++r.m_wordCount;
                            s = NoWord;
                        }
                        break;
                }
            }
            if (s == Word)
                ++r.m_wordCount;

            return r;
        }

        Stats combine(Stats const& r1, Stats const& r2) noexcept
        {
            Stats r{};
            r.m_charCount = r1.m_charCount + r2.m_charCount;
            r.m_wordCount = r2.m_wordCount + r2.m_wordCount;
            return r;
        }

        Stats combine(Stats&& r1, Stats const& r2) noexcept
        {
            r1.m_charCount += r2.m_charCount;
            r1.m_wordCount += r2.m_wordCount;
            return r1;
        }

        bool Stats::operator==(Stats const& rhs) const
        {
            return m_charCount == rhs.m_charCount && m_wordCount == rhs.m_wordCount;
        }

        bool Stats::operator!=(Stats const& rhs) const
        {
            return !(rhs == *this);
        }
    }
}