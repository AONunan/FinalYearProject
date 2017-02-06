#include "packettracer.h"
#include <QDebug>
#include "pcap.h"
#include "packetcapturewindow.h"
//#include <QByteArray>

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

void PacketTracer::print_payload(const u_char *payload, int len) {
    int len_rem = len;
    int line_width = 16; // Number of bytes in each line
    int line_len;
    int offset = 0; // zero-based offset counter
    const u_char *ch = payload; // Each character of the payload

    // Check for error
    if (len <= 0)
        return;

    qDebug() << "Len:" << len;

    // If length of data will fit on a single line
    if (len <= line_width) {
        print_hex_ascii_line(ch, len, offset);
    } else {
        // Otherwise, data takes up multiple lines
        for ( ;; ) {
            // Calculate current line length
            line_len = line_width % len_rem;

            // Print line
            print_hex_ascii_line(ch, line_len, offset);

            // Calculate total remaining
            len_rem = len_rem - line_len;

            // Move pointer to remaining bytes to print
            ch = ch + line_len;

            // Add offset
            offset = offset + line_width;

            // Check if nearing end of data
            if (len_rem <= line_width) {
                // Print last line
                print_hex_ascii_line(ch, len_rem, offset);
                break;
            }
        }
    }
}

void PacketTracer::print_hex_ascii_line(const u_char *payload, int len, int offset) {

    int i;
    int gap;
    QString valueInHex;
    int temp;

    const u_char *ch;
    qDebug() << "Offset:" << offset;

    ch = payload;

    for(i = 0; i < len; i++) {

        temp = *ch;
        valueInHex = QString("%1").arg(temp , 0, 16);
        qDebug() << "Payload = " << valueInHex;
        ch++;
    }
}
