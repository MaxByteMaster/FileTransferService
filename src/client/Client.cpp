#include "client/Client.h"

#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <thread>

#include "network/UdpSocket.h"

constexpr auto kFilesDirString = "client_files";

Client::Client()
{
    const auto udp_socket = UdpSocket::Create();
    if (!udp_socket)
        return;

    const std::string files_dir_string = kFilesDirString;

    DIR* files_dir = opendir(files_dir_string.c_str());
    if (!files_dir)
        return;

    dirent* entry;
    while ((entry = readdir(files_dir)))
    {
        // Check if it is a file
        if (entry->d_type != DT_REG)
            continue;

        std::string file_path = files_dir_string + "/" + entry->d_name;
        SendFile(file_path, *udp_socket);
    }

    closedir(files_dir);
}

Client::~Client()
{

}

void Client::SendFile(const std::string& filePath, UdpSocket udp_socket)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    static uint64_t file_id = 0;

    uint32_t packet_counter = 0;
    char buffer[kMaxPacketDataSize];

    while (file.read(buffer, kMaxPacketDataSize) || file.gcount() > 0)
    {
        auto packet = PacketSPtr(new Packet());
        packet->seq_number = packet_counter++;
        packet->type = PacketType::PUT;
        packet->id.FromUInt64(file_id);
        packet->data_size = file.gcount();
        std::memcpy(packet->data, buffer, packet->data_size);

        const auto id_to_packet_u_map = file_id_to_packet_u_map_.find(packet->id);
        if (id_to_packet_u_map == file_id_to_packet_u_map_.end())
        {
            const auto id_to_packet_u_map = IdToPacketUMapSPtr(new IdToPacketUMap());
            id_to_packet_u_map->insert(std::make_pair(packet->seq_number, packet));

            file_id_to_packet_u_map_.insert(std::make_pair(packet->id, id_to_packet_u_map));
        }
        else
            id_to_packet_u_map->second->insert(std::make_pair(packet->seq_number, packet));
    }

    file.close();

    const auto id_to_packet_u_map = file_id_to_packet_u_map_[FileID(file_id)];
    for (auto& id_to_packet : *id_to_packet_u_map)
    {
        id_to_packet.second->seq_total = packet_counter;

        packet_u_set_.insert(id_to_packet.second);
    }

    file_id_to_total_packets_.insert(std::make_pair(FileID(file_id), packet_counter));

    for (auto& packet : packet_u_set_)
    {
        char buffer[kMaxPacketSize];
        packet->Serialize(buffer);

        if (udp_socket.SendTo(buffer, packet->data_size + kPacketHeaderSize, "127.0.0.1", 2000) == -1)
            std::cerr << "Failed to send file: " << filePath << " " << strerror(errno) << std::endl;

        std::cout << "Sent packet " << packet->seq_number << " of file with id: " << packet->id.ToUInt64() << std::endl;
    }
}

int main()
{
    const auto client = Client();

    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}
