#include <cstdint>
#include <cstring>
#include <memory>

constexpr auto kMaxPacketSize = 1472;
constexpr auto kPacketHeaderSize = 19;
constexpr auto kMaxPacketDataSize = kMaxPacketSize - kPacketHeaderSize;

struct FileID
{
    uint8_t bytes[8];

    FileID() {}
    explicit FileID(const uint64_t value)
    {
        FromUInt64(value);
    }

    void FromUInt64(const uint64_t value)
    {
        bytes[0] = (value >> 56) & 0xFF;
        bytes[1] = (value >> 48) & 0xFF;
        bytes[2] = (value >> 40) & 0xFF;
        bytes[3] = (value >> 32) & 0xFF;
        bytes[4] = (value >> 24) & 0xFF;
        bytes[5] = (value >> 16) & 0xFF;
        bytes[6] = (value >> 8) & 0xFF;
        bytes[7] = value & 0xFF;
    }

    uint64_t ToUInt64()
    {
        uint64_t result = 0;
        result |= static_cast<uint64_t>(bytes[0]) << 56;
        result |= static_cast<uint64_t>(bytes[1]) << 48;
        result |= static_cast<uint64_t>(bytes[2]) << 40;
        result |= static_cast<uint64_t>(bytes[3]) << 32;
        result |= static_cast<uint64_t>(bytes[4]) << 24;
        result |= static_cast<uint64_t>(bytes[5]) << 16;
        result |= static_cast<uint64_t>(bytes[6]) << 8;
        result |= static_cast<uint64_t>(bytes[7]);
        return result;
    }

    bool operator==(const FileID& other) const
    {
        for (int i = 0; i < 8; ++i)
            if (bytes[i] != other.bytes[i])
                return false;

        return true;
    }
};

// Специализация структуры std::hash для FileID
namespace std
{

    template<>
    struct hash<FileID>
    {
        std::size_t operator()(const FileID& fileID) const noexcept
        {
            std::size_t hashValue = 0;
            for (size_t i = 0; i < sizeof(fileID.bytes); ++i)
                hashValue = hashValue * 31 + std::hash<uint8_t>()(fileID.bytes[i]);

            return hashValue;
        }
    };

} // namespace std

enum PacketType : uint8_t
{
    ACK,
    PUT
};

struct Packet
{
    uint32_t seq_number;
    uint32_t seq_total;
    PacketType type;
    FileID id;
    /* The protocol does not establish a way to determine the end of
    a packet, so I chose using data size */
    uint16_t data_size;
    char data[kMaxPacketDataSize];

    void Deserialize(char* buffer)
    {
        auto offset = 0;

        std::memcpy(&seq_number, buffer, sizeof(seq_number));
        offset += sizeof(seq_number);
        std::memcpy(&seq_total, &buffer[offset], sizeof(seq_total));
        offset += sizeof(seq_total);
        std::memcpy(&type, &buffer[offset], sizeof(type));
        offset += sizeof(type);
        std::memcpy(&id, &buffer[offset], sizeof(id));
        offset += sizeof(id);
        std::memcpy(&data_size, &buffer[offset], sizeof(data_size));
        offset += sizeof(data_size);
        std::memcpy(&data, &buffer[offset], data_size);
    }

    void Serialize(char* buffer)
    {
        auto offset = 0;

        std::memcpy(buffer, &seq_number, sizeof(seq_number));
        offset += sizeof(seq_number);
        std::memcpy(&buffer[offset], &seq_total, sizeof(seq_total));
        offset += sizeof(seq_total);
        std::memcpy(&buffer[offset], &type, sizeof(type));
        offset += sizeof(type);
        std::memcpy(&buffer[offset], &id, sizeof(id));
        offset += sizeof(id);
        std::memcpy(&buffer[offset], &data_size, sizeof(data_size));
        offset += sizeof(data_size);
        std::memcpy(&buffer[offset], &data, data_size);
    }
};

using PacketUPtr = std::unique_ptr<Packet>;
using PacketSPtr = std::shared_ptr<Packet>;

