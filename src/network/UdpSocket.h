#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <memory>
#include <netinet/in.h>
#include <string>
#include <unistd.h>

class UdpSocket
{
    using UdpSocketUPtr = std::unique_ptr<UdpSocket>;

public:
    static UdpSocketUPtr Create();
    ~UdpSocket();

    int Bind(const std::string& ip_address, const unsigned short port);
    ssize_t ReceiveFrom(void* buffer, const size_t& buffer_size, sockaddr_in* sender_address, socklen_t& sender_address_size);

    ssize_t SendTo(const void* buffer, const size_t buffer_size, const std::string& ip_address, const unsigned short port);

private:    
    int descriptor_;

    UdpSocket();
};

using UdpSocketUPtr = std::unique_ptr<UdpSocket>;

#endif // UDPSOCKET_H
