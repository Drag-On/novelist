/**********************************************************
 * @file   FlagSet.h
 * @author jan
 * @date   2/21/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_FLAGSET_H
#define NOVELIST_FLAGSET_H

#include <cstdint>
#include <gsl/gsl>


namespace novelist {

    namespace {
        template <typename Enum, typename...> struct all_same;

        template <typename Enum> struct all_same<Enum> : std::true_type { };

        template <typename Enum, typename T, typename ...Rest> struct all_same<Enum, T, Rest...>
                : std::integral_constant<bool, std::is_same<T, Enum>::value && all_same<Enum, Rest...>::value>
        { };
    }

    /**
     * Set of binary flags. Similar to std::bitset, but more convenient to use.
     * @tparam Flag Enumeration that is used to indicate flags
     * @tparam T Underlying datatype, defaults to uint32_t which allows for 32 flags total
     */
    template <typename Flag, typename T = uint32_t, typename = std::enable_if_t<std::is_integral_v<T>>>
    class FlagSet {
    public:
        /**
         * Construct a flag set with no bits set
         */
        constexpr FlagSet() noexcept = default;
        ~FlagSet() noexcept = default;
        constexpr FlagSet(FlagSet const& other) noexcept = default;
        constexpr FlagSet(FlagSet&& other) noexcept = default;

        /**
         * Construct a flag set with a number of bits set
         * @param flags Any number of enumerators
         */
        template <typename... Flag_Type, typename = std::enable_if_t<all_same<Flag, Flag_Type...>::value>>
        constexpr FlagSet(Flag_Type... flags) noexcept
                : m_flags((0 | ... | (1 << gsl::narrow<T>(flags))))
        {
        }

        constexpr FlagSet& operator=(FlagSet const& flags) noexcept = default;
        constexpr FlagSet& operator=(FlagSet&& flags) noexcept = default;

        /**
         * Test if any of the given flags are set
         * @param flags Any number of enumerators
         * @return True in case at least one of the given flags is set, otherwise false
         */
        template <typename... Flag_Type, typename = std::enable_if_t<all_same<Flag, Flag_Type...>::value>>
        constexpr bool testAnyOf(Flag_Type... flags) const noexcept
        {
            return (m_flags & (0 | ... | (1 << gsl::narrow<T>(flags))));
        }
        constexpr bool testAnyOf(FlagSet const& flags) const noexcept
        {
            return (m_flags & flags.m_flags);
        }

        /**
         * Test if all of the given flags are set
         * @param flags Any number of enumerators
         * @return True in case all of the given flags are set, otherwise false
         */
        template <typename... Flag_Type, typename = std::enable_if_t<all_same<Flag, Flag_Type...>::value>>
        constexpr bool testAllOf(Flag_Type... flags) const noexcept
        {
            T const mask = (0 | ... | (1 << gsl::narrow<T>(flags)));
            return (m_flags & mask) == mask;
        }
        constexpr bool testAllOf(FlagSet const& flags) const noexcept
        {
            return (m_flags & flags.m_flags) == flags.m_flags;
        }

        /**
         * Test if none of the given flags are set
         * @param flags Any number of enumerators
         * @return True in case none of the given flags are set, otherwise false
         */
        template <typename... Flag_Type, typename = std::enable_if_t<all_same<Flag, Flag_Type...>::value>>
        constexpr bool testNoneOf(Flag_Type... flags) const noexcept
        {
            return !testAnyOf(std::forward<Flag_Type>(flags)...);
        }
        constexpr bool testNoneOf(FlagSet const& flags) const noexcept
        {
            return !testAnyOf(flags);
        }

        /**
         * Test if a flag is set
         * @param flag Flag to check
         * @return True if set, otherwise false
         */
        constexpr bool test(Flag flag) const noexcept
        {
            return testAnyOf(flag);
        }

        /**
         * Test if any flag is set
         * @return True if at least one bit is set, otherwise false
         */
        constexpr bool testAny() const noexcept
        {
            return m_flags & ~0u;
        }

        /**
         * Test if all flags are set
         * @return True if all bits are set, otherwise false
         * @warning This might be confusing as it tests for all bits; however, if not every bit is used, for example
         *          because the amount of enumerators doesn't exactly match the number of bits in the underlying type,
         *          then this might return false even though the caller expects it to be true.
         */
        constexpr bool testAll() const noexcept
        {
            return m_flags == ~0u;
        }

        /**
         * Test if no flags are set
         * @return True if all bits are clear, otherwise false
         */
        constexpr bool testNone() const noexcept
        {
            return m_flags == 0;
        }

        /**
         * Clears all the given flags
         * @param flags Any number of enumerators
         */
        template <typename... Flag_Type, typename = std::enable_if_t<all_same<Flag, Flag_Type...>::value>>
        constexpr void clear(Flag_Type... flags) noexcept
        {
            m_flags &= ~(0 | ... | (1 << gsl::narrow<T>(flags)));
        }
        constexpr void clear(FlagSet const& flags) noexcept
        {
            m_flags &= ~flags.m_flags;
        }

        /**
         * Clears all flags
         */
        constexpr void clear() noexcept
        {
            m_flags = 0;
        }

        /**
         * Sets all given flags
         * @param flags Any number of enumerators
         */
        template <typename... Flag_Type, typename = std::enable_if_t<all_same<Flag, Flag_Type...>::value>>
        constexpr void set(Flag_Type... flags) noexcept
        {
            m_flags |= (0 | ... | (1 << gsl::narrow<T>(flags)));
        }
        constexpr void set(FlagSet const& flags) noexcept
        {
            m_flags |= flags.m_flags;
        }

        /**
         * Toggles all given flags
         * @param flags Any number of enumerators
         */
        template <typename... Flag_Type, typename = std::enable_if_t<all_same<Flag, Flag_Type...>::value>>
        constexpr void toggle(Flag_Type... flags) noexcept
        {
            m_flags ^= (0 | ... | (1 << gsl::narrow<T>(flags)));
        }
        constexpr void toggle(FlagSet const& flags) noexcept
        {
            m_flags ^= flags.m_flags;
        }

        /**
         * @return A copy of the underlying data
         */
        constexpr T data() const noexcept
        {
            return m_flags;
        }

    private:
        T m_flags = 0;
    };
}

#endif //NOVELIST_FLAGSET_H
