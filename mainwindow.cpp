#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTableWidget>
#include <QDateTime>

#include "packet.h"
#include "packetinfodialog.h"
#include "statwindow.h"

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

    row_count = 0;
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_button_applyFilter_clicked() {
    packetTracer.apply_filter(handle, &filter_expression, net);

    ui->button_applyFilter->setEnabled(false);
    ui->button_capture_packet->setEnabled(true);
}

void MainWindow::on_button_close_handle_clicked() {
    // Close handle
    pcap_close(handle);

    MainWindow::close();
}

void MainWindow::on_button_capture_packet_clicked() {
    status_bar_message = "Waiting for first packet.";
    ui->statusBar->showMessage(status_bar_message);

    Packet my_captured_packet;
    int i;

    // Get number of packets required from spinbox
    int no_of_packets = ui->spinBox_no_of_packets->value();

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

        //ui->label_app_status->setText(QString::number(i));
        status_bar_message = QString("Captured %1 of %2.").arg(QString::number(i + 1), QString::number(no_of_packets));
        ui->statusBar->showMessage(status_bar_message);
    }

    // Add each item in the vector to the UI
    for(i = captured_packets_vect.length() - no_of_packets; i < captured_packets_vect.length(); i++) {
        qDebug() << "**** Adding item:" << i;
        update_table(captured_packets_vect[i]);
    }

    status_bar_message = QString("Finished capturing %1 packets.").arg(QString::number(no_of_packets));
    ui->statusBar->showMessage(status_bar_message);

}

void MainWindow::update_table(Packet packet) {
    ui->tableWidget_packets->setRowCount(row_count + 1); // Add a new row
    ui->tableWidget_packets->scrollToBottom();
    //ui->tableWidget_packets->setItem(row_count, HEADER_PACKET_COUNT, new QTableWidgetItem("Test"));
    ui->tableWidget_packets->setItem(row_count, HEADER_PROTOCOL, new QTableWidgetItem(packet.getProtocol()));
    //ui->tableWidget_packets->setItem(row_count, HEADER_SRC_HOST, new QTableWidgetItem(packet));
    ui->tableWidget_packets->setItem(row_count, HEADER_PROTOCOL, new QTableWidgetItem(packet.getProtocol()));
    ui->tableWidget_packets->setItem(row_count, HEADER_PAYLOAD_LENGTH, new QTableWidgetItem(QString::number(packet.getPayload_length())));

    qDebug() << "# Packet payload length:" << packet.getPayload_length();

    row_count++;
}

void MainWindow::on_tableWidget_packets_cellDoubleClicked(int row, int column) {
    qDebug() << "You double clicked on row" << row << "and column" << column;

    // Open dialog with packet details with an argument
    PacketInfoDialog infoDialog("Test");
    infoDialog.setModal(true);
    infoDialog.exec();
}

void MainWindow::on_pushButton_filterSettings_clicked() {
    int i; // Counter

    QDateTime timestamp;
    tm *ltm;

    for(i = 0; i < captured_packets_vect.length(); i++) {
        qDebug() << "-------";
        qDebug() << "Protocol:" << captured_packets_vect[i].getProtocol();
        qDebug() << "Time:" << captured_packets_vect[i].getCurrent_time();
        timestamp.setTime_t(captured_packets_vect[i].getCurrent_time());
        qDebug() << timestamp.toString(Qt::SystemLocaleShortDate);
        long long_Time = (long)captured_packets_vect[i].getCurrent_time();
        ltm = localtime(&long_Time);
        qDebug() << "Hour   :" << ltm->tm_hour;
        qDebug() << "Minute :" << ltm->tm_min;
        qDebug() << "Second :" << ltm->tm_sec;
    }
}

void MainWindow::on_pushButton_statistics_clicked() {
    // Open dialog with packet details with an argument
    StatWindow statWindow;
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
}
