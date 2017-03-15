#include "packet.h"
#include <QDebug>

Packet::Packet() {
    reset_values();
}

QString Packet::timestamp_to_string(int input_time) {
    // Get packet timestamp
    tm *ltm;
    long long_Time = (long)input_time;
    ltm = localtime(&long_Time);

    return QString("%1:%2:%3").arg(ltm->tm_hour, 2, 10, QChar('0')).arg(ltm->tm_min, 2, 10, QChar('0')).arg(ltm->tm_sec, 2, 10, QChar('0')); // Get the timestamp in the format HH:MM:SS. The arguments pad the digits with zeros;

}

void Packet::reset_values() {
    total_header_length = 0;
    ip_header_length = 0;
    tcp_header_length = 0;
    payload_length = 0;
    protocol = "unknown";
    current_time = 0;
}

u_int Packet::getTcp_sequence_number() const {
    return tcp_sequence_number;
}

void Packet::setTcp_sequence_number(const u_int &value) {
    tcp_sequence_number = value;
}

u_int Packet::getTcp_acknowledgement_number() const {
    return tcp_acknowledgement_number;
}

void Packet::setTcp_acknowledgement_number(const u_int &value) {
    tcp_acknowledgement_number = value;
}

u_char Packet::getTcp_offset() const {
    return tcp_offset;
}

void Packet::setTcp_offset(const u_char &value) {
    tcp_offset = value;
}

u_char Packet::getTcp_flags() const {
    return tcp_flags;
}

void Packet::setTcp_flags(const u_char &value) {
    tcp_flags = value;
}

u_short Packet::getTcp_window() const {
    return tcp_window;
}

void Packet::setTcp_window(const u_short &value) {
    tcp_window = value;
}

u_short Packet::getTcp_checksum() const {
    return tcp_checksum;
}

void Packet::setTcp_checksum(const u_short &value) {
    tcp_checksum = value;
}

u_short Packet::getTcp_urgent_pointer() const {
    return tcp_urgent_pointer;
}

void Packet::setTcp_urgent_pointer(const u_short &value) {
    tcp_urgent_pointer = value;
}

u_char Packet::getIp_version() const {
    return ip_version;
}

void Packet::setIp_version(const u_char &value) {
    ip_version = value;
}

u_char Packet::getIp_type_of_service() const {
    return ip_type_of_service;
}

void Packet::setIp_type_of_service(const u_char &value) {
    ip_type_of_service = value;
}

u_short Packet::getIp_length() const {
    return ip_length;
}

void Packet::setIp_length(const u_short &value) {
    ip_length = value;
}

u_short Packet::getIp_id() const {
    return ip_id;
}

void Packet::setIp_id(const u_short &value) {
    ip_id = value;
}

u_short Packet::getIp_offset() const {
    return ip_offset;
}

void Packet::setIp_offset(const u_short &value) {
    ip_offset = value;
}

u_char Packet::getIp_time_to_live() const {
    return ip_time_to_live;
}

void Packet::setIp_time_to_live(const u_char &value) {
    ip_time_to_live = value;
}

u_char Packet::getIp_protocol() const {
    return ip_protocol;
}

void Packet::setIp_protocol(const u_char &value) {
    ip_protocol = value;

    switch(ip_protocol) {
    case 1:
        ip_protocol_string = "Internet Control Message Protocol (ICMP)";
        break;
    case 6:
        ip_protocol_string = "Transmission Control Protocol (TCP)";
        break;
    case 17:
        ip_protocol_string = "User Datagram Protocol (UDP)";
        break;
    case 47:
        ip_protocol_string = "General Routing Encapsulation (PPTP data over GRE)";
        break;
    case 51:
        ip_protocol_string = "Authentication Header (AH) IPSec";
        break;
    case 50:
        ip_protocol_string = "Encapsulation Security Payload (ESP) IPSec";
        break;
    case 8:
        ip_protocol_string = "Exterior Gateway Protocol (EGP)";
        break;
    case 3:
        ip_protocol_string = "Gateway-Gateway Protocol (GGP)";
        break;
    case 20:
        ip_protocol_string = "Host Monitoring Protocol (HMP)";
        break;
    case 88:
        ip_protocol_string = "Internet Group Management Protocol (IGMP)";
        break;
    case 66:
        ip_protocol_string = "MIT Remote Virtual Disk (RVD)";
        break;
    case 89:
        ip_protocol_string = "OSPF Open Shortest Path First";
        break;
    case 12:
        ip_protocol_string = "PARC Universal Packet Protocol (PUP)";
        break;
    case 27:
        ip_protocol_string = "Reliable Datagram Protocol (RDP)";
        break;
    case 46:
        ip_protocol_string = "Reservation Protocol (RSVP) QoS";
        break;
    default:
        ip_protocol_string = "unknown";
    }
}

u_short Packet::getIp_checksum() const {
    return ip_checksum;
}

void Packet::setIp_checksum(const u_short &value) {
    ip_checksum = value;
}

QString Packet::getIp_protocol_string() const {
    return ip_protocol_string;
}

QString Packet::tcp_port_to_string(u_short port) {
    switch(port) {
    case 20:
    case 21:
        return "FTP";
        break;
    case 22:
        return "SSH";
        break;
    case 23:
        return "Telnet";
        break;
    case 25:
        return "SMTP";
        break;
    case 53:
        return "DNS";
        break;
    case 67:
    case 68:
        return "DHCP";
        break;
    case 80:
        return "HTTP";
        break;
    case 110:
        return "POP3";
        break;
    case 443:
        return "HTTPS";
        break;
    default:
        return "unknown";
    }
}

short Packet::getTcp_window_scale() const {
    return tcp_window_scale;
}

void Packet::setTcp_window_scale(const short &value) {
    tcp_window_scale = value;
}

int Packet::getIp_header_length() const {
    return ip_header_length;
}

void Packet::setIp_header_length(int value) {
    ip_header_length = value;
}

int Packet::getTcp_header_length() const {
    return tcp_header_length;
}

void Packet::setTcp_header_length(int value) {
    tcp_header_length = value;
}

int Packet::getPayload_length() const {
    return payload_length;
}

void Packet::setPayload_length(int value) {
    payload_length = value;
}

QString Packet::getProtocol() const {
    return protocol;
}

void Packet::setProtocol(const QString &value) {
    protocol = value;
}

int Packet::getTotal_header_length() const {
    return total_header_length;
}

void Packet::setTotal_header_length(int value) {
    total_header_length = value;
}

time_t Packet::getCurrent_time() const {
    return current_time;
}

void Packet::setCurrent_time(const time_t &value) {
    current_time = value;
}

QVector<short> Packet::getPayload_vect() const {
    return payload_vect;
}

void Packet::setPayload_vect(const QVector<short> &value) {
    payload_vect = value;
}

/*
 * Necessary to clear vector with payload length 0.
 * Otherwise it gets populated with payload content when it shouldn't
 */
void Packet::clearPayload_vect() {
    payload_vect.clear();
}

u_short Packet::getTcp_source_port() const {
    return tcp_source_port;
}

void Packet::setTcp_source_port(const u_short &value) {
    tcp_source_port = value;
}

u_short Packet::getTcp_destination_port() const {
    return tcp_destination_port;
}

void Packet::setTcp_destination_port(const u_short &value) {
    tcp_destination_port = value;
}

QString Packet::getIp_source_address() const {
    return ip_source_address;
}

void Packet::setIp_source_address(const QString &value) {
    ip_source_address = value;
}

QString Packet::getIp_destination_address() const {
    return ip_destination_address;
}

void Packet::setIp_destination_address(const QString &value) {
    ip_destination_address = value;
}
