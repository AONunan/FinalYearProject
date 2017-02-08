#include "packettracer.h"
#include <QDebug>
#include "pcap.h"
#include "packetcapturewindow.h"
//#include <QByteArray>

// Constructor
PacketTracer::PacketTracer()
{

}

void PacketTracer::test_function() {
    qDebug() << "I am a test function in packettracer.cpp";
}

char* PacketTracer::get_network_interface_device() {
    char *dev = NULL;
    char errbuf[PCAP_ERRBUF_SIZE]; // To hold any error messages

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

void PacketTracer::set_mask_and_ip(char* dev, bpf_u_int32* netPtr, bpf_u_int32* maskPtr) {
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

void PacketTracer::open_for_sniffing(char* dev, pcap_t* handle) {
    char errbuf[PCAP_ERRBUF_SIZE];

    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        qDebug() << stderr;
        qDebug() << "Couldn't open device:" << errbuf;
    } else {
        qDebug() << "Device is open for sniffing";
    }
}

void PacketTracer::apply_filter(pcap_t* handle, struct bpf_program* fpPtr, bpf_u_int32 net) {

}

QString PacketTracer::get_filter_expression(QString source_host, QString dest_host, QString source_port, QString dest_port) {
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

void PacketTracer::print_payload(const u_char *payload, int payload_length) {
    int remaining_length = payload_length;
    int line_width = 16; // Number of bytes in each line
    int line_length;
    int offset = 0; // zero-based offset counter
    const u_char *next_char = payload; // Each character of the payload

    // Check for error
    if (payload_length <= 0)
        return;

    qDebug() << "Len:" << payload_length;

    // If length of data will fit on a single line
    if (payload_length <= line_width) {
        get_hex_ascii(next_char, payload_length, offset);
    } else {
        // Otherwise, data takes up multiple lines
        for ( ;; ) {
            // Get length of current line
            line_length = line_width % remaining_length;

            // Print line
            get_hex_ascii(next_char, line_length, offset);

            // Calculate total remaining
            remaining_length = remaining_length - line_length;

            // Move pointer to remaining bytes to print
            next_char = next_char + line_length;

            // Add offset
            offset = offset + line_width;

            // Check if nearing end of data
            if (remaining_length <= line_width) {
                // Print last line
                get_hex_ascii(next_char, remaining_length, offset);
                break;
            }
        }
    }
}

void PacketTracer::get_hex_ascii(const u_char *payload, int length, int offset) {
    // TODO: implement conversion to ascii
    int i;
    //int gap;
    QString value_in_hex;
    QString value_in_ascii;
    int temp;

    const u_char *ch;
    qDebug() << "Offset:" << offset;

    ch = payload;

    for(i = 0; i < length; i++) {

        temp = *ch;
        value_in_hex = QString("%1").arg(temp , 0, 16);
        qDebug() << "Payload = " << value_in_hex;
        //qDebug() << "ASCII   = " << value_in_ascii;
        qDebug() << value_in_hex.toUtf8();
        ch++;
    }
}
