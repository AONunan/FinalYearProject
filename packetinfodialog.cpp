#include "packetinfodialog.h"
#include "ui_packetinfodialog.h"

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
}

PacketInfoDialog::~PacketInfoDialog() {
    delete ui;
}
