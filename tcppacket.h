#ifndef TCPPACKET_H
#define TCPPACKET_H

#include "packet.h"

class TcpPacket : public Packet
{
public:
    TcpPacket();


private:
    typedef u_int tcp_seq;

    u_short tcp_source_port;
    u_short tcp_destination_port;
    tcp_seq tcp_sequence_number;
    tcp_seq tcp_acknowledgement_number;
    u_char tcp_offset; // data offset, rsvd
    u_char tcp_flags;
    u_short tcp_window;
    u_short tcp_checksum;
    u_short tcp_urgent_pointer;
};

#endif // TCPPACKET_H
