#pragma once

#include <cstdint>
#include <vector>

namespace ccsds
{
    // Struct representing a CCSDS Header
    struct CCSDSHeader
    {
        uint8_t raw[6];
        uint8_t version;
        bool type;
        bool secondary_header_flag;
        uint16_t apid;
        uint8_t sequence_flag;
        uint16_t packet_sequence_count;
        uint16_t packet_length;

        CCSDSHeader();
        CCSDSHeader(uint8_t *rawi);
        CCSDSHeader(const CCSDSHeader &v) noexcept;
        void encodeHDR();

        CCSDSHeader &operator=(const CCSDSHeader &v) noexcept;
    };

    // Struct representing
    struct CCSDSPacket
    {
        CCSDSHeader header;
        std::vector<uint8_t> payload;
        void encodeHDR();

        CCSDSPacket() {}
        CCSDSPacket(const CCSDSPacket &v) noexcept;

        CCSDSPacket &operator=(const CCSDSPacket &v) noexcept;
    };

    // Parse CCSDS header
    CCSDSHeader parseCCSDSHeader(uint8_t *header);
} // namespace ccsds