#include "UdpSocket.h"

#include <arpa/inet.h>

UdpSocket::UdpSocket()
{
    descriptor_ = socket(AF_INET, SOCK_DGRAM, 0);
}

UdpSocket::UdpSocketUPtr UdpSocket::Create()
{
    auto instance = UdpSocketUPtr(new UdpSocket());
    if (instance->descriptor_ == -1)
        return nullptr;

    return instance;
}

UdpSocket::~UdpSocket()
{
    close(descriptor_);
}

int UdpSocket::Bind(const std::string& ip_address, const unsigned short port)
{
    sockaddr_in socket_address;
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = inet_addr(ip_address.c_str());
    socket_address.sin_port = htons(port);

    return bind(descriptor_, reinterpret_cast<sockaddr*>(&socket_address), sizeof(socket_address));
}

ssize_t UdpSocket::ReceiveFrom(void* buffer, const size_t& buffer_size, sockaddr_in* sender_address, socklen_t& sender_address_size)
{
    return recvfrom(descriptor_, buffer, buffer_size, 0, reinterpret_cast<sockaddr*>(&sender_address), &sender_address_size);
}

ssize_t UdpSocket::SendTo(const void* buffer, const size_t buffer_size, const std::string& ip_address, const unsigned short port)
{
    sockaddr_in socket_address;
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = inet_addr(ip_address.c_str());
    socket_address.sin_port = htons(port);

    return sendto(descriptor_, buffer, buffer_size, 0, reinterpret_cast<sockaddr*>(&socket_address), sizeof(socket_address));
}
