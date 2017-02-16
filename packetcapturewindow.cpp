#include "packecapturewindow.h"
#include "ui_packetcapturewindow.h"
#include <QDebug>
#include <QTableWidget>

#include "packet.h"

// These correspond to the columns in the table
#define HEADER_PACKET_COUNT 0
#define HEADER_TIMESTAMP 1
#define HEADER_PROTOCOL 2
#define HEADER_PAYLOAD_LENGTH 3

int Packet::count = 0;

PacketCaptureWindow::PacketCaptureWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketCaptureWindow)
{
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

PacketCaptureWindow::~PacketCaptureWindow()
{
    delete ui;
}

void PacketCaptureWindow::on_button_applyFilter_clicked()
{
    packetTracer.apply_filter(handle, &filter_expression, net);

    ui->button_applyFilter->setEnabled(false);
    ui->button_capture_packet->setEnabled(true);
}

void PacketCaptureWindow::on_button_close_handle_clicked()
{
    // Close handle
    pcap_close(handle);

    PacketCaptureWindow::close();
}

void PacketCaptureWindow::on_button_capture_packet_clicked()
{
    Packet my_captured_packet;

    packet = 0;

    // Packets sometimes return as 0, causing errors. Loop until non-0 value returned
    while(packet == 0) {
        // Fetch single packet
        packet = pcap_next(handle, &header);
    }

    // Process captured packet
    my_captured_packet = packetTracer.captured_packet(&header, packet, my_captured_packet);

    // TODO: update UI
    update_table(my_captured_packet);

}

void PacketCaptureWindow::update_table(Packet packet) {
    ui->tableWidget_packets->setRowCount(row_count + 1); // Add a new row
    //ui->tableWidget_packets->setItem(row_count, HEADER_PACKET_COUNT, new QTableWidgetItem("Test"));
    ui->tableWidget_packets->setItem(row_count, HEADER_PACKET_COUNT, new QTableWidgetItem(QString::number(packet.getCount())));

    //ui->tableWidget_packets->setItem(row_count, 1, new QTableWidgetItem("Test").setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled));
    ui->tableWidget_packets->setItem(row_count, HEADER_PROTOCOL, new QTableWidgetItem(packet.getProtocol()));
    ui->tableWidget_packets->setItem(row_count, HEADER_PAYLOAD_LENGTH, new QTableWidgetItem(QString::number(packet.getPayload_length())));

    qDebug() << "# Packet payload length:" << packet.getPayload_length();

    row_count++;
}

void PacketCaptureWindow::on_tableWidget_packets_cellDoubleClicked(int row, int column) {
    qDebug() << "You double clicked on row" << row << "and column" << column;
}

void PacketCaptureWindow::on_pushButton_filterSettings_clicked()
{

}
