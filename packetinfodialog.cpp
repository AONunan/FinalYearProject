#include "packetinfodialog.h"
#include "ui_packetinfodialog.h"

#include <QDebug>

#define TCP_FIN  0x01
#define TCP_SYN  0x02
#define TCP_RST  0x04
#define TCP_PUSH 0x08
#define TCP_ACK  0x10
#define TCP_URG  0x20
#define TCP_ECE  0x40
#define TCP_CWR  0x80
#define TCP_FLAGS		(TCP_FIN|TCP_SYN|TCP_RST|TCP_ACK|TCP_URG|TCP_ECE|TCP_CWR)

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
    ui->label_ip_version->setToolTip(QString("(4 bits)%1").arg((displayed_packet.getIp_version() == 4) ? "\n\nIP Version 4" : ""));

    ui->label_ip_header_length->setText(QString::number(displayed_packet.getIp_header_length())); // TODO: Fill out length
    ui->label_ip_header_length->setToolTip(QString("(4 bits)\n\nThe length of the IP header, given in bytes."));

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

    ui->label_ip_checksum->setText(QString("0x%1").arg(displayed_packet.getIp_checksum(), 2, 16, QChar('0')));
    // Tool tip set in UI file

    ui->label_ip_src_address->setText(displayed_packet.getIp_source_address());

    ui->label_ip_dst_address->setText(displayed_packet.getIp_destination_address());

    // TODO: Fill out options

    // Set TCP fields
    ui->label_tcp_src_port->setText(QString::number(displayed_packet.getTcp_source_port()));

    ui->label_tcp_dst_port->setText(QString::number(displayed_packet.getTcp_destination_port()));

    ui->label_tcp_sequence_number->setText(QString::number(displayed_packet.getTcp_sequence_number()));

    ui->label_tcp_ack_number->setText(QString::number(displayed_packet.getTcp_acknowledgement_number()));

    ui->label_tcp_offset->setText(QString::number(displayed_packet.getTcp_offset()));

    ui->label_tcp_flags->setText(QString("0x%1").arg(displayed_packet.getTcp_flags(), 2, 16, QChar('0'))); // Display flags in Hex
    ui->label_tcp_flags->setToolTip(find_tcp_flag_string(displayed_packet.getTcp_flags())); // Show TCP flags in tooltip

    ui->label_tcp_window->setText(QString::number(displayed_packet.getTcp_window()));

    ui->label_tcp_checksum->setText(QString("0x%1").arg(displayed_packet.getTcp_checksum(), 2, 16, QChar('0'))); // Display as hex
    // Tool tip set in UI file

    ui->label_tcp_urgent_ptr->setText(QString::number(displayed_packet.getTcp_urgent_pointer()));

    // TODO: Fill out options

}

QString PacketInfoDialog::find_tcp_flag_string(int flags) {
    QString result = "(6 bits)\n";

    result += "\nFIN = ";
    result += (flags & TCP_FIN) ? "True" : "False";
    result += "\nFinish: Used to tear down connections. Each sides sends a FIN followed by an ACK and the connection closes.";

    result += "\n\nSYN = ";
    result += (flags & TCP_SYN) ? "True" : "False";
    result += "\nSynchronize: Used at the start of the three-way handshake to initialise a connection.";

    result += "\n\nRST = ";
    result += (flags & TCP_RST) ? "True" : "False";
    result += "\nReset: Used to inform the reciever that the sender has abrubtly shut the connection down.";

    result += "\n\nPUSH = ";
    result += (flags & TCP_PUSH) ? "True" : "False";
    result += "\nPush: Often set at the end of a block of data, informing the receiver that they can begin processing the data.";

    result += "\n\nACK = ";
    result += (flags & TCP_ACK) ? "True" : "False";
    result += "\nAcknowledgment: Used to acknowledge that data has been received. Used also when setting up and tearing down TCP connections.";

    result += "\n\nURG = ";
    result += (flags & TCP_URG) ? "True" : "False";
    result += "\nUrgent: Indicates that the data is urgent. The receiver will know to process the data immediately.";

    return result;
}
