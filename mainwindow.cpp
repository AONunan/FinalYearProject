#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTableWidget>
#include <QDateTime>
#include <QNetworkInterface>
#include <thread>
#include <QMessageBox>

#include "packet.h"
#include "packetinfodialog.h"
#include "statwindow.h"
#include "sidebysidewindow.h"
#include "filtersettingswindow.h"

// These correspond to the columns in the table
#define HEADER_TIMESTAMP 0
#define HEADER_PROTOCOL 1
#define HEADER_SRC_HOST 2
#define HEADER_DST_HOST 3
#define HEADER_PAYLOAD_LENGTH 4

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Set the network interface device
    dev = packetTracer.get_network_interface_device();
    ui->label_interface->setText(dev);

    // Set network ID and subnet mask
    packetTracer.set_mask_and_ip(dev, &net, &mask);

    // Open the handle
    handle = packetTracer.open_for_sniffing(dev);

    // Apply the filter
    packetTracer.apply_filter(handle, &filter_expression, net);

    row_count = 0;
    currently_capturing_packets = false;
    break_out_of_capture = false;

    ui->label_hint->hide(); // Hide the hint initially

    my_ip_address = find_my_ip_address();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_button_close_handle_clicked() {
    // Close handle
    pcap_close(handle);

    MainWindow::close();
}

void MainWindow::on_button_capture_packet_clicked() {
    if(!currently_capturing_packets) {
        currently_capturing_packets = true;

        // TODO: Implement multithreading to allow use of UI while loop is running
        capture_loop();

        ui->label_hint->show();

        currently_capturing_packets = false;
    } else {
        qDebug() << "Setting flag to break out of loop.";
        break_out_of_capture = true;
    }

}

void MainWindow::capture_loop() {
    ui->statusBar->showMessage("Waiting for first packet.");

    Packet my_captured_packet;
    int i;

    // Get number of packets required from spinbox
    int no_of_packets = ui->spinBox_no_of_packets->value();

    //qDebug() << "Value of bool:" << break_out_of_capture;
    ui->button_capture_packet->setText("Cancel");

    for(i = 0; i < no_of_packets; i++) {
        // TODO: Add loading symbol to window while packets are being captured.

        packet = 0;

        // Packets sometimes return as 0, causing errors. Loop until non-0 value returned
        while(packet == 0) {
            // Fetch single packet
            packet = pcap_next(handle, &header);
        }

        // Process captured packet
        my_captured_packet = packetTracer.captured_packet(&header, packet, my_captured_packet);

        // Store the captured packet in vector
        captured_packets_vect.append(my_captured_packet);

        ui->statusBar->showMessage(QString("Captured %1 of %2.").arg(QString::number(i + 1)).arg(QString::number(no_of_packets)));

        // TODO: Implement multithreading to allow breaking out of running loop
        /*if(break_out_of_capture) {
            qDebug() << "Breaking out of loop.";
            break;
        }
        break_out_of_capture = false;*/
    }

    // Add each newly captured packet in the vector to the UI
    for(i = captured_packets_vect.length() - no_of_packets; i < captured_packets_vect.length(); i++) {
        update_table(captured_packets_vect[i]);
        //ui->statusBar->showMessage(QString("Processing %1 of %2.").arg(QString::number(i + 1)).arg(QString::number(captured_packets_vect.length())));
    }

    ui->button_capture_packet->setText("Capture");

    ui->statusBar->showMessage(QString("Finished capturing %1 packets.").arg(QString::number(no_of_packets)));
}

QString MainWindow::find_my_ip_address()
{
    QList<QHostAddress> ip_addr_list = QNetworkInterface::allAddresses();

    for(int i = 0; i < ip_addr_list.count(); i++) {
        if(!ip_addr_list[i].isLoopback()) // Check that it is no a loopback address
            if(ip_addr_list[i].protocol() == QAbstractSocket::IPv4Protocol) // Check that it is an IPv4 address
                return ip_addr_list[i].toString();
    }

    // Return error string if no suitable addresses found
    return "IP_ADDRESS_ERROR";
}

void MainWindow::update_table(Packet packet) {
    // Create new row and scroll to bottom of table
    ui->tableWidget_packets->setRowCount(row_count + 1);
    ui->tableWidget_packets->scrollToBottom();

    ui->tableWidget_packets->setItem(row_count, HEADER_TIMESTAMP, new QTableWidgetItem(Packet::timestamp_to_string(packet.getCurrent_time())));
    ui->tableWidget_packets->setItem(row_count, HEADER_PROTOCOL, new QTableWidgetItem(packet.getProtocol()));
    //ui->tableWidget_packets->setItem(row_count, HEADER_SRC_HOST, new QTableWidgetItem(packet));
    ui->tableWidget_packets->setItem(row_count, HEADER_SRC_HOST, new QTableWidgetItem(packet.getIp_source_address()));
    ui->tableWidget_packets->setItem(row_count, HEADER_DST_HOST, new QTableWidgetItem(packet.getIp_destination_address()));
    ui->tableWidget_packets->setItem(row_count, HEADER_PAYLOAD_LENGTH, new QTableWidgetItem(QString::number(packet.getPayload_length())));

    row_count++;
}

void MainWindow::on_tableWidget_packets_cellDoubleClicked(int row) {
    ui->label_hint->hide();

    // Open dialog with packet details with an argument
    PacketInfoDialog infoDialog(captured_packets_vect[row]);
    infoDialog.setModal(true);
    infoDialog.exec();
}

void MainWindow::on_pushButton_filterSettings_clicked() {
    // Open filter settings window
    FilterSettingsWindow filterStatWindow;
    filterStatWindow.setModal(true);
    filterStatWindow.exec();
}

void MainWindow::on_pushButton_statistics_clicked() {
    // Open dialog with packet details with an argument
    StatWindow statWindow(captured_packets_vect);
    statWindow.setModal(true);
    statWindow.exec();
}

void MainWindow::on_pushButton_clear_clicked() {
    // Empty the vector
    captured_packets_vect.clear();

    // Reset the row count
    row_count = 0;

    // Clear the table
    ui->tableWidget_packets->clearContents();
    ui->tableWidget_packets->setRowCount(0);

    ui->label_hint->hide();
}

/*
 * By clicking on pushButton_side_by_side, you will open the SideBySideWindow screen.
 * A check if performed first:
 * - If you have clicked on a row, open the side by side view for this host address
 * - If no row is selected, check if only 1 possible server address exists. If so, open with this address.
 * - Otherwise display error message
 */
void MainWindow::on_pushButton_side_by_side_clicked()
{
    // Open dialog with packet details with an argument
    QString server_address;
    int current_row = ui->tableWidget_packets->currentRow();
    QVector<QString> temp_hosts;

    if(current_row != -1) { // Check that a row is actually selected
        // Look for server address (by checking that each side is not equal to my_ip_address
        if(captured_packets_vect[current_row].getIp_source_address() == my_ip_address) {
            server_address = captured_packets_vect[current_row].getIp_destination_address();
        } else {
            server_address = captured_packets_vect[current_row].getIp_source_address();
        }

        current_row = -1; // Set back to -1 (in case the user clears the screen before pressing the button again)

        // Open window
        SideBySideWindow sideBySideWindow(captured_packets_vect, server_address);
        sideBySideWindow.setModal(true);
        sideBySideWindow.exec();

    } else { // No row selected
        // Find all possible server addresses
        for(int i = 0; i < ui->tableWidget_packets->rowCount(); i++) {
            // Check for source hosts
            if(!(temp_hosts.contains(captured_packets_vect[i].getIp_source_address())) && (captured_packets_vect[i].getIp_source_address() != my_ip_address)) { // Check if host in vector
                temp_hosts.append(captured_packets_vect[i].getIp_source_address()); // If host isn't in vector, add it
            }

            // Do the same for destination hosts
            if(!(temp_hosts.contains(captured_packets_vect[i].getIp_destination_address())) && (captured_packets_vect[i].getIp_destination_address() != my_ip_address))
                temp_hosts.append(captured_packets_vect[i].getIp_destination_address());
            }

        // If there is only 1 possible server host, open the SideBySide window with this
        if(temp_hosts.count() == 1) {
            SideBySideWindow sideBySideWindow(captured_packets_vect, temp_hosts[0]);
            sideBySideWindow.setModal(true);
            sideBySideWindow.exec();
        // Otherwise display error message
        } else {
            QMessageBox::critical(this, "Error", "More than one possible server address. Please click on a row before opening side by side view.");
        }
    }
}

/*
 * For testing purposes
 */
void MainWindow::on_pushButton_test_clicked()
{
    qDebug() << "Current row:" << ui->tableWidget_packets->currentRow();
}
