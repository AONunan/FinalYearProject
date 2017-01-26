#include "packettracer.h"
#include <QDebug>
#include "pcap.h"
#include "packetcapturewindow.h"

// Constructor
PacketTracer::PacketTracer()
{

}

void PacketTracer::testFunction() {
    qDebug() << "I am a test function in packettracer.cpp";
}

char* PacketTracer::getNetworkInterfaceDevice() {
    char *dev = NULL;
    char errbuf[PCAP_ERRBUF_SIZE]; // to hold any possible error messages

    dev = pcap_lookupdev(errbuf);

    if (dev == NULL) {
        qDebug() << stderr;
        qDebug() << "Couldn't find default device:" << errbuf;
        exit(EXIT_FAILURE);
    } else {
        qDebug() << "Device found";
    }

    return dev;
}

void PacketTracer::setMaskAndIp(char* dev, bpf_u_int32* netPtr, bpf_u_int32* maskPtr) {
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_lookupnet(dev, netPtr, maskPtr, errbuf) == -1) {
        qDebug() << stderr;
        qDebug() << "Can't get netmask for device:" << dev;
        netPtr = 0;
        maskPtr = 0;
    } else {
        qDebug() << "Net and mask set";
    }
}

void PacketTracer::openForSniffing(char* dev, pcap_t* handle) {
    char errbuf[PCAP_ERRBUF_SIZE];

    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        qDebug() << stderr;
        qDebug() << "Couldn't open device:" << errbuf;
    } else {
        qDebug() << "Device is open for sniffing";
    }
}

void PacketTracer::applyFilter(pcap_t* handle, struct bpf_program* fpPtr, bpf_u_int32 net) {

}

QString PacketTracer::getFilterExpression(QString source_host, QString dest_host, QString source_port, QString dest_port) {
    QString source_filter_expression = "";
    QString destination_filter_expression = "";
    bool expression_has_text = false; // Checks if there is already text in the expression

    if(source_host != "") {
        source_filter_expression.append(expression_has_text ? " " : "");
        source_filter_expression.append("host " + source_host);
        expression_has_text = true;
    }

    if(source_port != "") {
        source_filter_expression.append(expression_has_text ? " " : "");
        source_filter_expression.append("port " + source_port);
        expression_has_text = true;
    }

    QString new_filter_expression = source_filter_expression + destination_filter_expression;
    return new_filter_expression;
}

void PacketTracer::packetHandler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    qDebug() << "You captured a packet";
}
