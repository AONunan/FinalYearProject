#include "packetcapturewindow.h"
#include "ui_packetcapturewindow.h"
#include <QDebug>

#include "packet.h"

int Packet::count = 0;

PacketCaptureWindow::PacketCaptureWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketCaptureWindow)
{
    ui->setupUi(this);

    // set the network interface device
    dev = packetTracer.get_network_interface_device();

    ui->label_interface->setText(dev);

    // set network ID and subnet mask
    ///////////////packetTracer.setMaskAndIp(dev, &net, &mask);
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        qDebug() << stderr;
        qDebug() << "Can't get netmask for device:" << dev;
        net = 0;
        mask = 0;
    } else {
        qDebug() << "Net and mask set";
    }

    // Open the handle
    ///////////////packetTracer.openForSniffing(dev, handle);
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        qDebug() << stderr;
        qDebug() << "Couldn't open device:" << errbuf;
    } else {
        qDebug() << "Device is open for sniffing";
    }
}

PacketCaptureWindow::~PacketCaptureWindow()
{
    delete ui;
}

void PacketCaptureWindow::on_button_applyFilter_clicked()
{
    char filter_exp[] = "tcp";

    // Compile filter expression
    if (pcap_compile(handle, &filter_expression, filter_exp, 0, net) == -1) {
        qDebug() << stderr;
        qDebug() << "Couldn't parse filter:" << pcap_geterr(handle);
    } else {
        qDebug() << "Filter compiled";
    }

    // Set filter expression
    // TODO: Move to packettracer.cpp
    if (pcap_setfilter(handle, &filter_expression) == -1) {
        qDebug() << stderr;
        qDebug() << "Couldn't install filter:" << pcap_geterr(handle);
    } else {
        qDebug() << "Filter set";
    }

    ui->button_applyFilter->setEnabled(false);
}

void PacketCaptureWindow::on_button_close_handle_clicked()
{
    // Close handle
    pcap_close(handle);

    PacketCaptureWindow::close();
}

void PacketCaptureWindow::on_button_capture_packet_clicked()
{
    packet = 0;

    // Packets sometimes return as 0, causing errors. Loop until non-0 value returned
    while(packet == 0) {
        // Fetch single packet
        packet = pcap_next(handle, &header);
    }

    // Process captured packet
    packetTracer.captured_packet(&header, packet);
    // TODO: update UI
}

void PacketCaptureWindow::on_pushButton_test_clicked()
{
    // TODO: Complete filter settings
    QString source_host = ui->lineEdit_src_host->text();
    QString dest_host = ui->lineEdit_dst_host->text();
    QString source_port = ui->lineEdit_src_port->text();
    QString dest_port = ui->lineEdit_dst_port->text();
    QString new_filter_expression = packetTracer.get_filter_expression(source_host, dest_host, source_port, dest_port);
    qDebug() << "The chosen filter express is:" << new_filter_expression;
}
