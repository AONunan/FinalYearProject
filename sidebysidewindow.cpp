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
    int tcp_flags,
        syn_ack_ack_number;

    for(int i = 0; i < input_vect.length(); i++) {
        tcp_flags = input_vect[i].getTcp_flags();

        if((tcp_flags & TCP_SYN) && !(tcp_flags & TCP_ACK)) { // SYN and not ACK
            all_row_entries[i].syn_ack_details = "Three-way handshake (1): SYN";

        } else if((tcp_flags & TCP_SYN) && (tcp_flags & TCP_ACK)) { // SYN and ACK
            syn_ack_ack_number = input_vect[i].getTcp_acknowledgement_number();
            all_row_entries[i].syn_ack_details = "Three-way handshake (2): SYN-ACK";

        } else if(!(tcp_flags & TCP_SYN) && (tcp_flags & TCP_ACK) && (input_vect[i].getTcp_sequence_number() == syn_ack_ack_number)) { // not SYN and ACK and the sequence number matches the ACK of SYN-ACK packet
            syn_ack_ack_number = 0; // Reset
            all_row_entries[i].syn_ack_details = "Three-way handshake (3): ACK";

        } else {
            all_row_entries[i].syn_ack_details = "";
        }
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
    ui->textBrowser_more_details->setText(QString::number(input_vect[row].getPayload_length()));
}

void SideBySideWindow::on_comboBox_choice_currentTextChanged(const QString &current_choice)
{
    qDebug() << "Chosen:" << current_choice;
}
