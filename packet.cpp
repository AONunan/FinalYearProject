#include "packet.h"
#include <QDebug>

Packet::Packet()
{
    //qDebug() << "New packet created";
    //testing();

}

void Packet::testing() {
    qDebug() << "# Packet IP header length:" << ip_header_length;
    qDebug() << "# Packet TCP header length:" << tcp_header_length;
    qDebug() << "# Packet payload length:" << payload_length;
    qDebug() << "# Packet count:" << Packet::count;
    Packet::count++;
}

void Packet::set_identifier(QString name) {
    identifier = name;
}

QString Packet::get_identifier() {
    QString name = "Packet name is:" + identifier;
    return name;
}

void Packet::set_ip_header(int length) {
    ip_header_length = length;
}

void Packet::set_tcp_header(int length) {
    tcp_header_length = length;
}

void Packet::set_payload(int length) {
    payload_length = length;
}
