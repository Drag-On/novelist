/**********************************************************
 * @file   Compare.h
 * @author jan
 * @date   3/8/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_COMPARE_H
#define NOVELIST_COMPARE_H

#include <type_traits>
#include <cstdint>

namespace novelist {
    namespace compare {
        using bigger_int_t = std::int_fast32_t;

        /**
         * Provides the signum of a numeric value
         * @tparam T Arithmetic type
         * @param val Value
         * @return -1 if value is negative, 1 if positive, otherwise 0
         */
        template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
        inline constexpr int signum(T val) noexcept
        {
            return (static_cast<T>(0) < val) - (val < static_cast<T>(0));
        }

        /**
         * Checks if a value is numerically less than another
         * @details This comparison works independent of signed/unsigned problems
         * @tparam T Type of first value
         * @tparam U Type of second value
         * @param a First value
         * @param b Second value
         * @return true if \p a is numerically less than \p b, otherwise false
         */
        template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
        inline constexpr bool less(T a, U b) noexcept
        {
            if constexpr (std::is_floating_point_v<T> || std::is_floating_point_v<U>)
                return a < b; // integer type converted to floating point type, this is fine
            if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
                return a < b; // both signed or unsigned, this is fine
            using uint_t = std::conditional_t<std::is_signed_v<T>, U, T>;
            using  int_t = std::conditional_t<std::is_signed_v<T>, T, U>;
            if constexpr (sizeof(int_t) > sizeof(uint_t))
                return a < b; // b will be promoted to signed type of a, this is fine
            if constexpr (sizeof(uint_t) < sizeof(int))
                return a < b; // both promoted to int, this is fine
            if constexpr (sizeof(uint_t) < sizeof(bigger_int_t))
            {
                // cast unsigned to bigger signed type
                if constexpr (std::is_signed_v<T>)
                    return a < static_cast<bigger_int_t>(b);
                else
                    return static_cast<bigger_int_t>(a) < b;
            }
            else
            {
                // don't know about bigger signed type, check via expression
                if constexpr (std::is_signed_v<T>)
                    return a < 0 || static_cast<uint_t>(a) < b;
                else
                    return b > 0 && a < static_cast<uint_t>(b);
            }
        }

        /**
         * Checks if a value is numerically greater than another
         * @details This comparison works independent of signed/unsigned problems
         * @tparam T Type of first value
         * @tparam U Type of second value
         * @param a First value
         * @param b Second value
         * @return true if \p a is numerically greater than \p b, otherwise false
         */
        template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
        inline constexpr bool greater(T a, U b) noexcept
        {
            return less(b, a);
        }

        /**
         * Checks if a value is numerically less or equal to another
         * @details This comparison works independent of signed/unsigned problems
         * @tparam T Type of first value
         * @tparam U Type of second value
         * @param a First value
         * @param b Second value
         * @return true if \p a is numerically less or equal to \p b, otherwise false
         */
        template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
        inline constexpr bool less_or_equal(T a, U b) noexcept
        {
            return less(a, b) || !less(b, a);
        }

        /**
         * Checks if a value is numerically greater or equal to another
         * @details This comparison works independent of signed/unsigned problems
         * @tparam T Type of first value
         * @tparam U Type of second value
         * @param a First value
         * @param b Second value
         * @return true if \p a is numerically greater or equal to \p b, otherwise false
         */
        template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
        inline constexpr bool greater_or_equal(T a, U b) noexcept
        {
            return less(b, a) || !less(a, b);
        }

        /**
         * Checks if a value is numerically unequal to another
         * @details This comparison works independent of signed/unsigned problems
         * @tparam T Type of first value
         * @tparam U Type of second value
         * @param a First value
         * @param b Second value
         * @return true if \p a is numerically unequal to \p b, otherwise false
         */
        template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
        inline constexpr bool not_equal(T a, U b) noexcept
        {
            return less(a, b) || less(b, a);
        }

        /**
         * Checks if a value is numerically equal to another
         * @details This comparison works independent of signed/unsigned problems
         * @tparam T Type of first value
         * @tparam U Type of second value
         * @param a First value
         * @param b Second value
         * @return true if \p a is numerically equal to \p b, otherwise false
         */
        template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
        inline constexpr bool equal(T a, U b) noexcept
        {
            return !not_equal(a, b);
        }
    }
}

#endif //NOVELIST_COMPARE_H
