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
    }

    template<typename T, size_t I>
    class StrongTypedef : public T { // TODO: Implement for custom types
    };

#define STRONG_TYPEDEF_GEN_BASIC_TYPE(T) \
    template<size_t I> \
    class StrongTypedef<T, I> { \
    private: \
        T m_data{}; \
    public: \
        constexpr StrongTypedef() = default; \
        ~StrongTypedef() = default; \
        explicit constexpr StrongTypedef(T val) noexcept \
                :m_data(val) { } \
        constexpr StrongTypedef(StrongTypedef<T, I> const& other) noexcept \
                :m_data(other.m_data) { } \
        constexpr StrongTypedef(StrongTypedef<T, I>&& other) noexcept \
                :m_data(other.m_data) { } \
        explicit inline constexpr operator T() const noexcept \
        { \
            return m_data; \
        }; \
        constexpr inline StrongTypedef<T, I>& operator=(T val) noexcept \
        { \
            m_data = val; \
            return *this; \
        }; \
        constexpr inline StrongTypedef<T, I>& operator=(StrongTypedef<T, I> const& other) noexcept \
        { \
            m_data = other.m_data; \
            return *this; \
        }; \
        constexpr inline StrongTypedef<T, I>& operator=(StrongTypedef<T, I>&& other) noexcept \
        { \
            m_data = other.m_data; \
            return *this; \
        }; \
        constexpr inline StrongTypedef<T, I>& operator+=(StrongTypedef<T, I> const& other) noexcept \
        { \
            m_data += other.m_data; \
            return *this; \
        }; \
        constexpr inline StrongTypedef<T, I> operator+(StrongTypedef<T, I> const& other) const noexcept \
        { \
            auto temp = *this; \
            temp += other; \
            return temp; \
        }; \
        constexpr inline StrongTypedef<T, I>& operator-=(StrongTypedef<T, I> const& other) noexcept \
        { \
            m_data -= other.m_data; \
            return *this; \
        }; \
        constexpr inline StrongTypedef<T, I> operator-(StrongTypedef<T, I> const& other) const noexcept \
        { \
            auto temp = *this; \
            temp -= other; \
            return temp; \
        }; \
        constexpr inline StrongTypedef<T, I>& operator*=(StrongTypedef<T, I> const& other) noexcept \
        { \
            m_data *= other.m_data; \
            return *this; \
        }; \
        constexpr inline StrongTypedef<T, I> operator*(StrongTypedef<T, I> const& other) const noexcept \
        { \
            auto temp = *this; \
            temp *= other; \
            return temp; \
        }; \
        constexpr inline StrongTypedef<T, I>& operator/=(StrongTypedef<T, I> const& other) noexcept \
        { \
            m_data /= other.m_data; \
            return *this; \
        }; \
        constexpr inline StrongTypedef<T, I> operator/(StrongTypedef<T, I> const& other) const noexcept \
        { \
            auto temp = *this; \
            temp /= other; \
            return temp; \
        }; \
        constexpr inline StrongTypedef<T, I> operator-() const noexcept \
        { \
            auto temp = *this; \
            temp.m_data = - temp.m_data; \
            return temp; \
        }; \
        constexpr inline StrongTypedef<T, I>& operator++() noexcept \
        { \
            ++m_data; \
            return *this; \
        }; \
        constexpr inline StrongTypedef<T, I> operator++(int) noexcept \
        { \
            m_data++; \
            return *this; \
        }; \
        constexpr inline StrongTypedef<T, I>& operator--() noexcept \
        { \
            --m_data; \
            return *this; \
        }; \
        constexpr inline StrongTypedef<T, I> operator--(int) noexcept \
        { \
            m_data--; \
            return *this; \
        }; \
        constexpr inline bool operator==(StrongTypedef<T, I> const& other) const noexcept \
        { \
            return m_data == other.m_data; \
        } \
        constexpr inline bool operator!=(StrongTypedef<T, I> const& other) const noexcept \
        { \
            return m_data != other.m_data; \
        } \
        constexpr inline bool operator<(StrongTypedef<T, I> const& other) const noexcept \
        { \
            return m_data < other.m_data; \
        } \
        constexpr inline bool operator<=(StrongTypedef<T, I> const& other) const noexcept \
        { \
            return m_data <= other.m_data; \
        } \
        constexpr inline bool operator>(StrongTypedef<T, I> const& other) const noexcept \
        { \
            return m_data > other.m_data; \
        } \
        constexpr inline bool operator>=(StrongTypedef<T, I> const& other) const noexcept \
        { \
            return m_data >= other.m_data; \
        } \
    }

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
