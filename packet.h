#ifndef PACKET_H
#define PACKET_H

#include <QString>
#include <QVector>

#define ETHER_ADDR_LEN	6       /* Ethernet addresses are 6 bytes */


#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Packet {
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

    time_t getCurrent_time() const;
    void setCurrent_time(const time_t &value);

    QVector<short> getPayload_vect() const;
    void setPayload_vect(const QVector<short> &value);
    void clearPayload_vect();

    u_short getTcp_source_port() const;
    void setTcp_source_port(const u_short &value);

    u_short getTcp_destination_port() const;
    void setTcp_destination_port(const u_short &value);

    QString getIp_source_address() const;
    void setIp_source_address(const QString &value);

    QString getIp_destination_address() const;
    void setIp_destination_address(const QString &value);

    void reset_values();
    u_int getTcp_sequence_number() const;
    void setTcp_sequence_number(const u_int &value);

    u_int getTcp_acknowledgement_number() const;
    void setTcp_acknowledgement_number(const u_int &value);

    u_char getTcp_offset() const;
    void setTcp_offset(const u_char &value);

    u_char getTcp_flags() const;
    void setTcp_flags(const u_char &value);

    u_short getTcp_window() const;
    void setTcp_window(const u_short &value);

    u_short getTcp_checksum() const;
    void setTcp_checksum(const u_short &value);

    u_short getTcp_urgent_pointer() const;
    void setTcp_urgent_pointer(const u_short &value);

    u_char getIp_version() const;
    void setIp_version(const u_char &value);

    u_char getIp_type_of_service() const;
    void setIp_type_of_service(const u_char &value);

    u_short getIp_length() const;
    void setIp_length(const u_short &value);

    u_short getIp_id() const;
    void setIp_id(const u_short &value);

    u_short getIp_offset() const;
    void setIp_offset(const u_short &value);

    u_char getIp_time_to_live() const;
    void setIp_time_to_live(const u_char &value);

    u_char getIp_protocol() const;
    void setIp_protocol(const u_char &value);

    u_short getIp_checksum() const;
    void setIp_checksum(const u_short &value);

    QString getIp_protocol_string() const;

    QString getCurrent_time_string() const;
    void setCurrent_time_string();

private:
    int total_header_length;
    int ip_header_length;
    int tcp_header_length;
    int payload_length;
    QVector<short> payload_vect;
    QString protocol;
    time_t current_time;
    QString current_time_string;

    // Ethernet stuff
    u_char ethernet_destination_address[ETHER_ADDR_LEN];
    u_char ethernet_source_address[ETHER_ADDR_LEN];
    u_short ethernet_type; // IP, ARP, RARP, etc

    // IP stuff
    QString ip_source_address;
    QString ip_destination_address;
    u_char ip_version; // version << 4 | header length >> 2
    u_char ip_type_of_service;
    u_short ip_length;
    u_short ip_id;
    u_short ip_offset; // fragment offset field
    u_char ip_time_to_live;
    u_char ip_protocol;
    QString ip_protocol_string;
    u_short ip_checksum;

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
};

#endif // PACKET_H
