#include "packettracer.h"
#include <QDebug>
//#include <QByteArray>

#include "packet.h"

// Constructor
PacketTracer::PacketTracer()
{

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

void PacketTracer::apply_filter(pcap_t *handle, bpf_program *filter_expression, bpf_u_int32 net) {

    char filter_exp[] = "tcp";

    // Compile filter expression
    if (pcap_compile(handle, filter_expression, filter_exp, 0, net) == -1) {
        qDebug() << stderr;
        qDebug() << "Couldn't parse filter:" << pcap_geterr(handle);
    } else {
        qDebug() << "Filter compiled";
    }

    // Set filter expression
    if (pcap_setfilter(handle, filter_expression) == -1) {
        qDebug() << stderr;
        qDebug() << "Couldn't install filter:" << pcap_geterr(handle);
    } else {
        qDebug() << "Filter set";
    }
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

void PacketTracer::captured_packet(pcap_pkthdr *header, const u_char *packet) {
    qDebug() << "**************************************************";
    Packet working_packet;

//    int header_length = header.len;
    int header_length = header->len;
    QString header_length_string = "Header length: " + QString(QString::number(header_length));

    const struct sniff_ethernet *ethernetPtr; // Pointer to beginning of Ethernet header
    const struct sniff_ip *ipPtr; // Pointer to beginning of IP header
    const struct sniff_tcp *tcpPtr; // Pointer to beginning of TCP header
    const u_char *payload; // Pointer to beginning of packet payload

    int ip_length; // Length of IP header
    int tcp_length; // Length of TCP header
    int payload_length; // Length of packet payload

    // Define Ethernet header
    ethernetPtr = (struct sniff_ethernet*)(packet);

    // Define IP header
    ipPtr = (struct sniff_ip*)(packet + SIZE_ETHERNET);

    // Calculate IP header length (i.e. offset)
    ip_length = ((ipPtr->ip_vhl) & 0x0f) * 4;
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
    tcp_length = ((tcpPtr->th_offx2 & 0xf0) >> 4) * 4;
    qDebug() << "TCP header length" << tcp_length << "bytes";
    if (tcp_length < 20) {
        qDebug() << "Invalid TCP header length" << tcp_length << "bytes";
        return;
    }

    qDebug() << "Source port:" << ntohs(tcpPtr->th_sport);
    qDebug() << "Destination port:" << ntohs(tcpPtr->th_dport);

    // Define packet payload
    payload = (u_char *)(packet + SIZE_ETHERNET + ip_length + tcp_length);

    // Calculate payload length
    payload_length = ntohs(ipPtr->ip_len) - (ip_length + tcp_length);


    //PacketTracer packetTracer;

    // Make call to function that will display packet payload
    if (payload_length > 0) {
        qDebug() << "Payload:" << payload_length << "bytes";
        print_payload(payload, payload_length);
    } else {
        qDebug() << "Payload size is 0";
    }

    working_packet.set_ip_header(ip_length);
    working_packet.set_tcp_header(tcp_length);
    working_packet.set_payload(payload_length);

    working_packet.display_packet_info();

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
    //qDebug() << "Offset:" << offset;

    ch = payload;

    for(i = 0; i < length; i++) {

        temp = *ch;
        value_in_hex = QString("%1").arg(temp , 0, 16);

        // PAYLOAD CONTENTS
        //qDebug() << "Payload:" << value_in_hex;
        //qDebug() << value_in_hex.toUtf8();
        ch++;
    }
}
