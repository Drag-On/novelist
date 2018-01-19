/**********************************************************
 * @file   StringHash.h
 * @author jan
 * @date   1/19/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_STRINGHASH_H
#define NOVELIST_STRINGHASH_H

#include <cstdint>
#include <array>

namespace novelist {

    struct CRC8 {
        using Hash_t = uint8_t;
        static constexpr uint32_t const polynomial = 0x07;
        static constexpr Hash_t const initialRemainder = 0x00;
        static constexpr Hash_t const finalXorValue = 0x00;
        static constexpr bool const reflectData = false;
        static constexpr bool const reflectRemainder = false;
    };
    struct CRC_CCITT {
        using Hash_t = uint16_t;
        static constexpr uint32_t const polynomial = 0x1021;
        static constexpr Hash_t const initialRemainder = 0xFFFF;
        static constexpr Hash_t const finalXorValue = 0x0000;
        static constexpr bool const reflectData = false;
        static constexpr bool const reflectRemainder = false;
    };
    struct CRC16 {
        using Hash_t = uint16_t;
        static constexpr uint32_t const polynomial = 0x8005;
        static constexpr Hash_t const initialRemainder = 0x0000;
        static constexpr Hash_t const finalXorValue = 0x0000;
        static constexpr bool const reflectData = true;
        static constexpr bool const reflectRemainder = true;
    };
    struct CRC32 {
        using Hash_t = uint32_t;
        static constexpr uint32_t const polynomial = 0x04C11DB7;
        static constexpr Hash_t const initialRemainder = 0xFFFFFFFF;
        static constexpr Hash_t const finalXorValue = 0xFFFFFFFF;
        static constexpr bool const reflectData = true;
        static constexpr bool const reflectRemainder = true;
    };

    namespace internal {
        template<typename T>
        constexpr T reverseBits(T input) noexcept
        {
            T output = input;
            for (size_t i = sizeof(input) * 8 - 1; i > 0; --i) {
                output <<= 1;
                input >>= 1;
                output |= input & 1;
            }
            return output;
        }

        template<typename Crc>
        constexpr const std::array<typename Crc::Hash_t, 256> makeCrcTable() noexcept
        {
            std::array<typename Crc::Hash_t, 256> crcTable{};
            constexpr int32_t const width = 8 * sizeof(typename Crc::Hash_t);
            constexpr int32_t const topBit = 1 << (width - 1);
            typename Crc::Hash_t remainder{};
            for (int dividend = 0; dividend < 256; ++dividend) {
                remainder = dividend << (width - 8);
                for (int8_t bit = 8; bit > 0; --bit) {
                    if (remainder & topBit)
                        remainder = (remainder << 1) ^ Crc::polynomial;
                    else
                        remainder = (remainder << 1);
                }
                crcTable.at(dividend) = remainder;
            }

            return crcTable;
        }

        template<typename Crc>
        constexpr const std::array<typename Crc::Hash_t, 256> crcTable = makeCrcTable<Crc>();
    }

    /**
     * Compute the CRC hash of a message
     * @tparam Crc CRC type
     * @param message Message
     * @param nBytes Message length in bytes
     * @return The hash
     */
    template<typename Crc = CRC32>
    constexpr typename Crc::Hash_t crcHash(char const* const message, size_t nBytes) noexcept
    {
        using namespace internal;

        uint8_t data{};
        constexpr int32_t const width = 8 * sizeof(typename Crc::Hash_t);
        typename Crc::Hash_t remainder = Crc::initialRemainder;

        for (size_t byte = 0; byte < nBytes; ++byte) {
            data = static_cast<uint8_t>(message[byte]);
            if constexpr (Crc::reflectData)
                data = reverseBits<uint8_t>(data);
            data ^= (remainder >> (width - 8));
            remainder = crcTable<Crc>[data] ^ (remainder << 8);
        }
        remainder ^= Crc::finalXorValue;

        if constexpr (Crc::reflectRemainder)
            remainder = reverseBits<typename Crc::Hash_t>(remainder);

        return remainder;
    }

    /**
     * Compute the CRC hash of a message
     * @tparam Crc CRC type
     * @param message Message
     * @return The hash
     */
    template<typename Crc = CRC32>
    constexpr typename Crc::Hash_t crcHash(std::string_view message) noexcept
    {
        char const* const dat = message.data();
        return crcHash<Crc>(dat, message.size());
    }

}

#endif //NOVELIST_STRINGHASH_H
