#include "tcppacket.h"

TcpPacket::TcpPacket()
{

}

int TcpPacket::getTcp_header_length() const
{
    return tcp_header_length;
}

void TcpPacket::setTcp_header_length(int value)
{
    tcp_header_length = value;
}
