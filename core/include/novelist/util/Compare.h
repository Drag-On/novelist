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

        /**
         * Checks if the given numbers are non-decreasing from left to right, i.e. each value is greater or equal to
         * its predecessor.
         * @tparam T Type of first value
         * @tparam U Type of second value
         * @tparam Ts Types of other values
         * @param a First value
         * @param b Second value
         * @param numbers Other values
         * @return true if the given parameters are non-decreasing from left to right, otherwise false
         */
        template<typename T, typename U, typename... Ts>
        inline constexpr bool non_decreasing(T a, U b, Ts... numbers) noexcept
        {
            if constexpr (sizeof...(numbers) == 0)
                return less_or_equal(a, b);
            else
                return less_or_equal(a, b) && non_decreasing(b, std::forward<Ts>(numbers)...);
        }

        /**
         * Checks if the given numbers are strictly increasing from left to right, i.e. each value is greater than
         * its predecessor.
         * @tparam T Type of first value
         * @tparam U Type of second value
         * @tparam Ts Types of other values
         * @param a First value
         * @param b Second value
         * @param numbers Other values
         * @return true if the given parameters are strictly increasing from left to right, otherwise false
         */
        template<typename T, typename U, typename... Ts>
        inline constexpr bool increasing(T a, U b, Ts... numbers) noexcept
        {
            if constexpr (sizeof...(numbers) == 0)
                return less(a, b);
            else
                return less(a, b) && increasing(b, std::forward<Ts>(numbers)...);
        }

        /**
         * Checks if the given numbers are non-increasing from left to right, i.e. each value is less or equal to
         * its predecessor.
         * @tparam T Type of first value
         * @tparam U Type of second value
         * @tparam Ts Types of other values
         * @param a First value
         * @param b Second value
         * @param numbers Other values
         * @return true if the given parameters are non-increasing from left to right, otherwise false
         */
        template<typename T, typename U, typename... Ts>
        inline constexpr bool non_increasing(T a, U b, Ts... numbers) noexcept
        {
            if constexpr (sizeof...(numbers) == 0)
                return greater_or_equal(a, b);
            else
                return greater_or_equal(a, b) && non_increasing(b, std::forward<Ts>(numbers)...);
        }

        /**
         * Checks if the given numbers are strictly decreasing from left to right, i.e. each value is less than
         * its predecessor.
         * @tparam T Type of first value
         * @tparam U Type of second value
         * @tparam Ts Types of other values
         * @param a First value
         * @param b Second value
         * @param numbers Other values
         * @return true if the given parameters are strictly decreasing from left to right, otherwise false
         */
        template<typename T, typename U, typename... Ts>
        inline constexpr bool decreasing(T a, U b, Ts... numbers) noexcept
        {
            if constexpr (sizeof...(numbers) == 0)
                return greater(a, b);
            else
                return greater(a, b) && decreasing(b, std::forward<Ts>(numbers)...);
        }
    }
}

#endif //NOVELIST_COMPARE_H
