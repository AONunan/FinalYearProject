#ifndef PACKET_H
#define PACKET_H


class Packet
{
public:
    Packet();
    void testing();
    void set_ip_header(int length);
    void set_tcp_header(int length);
    void set_payload(int length);
private:
    int ip_header_length;
    int tcp_header_length;
    int payload_length;

    static int count;
};

#endif // PACKET_H
