/**********************************************************
 * @file   CrcHashTest.cpp
 * @author jan
 * @date   1/19/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include "util/CrcHash.h"

using namespace novelist;

TEST_CASE("Reverse Bits", "[CrcHash]")
{
    using namespace internal;
    REQUIRE(reverseBits<uint8_t>(0b00000000) == 0b00000000);
    REQUIRE(reverseBits<uint8_t>(0b10000000) == 0b00000001);
    REQUIRE(reverseBits<uint8_t>(0b00000001) == 0b10000000);
    REQUIRE(reverseBits<uint8_t>(0b00100010) == 0b01000100);
    REQUIRE(reverseBits<uint16_t>(0b10000000'00000000) == 0b00000000'00000001);
}

TEST_CASE("CRC hash test", "[CrcHash]")
{
    CHECK(crcHash<CRC8>("123456789") == 0xf4);
    CHECK(crcHash<CRC_CCITT>("123456789") == 0x29B1);
    CHECK(crcHash<CRC16>("123456789") == 0xBB3D);
    CHECK(crcHash<CRC32>("123456789") == 0xCBF43926);
}