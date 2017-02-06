#include "packetcapturewindow.h"
#include "ui_packetcapturewindow.h"
#include <QDebug>



PacketCaptureWindow::PacketCaptureWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketCaptureWindow)
{
    ui->setupUi(this);

    //PacketTracer packetTracer;
    packetTracer.test_function();

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
    qDebug() << "Grabbing a single packet.";
    packet = pcap_next(handle, &header);

    // Find header length
    int header_length = header.len;
    QString header_length_string = QString(QString::number(header_length));

    // Display header length in window
    ui->listWidget_packets->addItem(header_length_string);
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

void PacketCaptureWindow::on_button_capture_stream_clicked()
{
    // Capture 10 packets, call captured_packet() each time
    pcap_loop(handle, 10, captured_packet, NULL);
}

void PacketCaptureWindow::captured_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    qDebug() << "**************************************************";

    QString packetDetails = "";

    static int packetCount = 1;

    const struct sniff_ethernet *ethernetPtr; // Pointer to beginning of Ethernet header
    const struct sniff_ip *ipPtr; // Pointer to beginning of IP header
    const struct sniff_tcp *tcpPtr; // Pointer to beginning of TCP header
    const u_char *payload; // Pointer to beginning of packet payload

    int ip_length; // Length of IP header
    int tcp_length; // Length of TCP header
    int payload_length; // Length of packet payload

    qDebug() << "Packet number:" << packetCount;
    packetCount++;

    // Define Ethernet header
    ethernetPtr = (struct sniff_ethernet*)(packet);

    // Define IP header
    ipPtr = (struct sniff_ip*)(packet + SIZE_ETHERNET);

    // Calculate IP header length (i.e. offset)
    ip_length = IP_HL(ipPtr)*4;
    qDebug() << "IP header length" << ip_length << "bytes";
    if (ip_length < 20) {
        qDebug() << "   * Invalid IP header length:" << ip_length << " bytes";
        return;
    }

    qDebug() << "Source IP:" << inet_ntoa(ipPtr->ip_src);
    qDebug() << "Destination IP:" << inet_ntoa(ipPtr->ip_dst);

    // Find protocol in use
    switch(ipPtr->ip_p) {
    case IPPROTO_TCP:
        qDebug() << "Protocol: TCP";
        break; // continue below if protocol = TCP
    case IPPROTO_UDP:
        qDebug() << "Protocol: UDP";
        return;
    case IPPROTO_ICMP:
        qDebug() << "Protocol: ICMP";
        return;
    case IPPROTO_IP:
        qDebug() << "Protocol: IP";
        return;
    default:
        qDebug() << "Protocol: unknown";
        return;
    }

    // If from here down is executed, we must be dealing with TCP

    // Define IP header
    tcpPtr = (struct sniff_tcp*)(packet + SIZE_ETHERNET + ip_length);

    // Calculate TCP header length (i.e. offset)
    tcp_length = TH_OFF(tcpPtr)*4;
    qDebug() << "TCP header length" << tcp_length << "bytes";
    if (tcp_length < 20) {
        qDebug() << "   * Invalid TCP header length" << tcp_length << "bytes";
        return;
    }

    qDebug() << "Source port:" << ntohs(tcpPtr->th_sport);
    qDebug() << "Destination port:" << ntohs(tcpPtr->th_dport);

    // Define packet payload
    payload = (u_char *)(packet + SIZE_ETHERNET + ip_length + tcp_length);

    // Calculate payload length
    payload_length = ntohs(ipPtr->ip_len) - (ip_length + tcp_length);

    PacketTracer packetTracer;

    // Make call to function that will display packet payload
    if (payload_length > 0) {
        qDebug() << "Payload:" << payload_length << "bytes";
        packetTracer.print_payload(payload, payload_length);
    } else {
        qDebug() << "Payload size is 0";
    }

    return;
}
