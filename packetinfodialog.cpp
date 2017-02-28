#include "packetinfodialog.h"
#include "ui_packetinfodialog.h"

#include <QDebug>

#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_PUSH 0x08
#define TCP_ACK 0x10
#define TCP_URG 0x20
#define TCP_ECE 0x40
#define TCP_CWR 0x80

PacketInfoDialog::PacketInfoDialog(const Packet packet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketInfoDialog) {
    ui->setupUi(this);

    displayed_packet = packet;

    show_header_details();
    ui->groupBox_ip_header->setTitle(QString("IP Header (%1 bytes)").arg(displayed_packet.getIp_header_length()));
    ui->groupBox_tcp_header->setTitle(QString("TCP Header (%1 bytes)").arg(displayed_packet.getTcp_header_length()));

    // If payload exists, print out payload
    if(displayed_packet.getPayload_length() > 0) {
        print_payload();
    } else {
        ui->groupBox_payload->hide();
        this->resize(700, 300);
    }

    //ui->textBrowser_payload_hex->setText("Hello world, this is a test");

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
    ui->pushButton_change_view->setText("Show field values");
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
    ui->pushButton_change_view->setText("Show field names");
    currently_showing_field_names = false;

    // Set IP field details
    ui->label_ip_version->setText(QString::number(displayed_packet.getIp_version()));
    ui->label_ip_header_length->setText(QString::number(displayed_packet.getIp_header_length())); // TODO: Fill out length
    ui->label_ip_type_of_service->setText(QString::number(displayed_packet.getIp_type_of_service()));
    ui->label_ip_total_length->setText(QString::number(displayed_packet.getIp_length()));
    ui->label_ip_id->setText(QString::number(displayed_packet.getIp_id()));
    // TODO: Fill out flags

    ui->label_ip_offset->setText(QString::number(displayed_packet.getIp_offset()));
    ui->label_ip_ttl->setText(QString::number(displayed_packet.getIp_time_to_live()));
    ui->label_ip_protocol->setText(QString::number(displayed_packet.getIp_protocol()));
    ui->label_ip_checksum->setText(QString("0x%1").arg(QString("%1").arg(displayed_packet.getIp_checksum(), 2, 16, QChar('0')).toUpper())); // Display as hex
    ui->label_ip_src_address->setText(displayed_packet.getIp_source_address());
    ui->label_ip_dst_address->setText(displayed_packet.getIp_destination_address());
    // TODO: Fill out options

    // Set IP field tooltips
    ui->label_ip_version->setToolTip(QString("(4 bits)%1").arg((displayed_packet.getIp_version() == 4) ? "\n\nIP Version 4" : "")); // Set tooltip to IPv4 if ip_version == 4
    ui->label_ip_header_length->setToolTip(QString("(4 bits)\n\nThe length of the IP header, given in bytes."));
    ui->label_ip_protocol->setToolTip(QString("(8 bits)\n\n%1").arg(displayed_packet.getIp_protocol_string()));

    // Set TCP field details
    ui->label_tcp_src_port->setText(QString::number(displayed_packet.getTcp_source_port()));
    ui->label_tcp_dst_port->setText(QString::number(displayed_packet.getTcp_destination_port()));
    ui->label_tcp_sequence_number->setText(QString::number(displayed_packet.getTcp_sequence_number()));
    ui->label_tcp_ack_number->setText(QString::number(displayed_packet.getTcp_acknowledgement_number()));
    ui->label_tcp_offset->setText(QString::number(displayed_packet.getTcp_offset()));
    ui->label_tcp_flags->setText(QString("0b%1").arg(displayed_packet.getTcp_flags(), 6, 2, QChar('0'))); // Display flags in binary
    ui->label_tcp_window->setText(QString::number(displayed_packet.getTcp_window()));
    ui->label_tcp_checksum->setText(QString("0x%1").arg(QString("%1").arg(displayed_packet.getTcp_checksum(), 2, 16, QChar('0')).toUpper())); // Display as hex
    ui->label_tcp_urgent_ptr->setText(QString::number(displayed_packet.getTcp_urgent_pointer()));
    // TODO: Fill out options

    // Set TCP field tooltips
    ui->label_tcp_src_port->setToolTip(QString("(16 bits)\n\n%1").arg(Packet::tcp_port_to_string(displayed_packet.getTcp_source_port())));
    ui->label_tcp_dst_port->setToolTip(QString("(16 bits)\n\n%1").arg(Packet::tcp_port_to_string(displayed_packet.getTcp_destination_port())));
    ui->label_tcp_flags->setToolTip(find_tcp_flag_string(displayed_packet.getTcp_flags()));
}

QString PacketInfoDialog::find_tcp_flag_string(int flags) {
    QString result = "(6 bits)\n";

    result += "\nFIN = ";
    result += (flags & TCP_FIN) ? "True" : "False"; // If FIN bit is set, set to "True", otherwise "False"
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

void PacketInfoDialog::print_payload() {
    ui->groupBox_payload->setTitle(QString("Payload (%1 bytes)").arg(displayed_packet.getPayload_length()));
    print_payload_offset();
}

void PacketInfoDialog::print_payload_offset() {
    int i;
    int row_count = 0;

    /*    ui->tableWidget_packets->setRowCount(row_count + 1);
    ui->tableWidget_packets->scrollToBottom();

    ui->tableWidget_packets->setItem(row_count, HEADER_TIMESTAMP, new QTableWidgetItem(Packet::timestamp_to_string(packet.getCurrent_time())));
    */

    for(i = 0; i <= displayed_packet.getPayload_length(); i += 16) {
        ui->tableWidget_payload->setRowCount(row_count + 1);
        ui->tableWidget_payload->setItem(row_count, 0, new QTableWidgetItem(QString("%1").arg(i, 5, 10, QChar('0'))));
        row_count++;
    }
}
