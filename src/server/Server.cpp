#include "Server.h"

#include <iostream>

#include "network/Packet.h"
#include "network/UdpSocket.h"

Server::Server()
{
    const auto udp_socket = UdpSocket::Create();
    if (!udp_socket)
        return;

    if(udp_socket->Bind("127.0.0.1", 2000) == -1)
        return;

    char buffer[kMaxPacketSize];
    std::fill_n(buffer, sizeof(buffer), 0);

    sockaddr_in sender_address;
    socklen_t sender_address_size = sizeof(sender_address);

    for (;;)
    {
        const auto res = udp_socket->ReceiveFrom(buffer, sizeof(buffer), &sender_address, sender_address_size);

        const auto packet = PacketUPtr(new Packet());
        packet->Deserialize(buffer);

        std::cout
            << "Received packet: " << packet->seq_number
            << " of file with id: " << packet->id.ToUInt64()
            << " with length: " << packet->data_size + kPacketHeaderSize
            << std::endl;
    }
}

int main()
{
    const auto server = Server();

    return 0;
}
