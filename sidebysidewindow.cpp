#include "sidebysidewindow.h"
#include "ui_sidebysidewindow.h"

#include <QDebug>
#include <QMessageBox>
#include "packetinfodialog.h"

#define COLUMN_TIMESTAMP 0
#define COLUMN_CLIENT 1
#define COLUMN_SERVER 2
#define COLUMN_DETAILS 3

#define TCP_SYN 0x02
#define TCP_ACK 0x10

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
    populate_windowing();
    set_details(ui->comboBox_choice->currentText()); // Set the window details to what is currently in the combo box

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

    bool processing_twh; // Set to false until the three-way-handshake has been processed

    processing_twh = false;
    seq_number = -1;
    ack_number = -1;

    for(int i = 0; i < input_vect.length(); i++) {
        tcp_flags = input_vect[i].getTcp_flags();

        if((tcp_flags & TCP_SYN) && !(tcp_flags & TCP_ACK)) { // SYN and not ACK
            all_row_entries[i].syn_ack_details = "Three-way handshake (1): SYN";
            all_row_entries[i].syn_ack_more_details = QString("The client wants to initialise a connection with the server. This connection is established in TCP via the three-way handshake.\n\nAs this is the first step of the three-way handshake, the client sets the sequence number (SEQ) to a random number, in this case: SEQ = %1.\n\nThe acknowledgement number is initally set to 0. ACK = 0").arg(input_vect[i].getTcp_sequence_number());

            processing_twh = true; // From now on, will not display "Insufficient data..." message

        } else if((tcp_flags & TCP_SYN) && (tcp_flags & TCP_ACK)) { // SYN and ACK
            all_row_entries[i].syn_ack_details = "Three-way handshake (2): SYN-ACK";
            all_row_entries[i].syn_ack_more_details = QString("The server acknowledges receiving the SYN packet and sets its ACK number to 1 plus the SEQ number from the previous packet.\n"
                                                              "ACK = 1 + %1 = %2 (in other words, this is the SEQ number the server expects the client to send next)\n\n"
                                                              "The server sets its own random SEQ number, in this case: SEQ = %3").arg(seq_number).arg(input_vect[i].getTcp_acknowledgement_number()).arg(input_vect[i].getTcp_sequence_number());
            ack_number = input_vect[i].getTcp_acknowledgement_number(); // Set so that we can compare when checking if the next step in the three-way handshake

        } else if(!(tcp_flags & TCP_SYN) && (tcp_flags & TCP_ACK) && (input_vect[i].getTcp_sequence_number() == ack_number)) { // not SYN and ACK and current SEQ number matches previous ACK number
            all_row_entries[i].syn_ack_details = "Three-way handshake (3): ACK";
            all_row_entries[i].syn_ack_more_details = QString("The client acknowledges receiving the SYN-ACK packet and sets its ACK number to 1 + the SEQ number it just received.\n"
                                                              "ACK = 1 + %1 = %2.\n\n"
                                                              "The SEQ number the client sends is now %3 (note that this is in alignment with what the server says it is expecting in the SYN-ACK packet)").arg(seq_number).arg(input_vect[i].getTcp_acknowledgement_number()).arg(input_vect[i].getTcp_sequence_number());
            ack_number = -1; // Reset

        } else if(processing_twh ==  false) {
            all_row_entries[i].syn_ack_more_details = "Insufficient data to process details. Try another packet below.";

        } else if((tcp_flags & TCP_ACK) && (input_vect[i].getPayload_length() == 0)) { // ACK and zero payload
            all_row_entries[i].syn_ack_details = "ACK of data";
            all_row_entries[i].syn_ack_more_details = QString("The %1 just received %2 bytes, so the ACK number is the previous SEQ number + the payload length.\n"
                                                              "ACK = %3 + %4 = %5.\n"
                                                              "This is what the %1 expects the next SEQ number to be.\n\n"
                                                              "The SEQ number equals the ACK of the previous packet. SEQ = %6.").arg((all_row_entries[i].local) ? "client" : "server").arg(payload).arg(seq_number).arg(payload).arg(input_vect[i].getTcp_acknowledgement_number()).arg(input_vect[i].getTcp_sequence_number());

        } else if((input_vect[i].getPayload_length() > 0) && (payload == 0)) { // Sending data and just received nothing
            all_row_entries[i].syn_ack_details = "Sending data";
            all_row_entries[i].syn_ack_more_details = QString("The %1 sends %2 bytes of data.\n\n"
                                                              "Because no data was received in the last packet, the SEQ number remains at %3 and the ACK number remains at %4.").arg((all_row_entries[i].local) ? "client" : "server").arg(input_vect[i].getPayload_length()).arg(input_vect[i].getTcp_sequence_number()).arg(input_vect[i].getTcp_acknowledgement_number());

        } else {
            all_row_entries[i].syn_ack_details = "";

        }

        // Set the SEQ number and payload size for use in the next iteration of the loop
        seq_number = input_vect[i].getTcp_sequence_number();
        payload = input_vect[i].getPayload_length();
    }
}

void SideBySideWindow::populate_windowing() {
    int client_scaling_factor, server_scaling_factor;

    QLineSeries *series_client = new QLineSeries();
    QLineSeries *series_server = new QLineSeries();

    // Add origin to graph
    series_client->append(0, 0);
    series_server->append(0, 0);

    for(int i = 0; i < input_vect.length(); i++) {
        if(all_row_entries[i].local) { // Client
            if((input_vect[i].getTcp_window_scale() != -1) && (input_vect[i].getTcp_window_scale() != client_scaling_factor)) {
                all_row_entries[i].window_size = input_vect[i].getTcp_window();
                client_scaling_factor = input_vect[i].getTcp_window_scale();
                all_row_entries[i].windowing_details = QString("Client window scaling factor set to %1").arg(client_scaling_factor);
                all_row_entries[i].windowing_more_details = "New client scaling factor set for this session. From now on, we raise 2 to this power to get our window size multiplier.";

            } else if(input_vect[i].getTcp_window_scale() == -1) { // Scaling factor has not been changed
                all_row_entries[i].window_size = input_vect[i].getTcp_window() << client_scaling_factor;
                all_row_entries[i].windowing_more_details = QString("Client scaling factor remains at %2. We raise 2 to this power to get our window size multiplier.\n\n"
                                                                    "Window size = %1\n"
                                                                    "Window multiplier = 2^%2 = %3\n"
                                                                    "Window total size = %1 * %3 = %4 bytes").arg(input_vect[i].getTcp_window()).arg(client_scaling_factor).arg(pow(2, client_scaling_factor)).arg(all_row_entries[i].window_size);
            }

            if(all_row_entries[i].window_size > 0) {
                series_client->append(i, all_row_entries[i].window_size);
                qDebug() << "Plotting client:" << i << all_row_entries[i].window_size;
            }

        } else { // Server
            if((input_vect[i].getTcp_window_scale() != -1) && (input_vect[i].getTcp_window_scale() != server_scaling_factor)) {
                all_row_entries[i].window_size = input_vect[i].getTcp_window();
                server_scaling_factor = input_vect[i].getTcp_window_scale();
                all_row_entries[i].windowing_details = QString("Server window scaling factor set to %1").arg(server_scaling_factor);
                all_row_entries[i].windowing_more_details = "New server scaling factor set for this session. From now on, we raise 2 to this power to get our window size multiplier.";

            } else if(input_vect[i].getTcp_window_scale() == -1) { // Scaling factor has not been changed
                all_row_entries[i].window_size = input_vect[i].getTcp_window() << server_scaling_factor;
                all_row_entries[i].windowing_more_details = QString("Server scaling factor remains at %2. We raise 2 to this power to get our window size multiplier.\n\n"
                                                                    "Window size = %1\n"
                                                                    "Window multiplier = 2^%2 = %3\n"
                                                                    "Window total size = %1 * %3 = %4 bytes").arg(input_vect[i].getTcp_window()).arg(server_scaling_factor).arg(pow(2, server_scaling_factor)).arg(all_row_entries[i].window_size);
            }

            if(all_row_entries[i].window_size > 0) {
                series_server->append(i, all_row_entries[i].window_size);
                qDebug() << "Plotting server:" << i << all_row_entries[i].window_size;
            }
        }
    }

    // Draw charts
    QChart *window_chart = new QChart();
    window_chart->legend()->hide();
    window_chart->addSeries(series_client);
    window_chart->addSeries(series_server);
    window_chart->createDefaultAxes();
    ui->widget_window_size->setChart(window_chart);
}

void SideBySideWindow::set_details(QString choice) {
    QString details_field;

    if(choice == "SYN-ACK") {
        ui->widget_window_size->hide();
        ui->pushButton_more_info->setText("More info on three-way-handshake and SEQ, ACK numbers");
        more_info_popup_text = "The three-way handshake is used to when setting up a TCP connection between two entities on a network (host A and B). It works as follows:\n"
                               "1. Host A sends a SYN (synchronise) packet\n"
                               "2. Host B responds with a SYN-ACK packet (acknowledge the previous and synchronise)\n"
                               "3. Host A responds with an ACK packet (acknowledge the previous)\n\n"
                               "Now that a connection is established, data transfer can begin. Sequence numbers and acknowledgement numbers are used to ensure packets arrive correctly. If either side determines that a packet has been lost, the missing data will be reset.";

    } else if(choice == "Windowing") {
        ui->widget_window_size->show();
        ui->pushButton_more_info->setText("More info on windowing and flow control");
        more_info_popup_text = "Windowing is a type of flow control used in TCP. It determines how much data can be sent before requiring an acknowledgement.\n"
                               "If window sizes are small, it can really slow down network traffic if window sizes are small. However, if window sizes are too large, more data will need to be resent if a packet is dropped.";

    } else {
        ui->pushButton_more_info->setText("NULL");
        more_info_popup_text = "NULL";
    }



    for(int i = 0; i < input_vect.length(); i++) {
        // Decide what to set as the details field
        if(choice == "SYN-ACK") {
            details_field = all_row_entries[i].syn_ack_details;
        } else if(choice == "Windowing") {
            details_field = all_row_entries[i].windowing_details;
        } else {
            details_field = "";
        }

        ui->tableWidget_packets->setItem(i, COLUMN_DETAILS, new QTableWidgetItem(details_field));
    }

}

QString SideBySideWindow::get_more_details(int row) {
    QString choice = ui->comboBox_choice->currentText();

    if(choice == "SYN-ACK") {
        return all_row_entries[row].syn_ack_more_details;
    } else if(choice == "Windowing") {
        return all_row_entries[row].windowing_more_details;
    } else {
        return "";
    }
}

void SideBySideWindow::on_tableWidget_packets_cellDoubleClicked(int row) {
    if(input_vect[row].getIp_protocol() ==  6) { // If user has clicked on a TCP packet
        ui->label_hint->hide();

        // Open dialog with packet details with an argument
        PacketInfoDialog infoDialog(&input_vect, row);
        infoDialog.setModal(true);
        infoDialog.exec();
    } else {
        QMessageBox::warning(this, "Error", QString("Please choose a TCP packet. Details not available for %1 packets.").arg(input_vect[row].getIp_protocol_string()));
    }
}

void SideBySideWindow::on_tableWidget_packets_itemSelectionChanged()
{
    int row = ui->tableWidget_packets->currentRow();
    ui->textBrowser_more_details->setText(get_more_details(row));
}

void SideBySideWindow::on_comboBox_choice_currentTextChanged(const QString &current_choice)
{
    ui->label_hint->hide();
    set_details(current_choice); // Set the more_details textbox contents
}

void SideBySideWindow::on_pushButton_more_info_clicked()
{
    QMessageBox::information(this, "More info", more_info_popup_text);
}
