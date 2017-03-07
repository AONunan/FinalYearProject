#include "sidebysidewindow.h"
#include "ui_sidebysidewindow.h"

#include <QDebug>
#include "packetinfodialog.h"

#define COLUMN_TIMESTAMP 0
#define COLUMN_CLIENT 1
#define COLUMN_SERVER 2
#define COLUMN_DETAILS 3

#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_PUSH 0x08
#define TCP_ACK 0x10
#define TCP_URG 0x20
#define TCP_ECE 0x40
#define TCP_CWR 0x80

SideBySideWindow::SideBySideWindow(QVector<Packet> vect, const QString input_server_address, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SideBySideWindow)
{
    ui->setupUi(this);

    // Set class member variables to input variables
    input_vect = vect;
    server_address = input_server_address;

    row_count = 0;

    populate_entries();
    update_table();
    populate_syn_ack();
    set_details("SYN-ACK");

}

SideBySideWindow::~SideBySideWindow()
{
    delete ui;
}

void SideBySideWindow::populate_entries() {
    int i;

    for(i = 0; i < input_vect.length(); i++) {
        struct row_entry new_row;
        new_row.index = i;
        new_row.timestamp = Packet::timestamp_to_string(input_vect[i].getCurrent_time());

        if(input_vect[i].getIp_destination_address() == server_address) { // Client is sending to server
            new_row.local = true;
        } else if(input_vect[i].getIp_source_address() == server_address) { // Server is sending to client
            new_row.local = false;
        }

        all_row_entries.append(new_row);
    }
}

void SideBySideWindow::update_table() {
    for(int i = 0; i < input_vect.length(); i++) {
        // Create new row and scroll to bottom of table
        ui->tableWidget_packets->setRowCount(row_count + 1);
        ui->tableWidget_packets->scrollToBottom();

        ui->tableWidget_packets->setItem(row_count, COLUMN_TIMESTAMP, new QTableWidgetItem(all_row_entries[i].timestamp));
        ui->tableWidget_packets->setItem(row_count, (all_row_entries[i].local ? COLUMN_CLIENT : COLUMN_SERVER), new QTableWidgetItem(QString("%1 bytes").arg(input_vect[i].getPayload_length()))); // Display data in either the client or server column depending on the value of the boolean

        row_count++;
    }

}

void SideBySideWindow::populate_syn_ack() {
    u_int tcp_flags,
        seq_number,
        ack_number,
        payload;

    bool processing_twh, // Set to false until the three-way-handshake has been processed
         end_of_twh, // Set to True at the very end of the three-way-handshake, false every other time
         currently_in_twh; // Set to True while processing three-way-handshake

    processing_twh = false;
    end_of_twh = false;
    currently_in_twh = false;

    for(int i = 0; i < input_vect.length(); i++) {
        tcp_flags = input_vect[i].getTcp_flags();

        if((tcp_flags & TCP_SYN) && !(tcp_flags & TCP_ACK)) { // SYN and not ACK
            all_row_entries[i].syn_ack_details = "Three-way handshake (1): SYN";
            all_row_entries[i].syn_ack_more_details = QString("The client wants to initialise a connection with the server. This connection is established in TCP via the three-way handshake.\n\nAs this is the first step of the three-way handshake, the client sets the sequence number (SEQ) to a random number, in this case: SEQ = %1.\n\nThe acknowledgement number is initally set to 0. ACK = 0").arg(input_vect[i].getTcp_sequence_number());

            processing_twh = true; // From now on, will not display "Insufficient data..." message
            currently_in_twh = true;

        } else if((tcp_flags & TCP_SYN) && (tcp_flags & TCP_ACK)) { // SYN and ACK
            all_row_entries[i].syn_ack_details = "Three-way handshake (2): SYN-ACK";
            all_row_entries[i].syn_ack_more_details = QString("The server acknowledges receiving the SYN packet and sets its ACK number to 1 plus the SEQ number from the previous packet.\nACK = 1 + %1 = %2 (in other words, this is the SEQ number the server expects the client to send next)\n\nThe server sets its own random SEQ number, in this case: SEQ = %3").arg(seq_number).arg(input_vect[i].getTcp_acknowledgement_number()).arg(input_vect[i].getTcp_sequence_number());

        } else if(!(tcp_flags & TCP_SYN) && (tcp_flags & TCP_ACK) && (currently_in_twh)) { // not SYN and ACK and still processing the three-way-handshake
            all_row_entries[i].syn_ack_details = "Three-way handshake (3): ACK";
            all_row_entries[i].syn_ack_more_details = QString("The client acknowledges receiving the SYN-ACK packet and sets its ACK number to 1 + the SEQ number it just received.\nACK = 1 + %1 = %2.\n\nThe SEQ number the client sends is now %3 (note that this is in alignment with what the server says it is expecting in the SYN-ACK packet)").arg(seq_number).arg(input_vect[i].getTcp_acknowledgement_number()).arg(input_vect[i].getTcp_sequence_number());
            currently_in_twh = false; // Set to false as we are finished processing the three-way-handshake
            end_of_twh = true; // 3-way-handshake finished

        } else if(processing_twh ==  false) {
            all_row_entries[i].syn_ack_more_details = "Insufficient data to process details. Try another packet below.";

        /*} else if(end_of_twh) { // This is the first packet sent after the three-way-handshake
            all_row_entries[i].syn_ack_details = "GET request";
            all_row_entries[i].syn_ack_more_details = QString("The three-way-handshake has completed and the client sends a GET request to the server. As nothing was received, the SEQ number remains at %1 and the ACK number remains at %2.").arg(input_vect[i].getTcp_sequence_number()).arg(input_vect[i].getTcp_acknowledgement_number());
            end_of_twh = false;*/

        } else if((tcp_flags & TCP_ACK) && (input_vect[i].getPayload_length() == 0)) { // ACK and zero payload
            all_row_entries[i].syn_ack_details = "ACK of data";
            all_row_entries[i].syn_ack_more_details = QString("The %1 just received %2 bytes, so the ACK number is the previous SEQ number + the payload length.\nACK = %3 + %4 = %5.\nThis is what the %1 expects the next SEQ number to be.\n\nThe SEQ number equals the ACK of the previous packet. SEQ = %6.").arg((all_row_entries[i].local) ? "client" : "server").arg(payload).arg(seq_number).arg(payload).arg(input_vect[i].getTcp_acknowledgement_number()).arg(input_vect[i].getTcp_sequence_number());

        } else if((input_vect[i].getPayload_length() > 0) && (payload == 0)) { // Sending data and just received nothing
            all_row_entries[i].syn_ack_details = "Sending data";
            all_row_entries[i].syn_ack_more_details = QString("The %1 sends %2 bytes of data.\n\nBecause no data was received in the last packet, the SEQ number remains at %3 and the ACK number remains at %4.").arg((all_row_entries[i].local) ? "client" : "server").arg(input_vect[i].getPayload_length()).arg(input_vect[i].getTcp_sequence_number()).arg(input_vect[i].getTcp_acknowledgement_number());

        } else {
            all_row_entries[i].syn_ack_details = "";

        }

        // Set the SEQ and ACK numbers for use in the next iteration of the loop
        seq_number = input_vect[i].getTcp_sequence_number();
        ack_number = input_vect[i].getTcp_acknowledgement_number();
        payload = input_vect[i].getPayload_length();
    }
}

void SideBySideWindow::set_details(QString choice) {
    QString details_field;

    for(int i = 0; i < input_vect.length(); i++) {
        // Decide what to set as the details field
        if(choice == "SYN-ACK") {
            details_field = all_row_entries[i].syn_ack_details;
        } else {
            details_field = "";
        }

        ui->tableWidget_packets->setItem(i, COLUMN_DETAILS, new QTableWidgetItem(details_field));
    }

}

void SideBySideWindow::on_tableWidget_packets_cellDoubleClicked(int row) {
    // Open dialog with packet details with an argument
    PacketInfoDialog infoDialog(input_vect[row]);
    infoDialog.setModal(true);
    infoDialog.exec();
}

void SideBySideWindow::on_tableWidget_packets_itemSelectionChanged()
{
    int row = ui->tableWidget_packets->currentRow();
    //ui->textBrowser_more_details->setText(QString::number(input_vect[row].getPayload_length()));
    ui->textBrowser_more_details->setText(all_row_entries[row].syn_ack_more_details);
}

void SideBySideWindow::on_comboBox_choice_currentTextChanged(const QString &current_choice)
{
    ui->label_hint->hide();
    qDebug() << "Chosen:" << current_choice;
}
