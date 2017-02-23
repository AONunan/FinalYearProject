#include "packetinfodialog.h"
#include "ui_packetinfodialog.h"

#include <QDebug>

PacketInfoDialog::PacketInfoDialog(const Packet packet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketInfoDialog) {
    ui->setupUi(this);

    ui->label_protocol->setText(packet.getProtocol());
    ui->label_src_host->setText("SOURCE HOST HERE");
    ui->label_dst_host->setText("DESTINATION HOST HERE");
    ui->label_src_port->setText("SOURCE PORT HERE");
    ui->label_dst_port->setText("DESTINATION PORT HERE");
    ui->label_ip_length->setText(QString::number(packet.getIp_header_length()));
    ui->label_tcp_length->setText(QString::number(packet.getTcp_header_length()));
    ui->label_payload_length->setText(QString::number(packet.getPayload_length()));

    int i;

    for(i = 0; i < packet.getPayload_vect().length(); i++) {
        qDebug() << "...";
        qDebug() << packet.getPayload_vect()[i];
        qDebug() << QString("%1").arg(packet.getPayload_vect()[i], 2, 16, QChar('0'));
    }
}

PacketInfoDialog::~PacketInfoDialog() {
    delete ui;
}
