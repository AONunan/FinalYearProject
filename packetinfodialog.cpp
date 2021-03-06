#include "packetinfodialog.h"
#include "ui_packetinfodialog.h"

#include <QDebug>

#define TCP_FIN 0x01 // 0b000001
#define TCP_SYN 0x02 // 0b000010
#define TCP_RST 0x04 // 0b000100
#define TCP_PSH 0x08 // 0b001000
#define TCP_ACK 0x10 // 0b010000
#define TCP_URG 0x20 // 0b100000

#define FIELD_IP_VERSION           "Version"
#define FIELD_IP_HEADER_LENGTH     "Header Length"
#define FIELD_IP_TYPE_OF_SERVICE   "Type of Service"
#define FIELD_IP_TOTAL_LENGTH      "Total Length"
#define FIELD_IP_ID                "Identification"
#define FIELD_IP_FLAGS             "Flags"
#define FIELD_IP_OFFSET            "Fragment Offset"
#define FIELD_IP_TTL               "Time To Live"
#define FIELD_IP_PROTOCOL          "Protocol"
#define FIELD_IP_CHECKSUM          "Header Checksum"
#define FIELD_IP_SRC_ADDRESS       "Source Address"
#define FIELD_IP_DST_ADDRESS       "Destination Address"
#define FIELD_IP_OPTIONS           "Options"

#define FIELD_TCP_SRC_PORT         "Source Port"
#define FIELD_TCP_DST_PORT         "Destination Port"
#define FIELD_TCP_SEQUENCE_NUMBER  "Sequence Number"
#define FIELD_TCP_ACK_NUMBER       "Acknowledgement Number"
#define FIELD_TCP_OFFSET           "Offset"
#define FIELD_TCP_RESERVED         "Reserved"
#define FIELD_TCP_FLAGS            "Flags"
#define FIELD_TCP_WINDOW           "Window"
#define FIELD_TCP_CHECKSUM         "Checksum"
#define FIELD_TCP_URGENT_PTR       "Urgent Pointer"
#define FIELD_TCP_OPTIONS          "Options"

PacketInfoDialog::PacketInfoDialog(QVector<Packet>* input_vect_ptr, const int input_packet_index, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketInfoDialog) {
    ui->setupUi(this);

    vect_ptr = input_vect_ptr;
    current_index = input_packet_index;

    set_up_display();
}

PacketInfoDialog::~PacketInfoDialog() {
    delete ui;
}

/*
 * Set up the header fields/packet payload
 */
void PacketInfoDialog::set_up_display() {
    QVector<Packet> packet_vect = *vect_ptr; // Set up QVector from pointer
    displayed_packet = packet_vect[current_index]; // Get the required packet from the offset

    vect_length = packet_vect.length();

    // Set window title
    QDialog::setWindowTitle(QString("Packet %1 of %2").arg(current_index + 1).arg(vect_length));

    // Decide if any buttons need to be disabled
    if(current_index == 0) {
        ui->pushButton_previous_packet->setEnabled(false);
    } else if(current_index == vect_length - 1) {
        ui->pushButton_next_packet->setEnabled(false);
    } else {
        ui->pushButton_previous_packet->setEnabled(true);
        ui->pushButton_next_packet->setEnabled(true);
    }

    show_header_details();
    ui->groupBox_ip_header->setTitle(QString("IP Header (%1 bytes, %2 bits, %3 32 bit words)").arg(displayed_packet.getIp_header_length()).arg(displayed_packet.getIp_header_length() * 8).arg(displayed_packet.getIp_header_length() / 4));
    ui->groupBox_tcp_header->setTitle(QString("TCP Header (%1 bytes, %2 bits, %3 32 bit words)").arg(displayed_packet.getTcp_header_length()).arg(displayed_packet.getTcp_header_length() * 8).arg(displayed_packet.getTcp_header_length() / 4));

    // If payload exists, print out payload
    if(displayed_packet.getPayload_length() > 0) {
        ui->groupBox_payload->show();
        this->resize(700, 600);

        // Increase the width of the columns
        ui->tableWidget_payload->setColumnWidth(0, 70);
        ui->tableWidget_payload->setColumnWidth(1, 410);

        print_payload();

    } else {
        // Hide the payload box and resize the window to a smaller size
        ui->groupBox_payload->hide();
        this->resize(700, 200);
        this->resize(700, 200); // Not sure why but window doesn't always resize properly unless I include this line twice
    }
}

/*
 * Switch between displaying header field names and details
 */
void PacketInfoDialog::on_pushButton_change_view_clicked() {
    if(currently_showing_field_names) {
        show_header_details();
    } else {
        show_header_field_names();
    }
}

/*
 * Show field names
 */
void PacketInfoDialog::show_header_field_names() {
    ui->pushButton_change_view->setText("Show field values");
    currently_showing_field_names = true;

    // Set IP fields
    ui->label_ip_version->setText(FIELD_IP_VERSION);
    ui->label_ip_header_length->setText(FIELD_IP_HEADER_LENGTH);
    ui->label_ip_type_of_service->setText(FIELD_IP_TYPE_OF_SERVICE);
    ui->label_ip_total_length->setText(FIELD_IP_TOTAL_LENGTH);
    ui->label_ip_id->setText(FIELD_IP_ID);
    ui->label_ip_flags->setText(FIELD_IP_FLAGS);
    ui->label_ip_offset->setText(FIELD_IP_OFFSET);
    ui->label_ip_ttl->setText(FIELD_IP_TTL);
    ui->label_ip_protocol->setText(FIELD_IP_PROTOCOL);
    ui->label_ip_checksum->setText(FIELD_IP_CHECKSUM);
    ui->label_ip_src_address->setText(FIELD_IP_SRC_ADDRESS);
    ui->label_ip_dst_address->setText(FIELD_IP_DST_ADDRESS);
    ui->label_ip_options->setText(FIELD_IP_OPTIONS);

    // Set TCP fields
    ui->label_tcp_src_port->setText(FIELD_TCP_SRC_PORT);
    ui->label_tcp_dst_port->setText(FIELD_TCP_DST_PORT);
    ui->label_tcp_sequence_number->setText(FIELD_TCP_SEQUENCE_NUMBER);
    ui->label_tcp_ack_number->setText(FIELD_TCP_ACK_NUMBER);
    ui->label_tcp_offset->setText(FIELD_TCP_OFFSET);
    ui->label_tcp_reserved->setText(FIELD_TCP_RESERVED);
    ui->label_tcp_flags->setText(FIELD_TCP_FLAGS);
    ui->label_tcp_window->setText(FIELD_TCP_WINDOW);
    ui->label_tcp_checksum->setText(FIELD_TCP_CHECKSUM);
    ui->label_tcp_urgent_ptr->setText(FIELD_TCP_URGENT_PTR);
    ui->label_tcp_options->setText(FIELD_TCP_OPTIONS);
}

/*
 * Show field values and set tooltips
 */
void PacketInfoDialog::show_header_details() {
    ui->pushButton_change_view->setText("Show field names");
    currently_showing_field_names = false;

    // Set IP field details
    ui->label_ip_version->setText(QString::number(displayed_packet.getIp_version()));
    ui->label_ip_header_length->setText(QString::number(displayed_packet.getIp_header_length())); // TODO: Fill out length
    ui->label_ip_type_of_service->setText(QString::number(displayed_packet.getIp_type_of_service()));
    ui->label_ip_total_length->setText(QString::number(displayed_packet.getIp_length()));
    ui->label_ip_id->setText(QString::number(displayed_packet.getIp_id()));
    ui->label_ip_offset->setText(QString::number(displayed_packet.getIp_offset()));
    ui->label_ip_ttl->setText(QString::number(displayed_packet.getIp_time_to_live()));
    ui->label_ip_protocol->setText(QString::number(displayed_packet.getIp_protocol()));
    ui->label_ip_checksum->setText(QString("0x%1").arg(QString("%1").arg(displayed_packet.getIp_checksum(), 2, 16, QChar('0')).toUpper())); // Display as hex
    ui->label_ip_src_address->setText(displayed_packet.getIp_source_address());
    ui->label_ip_dst_address->setText(displayed_packet.getIp_destination_address());

    // Set IP field tooltips
    ui->label_ip_version->setToolTip(QString("%1 (4 bits)"
                                             "%2").arg(FIELD_IP_VERSION).arg((displayed_packet.getIp_version() == 4) ? "\n\nIP Version 4" : "")); // Set tooltip to IPv4 if ip_version == 4
    ui->label_ip_total_length->setToolTip(QString("%1 (16 bits)\n"
                                                  "The total length of the header and the data in bytes.\n\n"
                                                  "IP header = %2 bytes\n"
                                                  "TCP header = %3 bytes\n"
                                                  "Payload = %4 bytes\n\n"
                                                  "Total length = %2 + %3 + %4 = %5 bytes").arg(FIELD_IP_TOTAL_LENGTH).arg(displayed_packet.getIp_header_length()).arg(displayed_packet.getTcp_header_length()).arg(displayed_packet.getPayload_length()).arg(displayed_packet.getIp_length()));
    ui->label_ip_protocol->setToolTip(QString("%1 (8 bits)\n\n"
                                              "%2").arg(FIELD_IP_PROTOCOL).arg(displayed_packet.getIp_protocol_string()));

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

    // Set TCP field tooltips
    ui->label_tcp_src_port->setToolTip(QString("%1 (16 bits)\n\n"
                                               "%2").arg(FIELD_TCP_SRC_PORT).arg(Packet::tcp_port_to_string(displayed_packet.getTcp_source_port())));
    ui->label_tcp_dst_port->setToolTip(QString("%1 (16 bits)\n\n"
                                               "%2").arg(FIELD_TCP_DST_PORT).arg(Packet::tcp_port_to_string(displayed_packet.getTcp_destination_port())));
    ui->label_tcp_offset->setToolTip(QString("%1 (4 bits)\n\n"
                                             "The number of 32 bit words in the TCP header, indicating where the data payload begins.\n"
                                             "This also indicates the TCP header length.\n\n"
                                             "%2 * 32 bits = %3 bits = %4 bytes = Offset of data = TCP header length\n\n"
                                             "This also tells us that the options field must be %5 bytes long (total length - 20 bytes for default TCP header)").arg(FIELD_TCP_OFFSET).arg(displayed_packet.getTcp_offset()).arg(displayed_packet.getTcp_offset() * 32).arg(displayed_packet.getTcp_offset() * 32 / 8).arg(displayed_packet.getTcp_header_length() - 20));
    ui->label_tcp_flags->setToolTip(find_tcp_flag_string(displayed_packet.getTcp_flags()));
    ui->label_tcp_options->setToolTip(QString("%1 (%2 bytes, variable length)\n\n"
                                              "Options can be added at the end of the TCP header, to provide extra functionality.\n"
                                              "E.g. maximum segment sizes, window scaling factor, selective acknowledgements, timestamps, etc."
                                              "%3").arg(FIELD_TCP_OPTIONS).arg(displayed_packet.getTcp_header_length() - 20).arg((displayed_packet.getTcp_window_scale() != -1) ? QString("\n\nOptions for this packet (partial):\nWindow scaling factor = %1").arg(displayed_packet.getTcp_window_scale()) : ""));
}

/*
 * Get the tooltip string for TCP flags
 */
QString PacketInfoDialog::find_tcp_flag_string(int flags) {
    QString result = QString("%1 (6 bits)\n").arg(FIELD_TCP_FLAGS);

    result += "\nFIN = ";
    result += (flags & TCP_FIN) ? "1" : "0"; // If FIN bit is set, set to "True", otherwise "False"
    result += "\nFinish: Used to tear down connections. Each sides sends a FIN followed by an ACK and the connection closes.";

    result += "\n\nSYN = ";
    result += (flags & TCP_SYN) ? "1" : "0";
    result += "\nSynchronize: Used at the start of the three-way handshake to initialise a connection.";

    result += "\n\nRST = ";
    result += (flags & TCP_RST) ? "1" : "0";
    result += "\nReset: Used to inform the reciever that the sender has abrubtly shut the connection down.";

    result += "\n\nPUSH = ";
    result += (flags & TCP_PSH) ? "1" : "0";
    result += "\nPush: Often set at the end of a block of data, informing the receiver that they can begin processing the data.";

    result += "\n\nACK = ";
    result += (flags & TCP_ACK) ? "1" : "0";
    result += "\nAcknowledgment: Used to acknowledge that data has been received. Used also when setting up and tearing down TCP connections.";

    result += "\n\nURG = ";
    result += (flags & TCP_URG) ? "1" : "0";
    result += "\nUrgent: Indicates that the data is urgent. The receiver will know to process the data immediately.";

    return result;
}

/*
 * Display the offset, hex and ASCII
 */
void PacketInfoDialog::print_payload() {
    ui->groupBox_payload->setTitle(QString("Payload (%1 bytes)").arg(displayed_packet.getPayload_length()));
    print_payload_offset();
    print_payload_hex_ascii();
}

/*
 * Display the offset
 */
void PacketInfoDialog::print_payload_offset() {
    int i;
    int row_count = 0;

    for(i = 0; i < displayed_packet.getPayload_length(); i += 16) {
        ui->tableWidget_payload->setRowCount(row_count + 1);
        ui->tableWidget_payload->setItem(row_count, 0, new QTableWidgetItem(QString("%1").arg(i, 5, 10, QChar('0'))));
        row_count++;
    }
}

/*
 * Display the hex data and the ASCII format
 */
void PacketInfoDialog::print_payload_hex_ascii() {
    // No need to increase table row count in this function, that was done in print_payload_offset()

    int i, j;
    int total_printed = 0;
    int current_row = 0;
    QString hex_line, ascii_line;

    // Display all rows except the last one (because the last row will be a variable length, not always 16 long
    for(i = 0; i < displayed_packet.getPayload_length() - 16; i += 16) {
        hex_line = ""; // Reset
        ascii_line = "";

        for(j = 0; j < 16; j++) {
            // Get all values in row
            hex_line += QString("%1 ").arg(displayed_packet.getPayload_vect()[i + j], 2, 16, QChar('0'));

            // Exlcude extended ascii characters
            if((32 <= displayed_packet.getPayload_vect()[i + j]) && (displayed_packet.getPayload_vect()[i + j] <= 126)) {
                ascii_line += char(displayed_packet.getPayload_vect()[i + j]);
            } else {
                ascii_line += ".";
            }
            total_printed++;
        }

        // Add to table
        ui->tableWidget_payload->setItem(current_row, 1, new QTableWidgetItem(hex_line));
        ui->tableWidget_payload->setItem(current_row, 2, new QTableWidgetItem(ascii_line));
        current_row++;
    }

    hex_line = "";

    // Print the final row
    for(i = total_printed; i < displayed_packet.getPayload_length(); i++) {
        hex_line += QString("%1 ").arg(displayed_packet.getPayload_vect()[i], 2, 16, QChar('0'));
        ascii_line += char(displayed_packet.getPayload_vect()[i]);
    }

    ui->tableWidget_payload->setItem(current_row, 1, new QTableWidgetItem(hex_line));
    ui->tableWidget_payload->setItem(current_row, 2, new QTableWidgetItem(ascii_line));
}

/*
 * Go to next packet
 */
void PacketInfoDialog::on_pushButton_next_packet_clicked() {
    current_index++;
    set_up_display();
}

/*
 * Go to previous packet
 */
void PacketInfoDialog::on_pushButton_previous_packet_clicked() {
    current_index--;
    set_up_display();
}
