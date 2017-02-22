#include "packet.h"
#include <QDebug>

Packet::Packet() {
    total_header_length = 0;
    ip_header_length = 0;
    tcp_header_length = 0;
    payload_length = 0;
    protocol = "unknown";
    current_time = time(0);
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

QString Packet::getPayload() const {
    return payload;
}

void Packet::setPayload(const QString &value) {
    payload = value;
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

time_t Packet::getCurrent_time() const
{
    return current_time;
}
