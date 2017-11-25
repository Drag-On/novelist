/**********************************************************
 * @file   TextAnalyzer.h
 * @author jan
 * @date   11/25/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTANALYZER_H
#define NOVELIST_TEXTANALYZER_H

#include <cstdlib>
#include <QtCore/QDateTime>

namespace novelist {
    namespace TextAnalyzer {
        /**
         * Text analysis result
         */
        struct Stats {
            size_t m_charCount = 0; //!< Number of characters of the text
            size_t m_wordCount = 0; //!< Number of words

            bool operator==(Stats const& rhs) const;

            bool operator!=(Stats const& rhs) const;
        };

        /**
         * Analyzes a text for statistical properties like character count and word count
         * @param text Text to analyze
         * @return Analysis result
         */
        Stats analyze(QString const& text) noexcept;

        /**
         * Combine two analysis results
         * @param r1 First result
         * @param r2 Second result
         * @return Combined stats
         */
        Stats combine(Stats const& r1, Stats const& r2) noexcept;

        /**
         * Combine two analysis results
         * @param r1 First result
         * @param r2 Second result
         * @return Combined stats
         */
        Stats combine(Stats&& r1, Stats const& r2) noexcept;

        /**
         * Combine any number of analysis results
         * @tparam T Stats
         * @tparam U Stats
         * @tparam Ts Stats
         * @param r1 First result
         * @param r2 Second result
         * @param args More results
         * @return Combined stats
         */
        template<typename T, typename U, typename ... Ts>
        Stats combine(T r1, U r2, Ts... args) noexcept
        {
            return combine(combine(std::forward<T>(r1), std::forward<T>(r2)), std::forward<Ts>(args)...);
        }
    }
}

#endif //NOVELIST_TEXTANALYZER_H
