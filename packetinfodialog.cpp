#include "packetinfodialog.h"
#include "ui_packetinfodialog.h"

#include <QDebug>

PacketInfoDialog::PacketInfoDialog(const Packet packet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketInfoDialog) {
    ui->setupUi(this);

    displayed_packet = packet;

    ui->label_protocol->setText(displayed_packet.getProtocol());
    ui->label_src_host->setText(displayed_packet.getIp_source_address());
    ui->label_dst_host->setText(displayed_packet.getIp_destination_address());
    ui->label_src_port->setText(QString::number(displayed_packet.getTcp_source_port()));
    ui->label_dst_port->setText(QString::number(displayed_packet.getTcp_destination_port()));
    ui->label_ip_length->setText(QString::number(displayed_packet.getIp_header_length()));
    ui->label_tcp_length->setText(QString::number(displayed_packet.getTcp_header_length()));
    ui->label_payload_length->setText(QString::number(displayed_packet.getPayload_length()));
}

PacketInfoDialog::~PacketInfoDialog() {
    delete ui;
}

void PacketInfoDialog::on_pushButton_print_payload_clicked()
{
    int i;
    qDebug() << "Payload vect length:" << displayed_packet.getPayload_vect().length();

    for(i = 0; i < displayed_packet.getPayload_vect().length(); i++) {
        qDebug() << QString("%1").arg(displayed_packet.getPayload_vect()[i], 2, 16, QChar('0')).toUpper();
    }
}
