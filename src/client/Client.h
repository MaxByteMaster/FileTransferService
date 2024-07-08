#ifndef CLIENT_H
#define CLIENT_H

#include <unordered_map>
#include <unordered_set>

#include "network/Packet.h"
#include "network/UdpSocket.h"

class Client
{
public:
    Client();
    ~Client();

    void SendFiles(const std::string& filePath);
    void SendFile(const std::string& filePath, UdpSocket udp_socket);

private:
    using IdToPacketUMap = std::unordered_map<uint32_t, PacketSPtr>;
    using IdToPacketUMapSPtr = std::shared_ptr<IdToPacketUMap>;
    std::unordered_map<FileID, IdToPacketUMapSPtr> file_id_to_packet_u_map_;

    std::unordered_map<FileID, uint32_t> file_id_to_total_packets_;

    std::unordered_set<PacketSPtr> packet_u_set_;
};

#endif // CLIENT_H
