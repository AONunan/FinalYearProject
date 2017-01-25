#include "packetcapturewindow.h"
#include "ui_packetcapturewindow.h"
#include <QDebug>
#include "packettracer.h"

PacketCaptureWindow::PacketCaptureWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketCaptureWindow)
{
    ui->setupUi(this);

    PacketTracer packetTracer;
    packetTracer.testFunction();

    // set the network interface device
    dev = packetTracer.getNetworkInterfaceDevice();

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

    // open the handle
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
    char filter_exp[] = "port 443";

    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        qDebug() << stderr;
        qDebug() << "Couldn't parse filter:" << pcap_geterr(handle);
    } else {
        qDebug() << "Filter compiled";
    }

    // TODO: Move to packettracer.cpp
    if (pcap_setfilter(handle, &fp) == -1) {
        qDebug() << stderr;
        qDebug() << "Couldn't install filter:" << pcap_geterr(handle);
    } else {
        qDebug() << "Filter set";
    }

    ui->button_applyFilter->setEnabled(false);
}

void PacketCaptureWindow::on_button_close_handle_clicked()
{
    /* And close the session */
    pcap_close(handle);

    PacketCaptureWindow::close();
}

void PacketCaptureWindow::on_button_capture_packet_clicked()
{
    PacketTracer packetTracer;

    qDebug() << "Grabbing a single packet.";
    packet = pcap_next(handle, &header);
    /* Print its length */
    int header_length = header.len;
    QString value = QString( QString::number(header_length));
    ui->listWidget_packets->addItem(value);
}

void PacketCaptureWindow::on_pushButton_test_clicked()
{
    // TODO: Complete filter settings
    PacketTracer packetTracer;
    QString source_host = ui->lineEdit_src_host->text();
    QString dest_host = ui->lineEdit_dst_host->text();
    QString source_port = ui->lineEdit_src_port->text();
    QString dest_port = ui->lineEdit_dst_port->text();
    QString new_filter_expression = packetTracer.getFilterExpression(source_host, dest_host, source_port, dest_port);
    qDebug() << "The chosen filter express is:" << new_filter_expression;
}

void PacketCaptureWindow::on_button_capture_stream_clicked()
{
    PacketTracer packetTracer;

    // capture 10 packets, call packetHandler() each time
    pcap_loop(handle, 10, captured_packet, NULL);
}

void PacketCaptureWindow::captured_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    static int packetCount = 1;

    /* declare pointers to packet headers */
    const struct sniff_ethernet *ethernet_header;  /* The ethernet header [1] */
    const struct sniff_ip *ip_header;              /* The IP header */
    const struct sniff_tcp *tcp_header;			/* The TCP header */
    const char *payload;					/* Packet payload */

    int ip_length;
    int tcp_length;
    int payload_length;

    qDebug() << "Packet number:" << packetCount;
    packetCount++;
}
