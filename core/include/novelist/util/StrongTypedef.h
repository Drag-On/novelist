/**********************************************************
 * @file   StrongTypedef.h
 * @author jan
 * @date   1/19/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_STRONGTYPEDEF_H
#define NOVELIST_STRONGTYPEDEF_H

#include <cstddef>
#include <string_view>
#include "CrcHash.h"

namespace novelist {
#define STRONG_TYPEDEF(alias, type) using alias = StrongTypedef< \
    type, novelist::crcHash(std::string_view(#alias, novelist::internal::strlen(#alias)))>

    namespace internal {
        template<size_t N>
        constexpr size_t strlen(char const (&)[N])
        {
            return N - 1;
        }

        template<typename T, size_t I>
        class BasicStrongTypedef {
        private:
            T m_data{};
        public:
            constexpr BasicStrongTypedef() = default;

            ~BasicStrongTypedef() = default;

            explicit constexpr BasicStrongTypedef(T val) noexcept
                    :m_data(val) { }

            constexpr BasicStrongTypedef(BasicStrongTypedef<T, I> const& other) noexcept
                    :m_data(other.m_data) { }

            constexpr BasicStrongTypedef(BasicStrongTypedef<T, I>&& other) noexcept
                    :m_data(other.m_data) { }

            explicit constexpr operator T() noexcept
            {
                return m_data;
            };

            constexpr BasicStrongTypedef<T, I>& operator=(T val) noexcept
            {
                m_data = val;
                return *this;
            };

            constexpr BasicStrongTypedef<T, I>& operator=(BasicStrongTypedef<T, I> other) noexcept
            {
                m_data = other.m_data;
                return *this;
            };

            constexpr BasicStrongTypedef<T, I>& operator=(BasicStrongTypedef<T, I>&& other) noexcept
            {
                m_data = other.m_data;
                return *this;
            };

            constexpr bool operator==(BasicStrongTypedef<T, I> other) noexcept
            {
                return m_data == other.m_data;
            }

            constexpr bool operator!=(BasicStrongTypedef<T, I> other) noexcept
            {
                return m_data != other.m_data;
            }

            constexpr bool operator<(BasicStrongTypedef<T, I> other) noexcept
            {
                return m_data < other.m_data;
            }

            constexpr bool operator<=(BasicStrongTypedef<T, I> other) noexcept
            {
                return m_data <= other.m_data;
            }

            constexpr bool operator>(BasicStrongTypedef<T, I> other) noexcept
            {
                return m_data > other.m_data;
            }

            constexpr bool operator>=(BasicStrongTypedef<T, I> other) noexcept
            {
                return m_data >= other.m_data;
            }
        };
    }

    template<typename T, size_t I>
    class StrongTypedef : public T { // TODO: Implement for custom types
    };

#define STRONG_TYPEDEF_GEN_BASIC_TYPE(T) \
    template<size_t I> \
    class StrongTypedef<T, I> : public internal::BasicStrongTypedef<T, I> { \
        using internal::BasicStrongTypedef<T, I>::BasicStrongTypedef; \
        using internal::BasicStrongTypedef<T, I>::operator=; \
    }

    STRONG_TYPEDEF_GEN_BASIC_TYPE(void);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(std::nullptr_t);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(bool);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(signed char);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(unsigned char);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(char);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(wchar_t);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(char16_t);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(char32_t);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(short int);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(unsigned short int);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(int);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(unsigned int);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(long int);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(unsigned long int);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(long long int);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(unsigned long long int);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(float);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(double);
    STRONG_TYPEDEF_GEN_BASIC_TYPE(long double);

}

#endif //NOVELIST_STRONGTYPEDEF_H
