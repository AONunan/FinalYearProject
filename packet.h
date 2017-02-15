#ifndef PACKET_H
#define PACKET_H

#include <QString> // TODO: remove once finished with identifier
#include <QByteArray>

#define ETHER_ADDR_LEN	6       /* Ethernet addresses are 6 bytes */


#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Packet
{
public:
    Packet();
    int getIp_header_length() const;
    void setIp_header_length(int value);

    int getTcp_header_length() const;
    void setTcp_header_length(int value);

    int getPayload_length() const;
    void setPayload_length(int value);

    QString getPayload() const;
    void setPayload(const QString &value);

    QString getProtocol() const;
    void setProtocol(const QString &value);

    int getTotal_header_length() const;
    void setTotal_header_length(int value);

private:
    int total_header_length;
    int ip_header_length;
    int tcp_header_length;
    int payload_length;
    QString protocol;
    QString payload;
    QByteArray byte_array;

    // Ethernet stuff
    u_char ethernet_destination_address[ETHER_ADDR_LEN];
    u_char ethernet_source_address[ETHER_ADDR_LEN];
    u_short ethernet_type; // IP, ARP, RARP, etc

    // IP stuff
    u_char ip_version; // version << 4 | header length >> 2
    u_char ip_type_of_service;
    u_short ip_length;
    u_short ip_id;
    u_short ip_offset; // fragment offset field
    u_char ip_time_to_live;
    u_char ip_protocol;
    u_short ip_checksum;
    struct in_addr ip_source_address, ip_destination_address;

    // TCP stuff
    u_short tcp_source_port;
    u_short tcp_destination_port;
    u_int tcp_sequence_number;
    u_int tcp_acknowledgement_number;
    u_char tcp_offset; // data offset, rsvd
    u_char tcp_flags;
    u_short tcp_window;
    u_short tcp_checksum;
    u_short tcp_urgent_pointer;

    static int count;
};

#endif // PACKET_H
