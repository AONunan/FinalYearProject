#include "packetinfodialog.h"
#include "ui_packetinfodialog.h"

#include <QDebug>

PacketInfoDialog::PacketInfoDialog(const Packet packet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketInfoDialog) {
    ui->setupUi(this);

    displayed_packet = packet;

    show_header_details();

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

void PacketInfoDialog::on_pushButton_change_view_clicked() {
    if(currently_showing_field_names) {
        show_header_details();
    } else {
        show_header_field_names();
    }
}

void PacketInfoDialog::show_header_field_names() {
    currently_showing_field_names = true;

    // Set IP fields
    ui->label_ip_version->setText("Version");
    ui->label_ip_header_length->setText("Header Length");
    ui->label_ip_type_of_service->setText("Type of Service");
    ui->label_ip_total_length->setText("Total Length");
    ui->label_ip_id->setText("Identification");
    ui->label_ip_flags->setText("Flags");
    ui->label_ip_offset->setText("Fragment Offset");
    ui->label_ip_ttl->setText("Time To Live");
    ui->label_ip_protocol->setText("Protocol");
    ui->label_ip_checksum->setText("Header Checksum");
    ui->label_ip_src_address->setText("Source Address");
    ui->label_ip_dst_address->setText("Destination Address");
    ui->label_ip_options->setText("Options");

    // Set TCP fields
    ui->label_tcp_src_port->setText("Source Port");
    ui->label_tcp_dst_port->setText("Destination Port");
    ui->label_tcp_sequence_number->setText("Sequence Number");
    ui->label_tcp_ack_number->setText("Acknowledgement Number");
    ui->label_tcp_offset->setText("Offset");
    ui->label_tcp_reserved->setText("Reserved");
    ui->label_tcp_flags->setText("Flags");
    ui->label_tcp_window->setText("Window");
    ui->label_tcp_checksum->setText("Checksum");
    ui->label_tcp_urgent_ptr->setText("Urgent Pointer");
    ui->label_tcp_options->setText("Options");
}

void PacketInfoDialog::show_header_details() {
    currently_showing_field_names = false;

    // Set IP fields
    ui->label_ip_version->setText(QString::number(displayed_packet.getIp_version()));

    ui->label_ip_header_length->setText(QString::number(displayed_packet.getIp_header_length())); // TODO: Fill out length
    ui->label_ip_header_length->setToolTip(QString("(4 bits)\n\nThe length of the IP header, given in 32 bit words. In this case, the IP header = %1 * 32 = %2 bits (%3 bytes)").arg(displayed_packet.getIp_header_length()).arg(displayed_packet.getIp_header_length() * 32).arg(displayed_packet.getIp_header_length() * 32 / 8));

    ui->label_ip_type_of_service->setText(QString::number(displayed_packet.getIp_type_of_service()));
    // Tool tip set in UI file

    ui->label_ip_total_length->setText(QString::number(displayed_packet.getIp_length())); // TODO: Fill out length

    ui->label_ip_id->setText(QString::number(displayed_packet.getIp_id()));
    // Tool tip set in UI file

    // TODO: Fill out flags

    ui->label_ip_offset->setText(QString::number(displayed_packet.getIp_offset()));

    ui->label_ip_ttl->setText(QString::number(displayed_packet.getIp_time_to_live()));

    ui->label_ip_protocol->setText(QString::number(displayed_packet.getIp_protocol()));
    ui->label_ip_protocol->setToolTip(QString("(8 bits)\n\n%1").arg(displayed_packet.getIp_protocol_string()));

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
    ui->label_tcp_flags->setText(QString("0x%1").arg(displayed_packet.getTcp_flags(), 2, 16, QChar('0')).toUpper()); // Display flags in Hex
    ui->label_tcp_window->setText(QString::number(displayed_packet.getTcp_window()));
    ui->label_tcp_checksum->setText(QString::number(displayed_packet.getTcp_checksum()));
    ui->label_tcp_urgent_ptr->setText(QString::number(displayed_packet.getTcp_urgent_pointer()));
    // TODO: Fill out options

}
