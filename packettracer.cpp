#include "packettracer.h"
#include <QDebug>
#include <QByteArray>

#include "packet.h"

// Constructor
PacketTracer::PacketTracer() {

}

char* PacketTracer::get_network_interface_device() {
    char *dev = NULL;
    char errbuf[PCAP_ERRBUF_SIZE]; // To hold any error messages

    dev = pcap_lookupdev(errbuf);

    if (dev == NULL) {
        qDebug() << stderr;
        qDebug() << "Couldn't find default device:" << errbuf;
        exit(EXIT_FAILURE);
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
    }
}

pcap_t * PacketTracer::open_for_sniffing(char* dev) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        qDebug() << stderr;
        qDebug() << "Couldn't open device:" << errbuf;
    }

    return handle;
}

void PacketTracer::apply_filter(pcap_t *handle, bpf_program *filter_expressionPtr, bpf_u_int32 net) {
    char filter_expession_string[] = "tcp";

    // Compile filter expression
    if (pcap_compile(handle, filter_expressionPtr, filter_expession_string, 0, net) == -1) {
        qDebug() << stderr;
        qDebug() << "Couldn't parse filter:" << pcap_geterr(handle);
    }

    // Set filter expression
    if (pcap_setfilter(handle, filter_expressionPtr) == -1) {
        qDebug() << stderr;
        qDebug() << "Couldn't install filter:" << pcap_geterr(handle);
    }
}

Packet PacketTracer::captured_packet(pcap_pkthdr *header, const u_char *packet, Packet working_packet) {
    qDebug() << "**************************************************";

    //const struct ethernet_header *ethernetPtr; // Pointer to beginning of Ethernet header
    const struct ip_header *ipPtr; // Pointer to beginning of IP header
    const struct tcp_header *tcpPtr; // Pointer to beginning of TCP header
    const u_char *payload; // Pointer to beginning of packet payload

    int ip_header_length; // Length of IP header
    int tcp_header_length; // Length of TCP header
    int payload_length; // Length of packet payload

    working_packet.setTotal_header_length(header->len);

    // Define Ethernet header, same as pointer to packet
    //ethernetPtr = (struct ethernet_header*)(packet);

    // Define IP header, same as pointer plus ethernet length
    ipPtr = (struct ip_header*)(packet + SIZE_ETHERNET);

    // Calculate IP header length (i.e. offset)
    ip_header_length = ((ipPtr->version) & 0x0f) * 4;
    working_packet.setIp_header_length(ip_header_length);

    qDebug() << "IP header length" << ip_header_length << "bytes";
    if (ip_header_length < 20) {
        qDebug() << "Invalid IP header length:" << ip_header_length << " bytes";
        return working_packet;
    } else {
        working_packet.setIp_header_length(ip_header_length);
    }

    //qDebug() << "Source IP:" << inet_ntoa(ipPtr->source_address);
    // TODO: Convert to QString
    //qDebug() << "Destination IP:" << inet_ntoa(ipPtr->destination_address);


    // Find protocol in use
    switch(ipPtr->protocol) {
    case IPPROTO_TCP:
        working_packet.setProtocol("TCP");
        break; // continue below if protocol = TCP
    case IPPROTO_UDP:
        working_packet.setProtocol("UDP");
        return working_packet;
    case IPPROTO_ICMP:
        working_packet.setProtocol("ICMP");
        return working_packet;
    case IPPROTO_IP:
        working_packet.setProtocol("IP");
        return working_packet;
    default:
        working_packet.setProtocol("unknown");
        return working_packet;
    }


    // If from here down is executed, we must be dealing with TCP

    // Define TCP header, equals packet pointer plus ethernet size plus ip size
    tcpPtr = (struct tcp_header*)(packet + SIZE_ETHERNET + ip_header_length);

    // Calculate TCP header length (i.e. offset)
    tcp_header_length = ((tcpPtr->offset & 0xf0) >> 4) * 4;
    working_packet.setTcp_header_length(tcp_header_length);

    qDebug() << "TCP header length" << tcp_header_length << "bytes";
    if (tcp_header_length < 20) {
        qDebug() << "Invalid TCP header length" << tcp_header_length << "bytes";
        return working_packet;
    } else {
        working_packet.setTcp_header_length(tcp_header_length);
    }

//    qDebug() << "Source port:" << ntohs(tcpPtr->source_port);
//    qDebug() << "Destination port:" << ntohs(tcpPtr->destination_port);

    // Define packet payload
    payload = (u_char *)(packet + SIZE_ETHERNET + ip_header_length + tcp_header_length);

    // Calculate payload length
    payload_length = ntohs(ipPtr->length) - (ip_header_length + tcp_header_length);
    working_packet.setPayload_length(payload_length);

//    qDebug() << "Payload length:" << payload_length;

    // Make call to function that will display packet payload
    if (payload_length > 0) {
        working_packet.setPayload_length(payload_length);
        print_payload(payload, payload_length);
    } else {
        qDebug() << "Payload size is 0";
    }

    return working_packet;

}

void PacketTracer::print_payload(const u_char *payload, int payload_length) {
    qDebug() << "Beggining payload printing...";
    int remaining_length = payload_length;
    int line_width = 16; // Number of bytes in each line
    int line_length;
    int offset = 0; // zero-based offset counter
    const u_char *next_char = payload; // Each character of the payload


    /*int i;

    for(i = 0; i < payload_length; i++) {
        qDebug() << "Entering for loop";
        get_hex_ascii(next_char, line_length, offset);
    }*/

    // If length of data will fit on a single line
    if (payload_length <= line_width) {
        get_hex_ascii(next_char, payload_length, offset);
    } else {
        // Otherwise, data takes up multiple lines
        while(true) {
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
    int temp;

    const u_char *ch;
    //qDebug() << "Offset:" << offset;

    ch = payload;

    for(i = 0; i < length; i++) {
        temp = *ch;
        value_in_hex = QString("%1").arg(temp , 0, 16);

        // PAYLOAD CONTENTS
        //qDebug() << "Payload:" << value_in_hex;
        ch++;
    }
}
