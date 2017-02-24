#include "packetinfodialog.h"
#include "ui_packetinfodialog.h"

#include <QDebug>

PacketInfoDialog::PacketInfoDialog(const Packet packet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketInfoDialog) {
    ui->setupUi(this);

    displayed_packet = packet;

    // Set IP fields
    ui->label_ip_version->setText(QString::number(displayed_packet.getIp_version()));
    // TODO: Fill out length
    ui->label_ip_type_of_service->setText(QString::number(displayed_packet.getIp_type_of_service()));
    // TODO: Fill out length
    ui->label_ip_id->setText(QString::number(displayed_packet.getIp_id()));
    // TODO: Fill out flags
    ui->label_ip_offset->setText(QString::number(displayed_packet.getIp_offset()));
    ui->label_ip_ttl->setText(QString::number(displayed_packet.getIp_time_to_live()));
    ui->label_ip_protocol->setText(QString::number(displayed_packet.getIp_protocol()));
    ui->label_ip_checksum->setText(QString::number(displayed_packet.getIp_checksum()));
    ui->label_ip_src_address->setText(displayed_packet.getIp_source_address());
    ui->label_ip_dst_address->setText(displayed_packet.getIp_destination_address());
    // TODO: Fill out options

    // Set TCP fields
    ui->label_tcp_src_port->setText(QString::number(displayed_packet.getTcp_source_port()));
    ui->label_tcp_dst_port->setText(QString::number(displayed_packet.getTcp_destination_port()));
    ui->label_tcp_sequence_number->setText(QString::number(displayed_packet.getTcp_sequence_number()));
    ui->label_tcp_ack_number->setText(QString::number(displayed_packet.getTcp_acknowledgement_number()));
    ui->label_tcp_offset->setText(QString::number(displayed_packet.getTcp_offset()));
    ui->label_tcp_flags->setText(QString::number(displayed_packet.getTcp_flags()));
    ui->label_tcp_window->setText(QString::number(displayed_packet.getTcp_window()));
    ui->label_tcp_checksum->setText(QString::number(displayed_packet.getTcp_checksum()));
    ui->label_tcp_urgent_ptr->setText(QString::number(displayed_packet.getTcp_urgent_pointer()));
    // TODO: Fill out options

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
