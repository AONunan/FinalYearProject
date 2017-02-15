#include "packet.h"
#include <QDebug>

Packet::Packet()
{
    payload_length = 0;
    Packet::count++;
}

int Packet::getIp_header_length() const
{
    return ip_header_length;
}

void Packet::setIp_header_length(int value)
{
    ip_header_length = value;
}

int Packet::getPayload_length() const
{
    return payload_length;
}

void Packet::setPayload_length(int value)
{
    payload_length = value;
}

QString Packet::getPayload() const
{
    return payload;
}

void Packet::setPayload(const QString &value)
{
    payload = value;
}
