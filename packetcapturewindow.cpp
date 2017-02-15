#include "packecapturewindow.h"
#include "ui_packetcapturewindow.h"
#include <QDebug>
#include <QTableWidget>

#include "packet.h"

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

    TcpPacket my_captured_packet;
    packet = 0;

    // Packets sometimes return as 0, causing errors. Loop until non-0 value returned
    while(packet == 0) {
        // Fetch single packet
        packet = pcap_next(handle, &header);
    }

    // Process captured packet
    my_captured_packet = packetTracer.captured_packet(&header, packet);

    // TODO: update UI
    update_table(my_captured_packet, row_count);

}

void PacketCaptureWindow::update_table(TcpPacket packet, int row) {
    ui->tableWidget_packets->setRowCount(row_count + 1); // Add a new row
    ui->tableWidget_packets->setItem(row, 0, new QTableWidgetItem("Test"));
    ui->tableWidget_packets->setItem(row, 3, new QTableWidgetItem(packet.getPayload_length()));

    row_count++;
}

void PacketCaptureWindow::on_pushButton_filterSettings_clicked()
{

}
