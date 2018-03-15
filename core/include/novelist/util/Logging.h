/**********************************************************
 * @file   Logging.h
 * @author jan
 * @date   3/14/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_LOGGING_H
#define NOVELIST_LOGGING_H

#include <string_view>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <cstring>
#include <QDebug>

#undef NOVELIST_CURRENT_FUNCTION
#ifdef __clang__
#define NOVELIST_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__)
#define NOVELIST_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define NOVELIST_CURRENT_FUNCTION __FUNCSIG__
#endif

namespace novelist::log {
    namespace internal {
        template<class T>
        constexpr std::size_t computeMagicValue() noexcept
        {
            // Get start position of template type in function string.
            // We assume the template type will always be long int for this purpose. Then the found position is
            // adjusted for the difference in function names to type_name().
            // This should work reasonably well on any compiler. Fingers crossed.
            static_assert(std::is_same_v<T, long int>);

            // TODO: Make s constexpr as soon as https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66639 is resolved.
            std::string_view const s = NOVELIST_CURRENT_FUNCTION;
            auto const pos = s.find("long int");
            auto adjustedPos = pos;
            adjustedPos += std::string_view("std::string_view").size() - std::string_view("std::size_t").size();
            adjustedPos += std::string_view("novelist::log::type_name").size() - std::string_view("novelist::log::internal::computeMagicValue").size();
            return adjustedPos;
        }
    }

    /**
     * Get the fully qualified type name as a string
     * @tparam T Type
     * @return String view of the qualified type name
     */
    template<class T>
    constexpr std::string_view type_name() noexcept
    {
        auto const magicValue = internal::computeMagicValue<long int>();
        std::string_view s = NOVELIST_CURRENT_FUNCTION;

        // Magic number gives the start of the template type within the function string. The end is marked by either
        // ';' or ']' (gcc, clang) or '>' (MVC). In the latter case we need to make sure to find only '>' that doesn't
        // match a previous opening '<', or otherwise the result will be wrong for template types.
        size_t angularBracketCount = 0;
        size_t i = magicValue;
        for (; i < s.size(); ++i)
        {
            auto const c = s.at(i);
            if (c == ';' || c == ']')
                break;
            else if (c == '<')
                angularBracketCount++;
            else if (c == '>')
            {
                if (angularBracketCount == 0)
                    break;
                angularBracketCount--;
            }
        }

        return std::string_view(s.begin() + magicValue, i - magicValue);
    }
}

#endif //NOVELIST_LOGGING_H
