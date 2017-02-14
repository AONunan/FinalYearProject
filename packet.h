#ifndef PACKET_H
#define PACKET_H

#include <QString> // TODO: remove once finished with identifier


class Packet
{
public:
    Packet();
    void display_packet_info();
    void set_ip_header(int length);
    void set_tcp_header(int length);
    void set_payload(int length);
    void set_identifier(QString name);
    QString get_identifier();
private:
    QString identifier;
    int ip_header_length;
    int tcp_header_length;
    int payload_length;

    static int count;
};

#endif // PACKET_H
