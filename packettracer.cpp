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

pcap_t* PacketTracer::open_for_sniffing(char* dev) {
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
    char filter_expession_string[] = "host 93.184.216.34";

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

    working_packet.setCurrent_time(time(0));

    //const struct ethernet_header *ethernetPtr; // Pointer to beginning of Ethernet header
    const struct ip_header *ipPtr; // Pointer to beginning of IP header
    const struct tcp_header *tcpPtr; // Pointer to beginning of TCP header
    const u_char *payloadPtr; // Pointer to beginning of packet payload

    int ip_header_length; // Length of IP header
    int tcp_header_length; // Length of TCP header
    int payload_length; // Length of packet payload

    working_packet.setTotal_header_length(header->len);

    // Define Ethernet header, same as pointer to packet
    //ethernetPtr = (struct ethernet_header*)(packet);

    // Define IP header, same as pointer plus ethernet length
    ipPtr = (struct ip_header*)(packet + SIZE_ETHERNET);

    // Calculate IP header length (i.e. offset)
    ip_header_length = ((ipPtr->version_and_header_length) & 0x0f) << 2; // Bitshift to the right to get header length
    working_packet.setIp_header_length(ip_header_length);

    qDebug() << "IP header length" << ip_header_length << "bytes";
    if (ip_header_length < 20) {
        qDebug() << "Invalid IP header length:" << ip_header_length << " bytes";
        return working_packet;
    } else {
        working_packet.setIp_header_length(ip_header_length);
    }

    // Set IP details
    working_packet.setIp_source_address(inet_ntoa(ipPtr->source_address));
    working_packet.setIp_destination_address(inet_ntoa(ipPtr->destination_address));
    working_packet.setIp_version((ipPtr->version_and_header_length) >> 4); // Bitshifting required to obtain version number
    working_packet.setIp_type_of_service(ipPtr->type_of_service);
    working_packet.setIp_length(ipPtr->length);
    working_packet.setIp_id(ipPtr->id);
    working_packet.setIp_offset(ipPtr->offset);
    working_packet.setIp_time_to_live(ipPtr->time_to_live);
    working_packet.setIp_protocol(ipPtr->protocol);
    working_packet.setIp_checksum(ipPtr->checksum);

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

    // Set TCP details
    working_packet.setTcp_source_port(ntohs(tcpPtr->source_port));
    working_packet.setTcp_destination_port(ntohs(tcpPtr->destination_port));
    working_packet.setTcp_sequence_number(tcpPtr->sequence_number);
    working_packet.setTcp_acknowledgement_number(tcpPtr->acknowledgement_number);
    working_packet.setTcp_offset(tcpPtr->offset);
    working_packet.setTcp_flags(tcpPtr->flags);
    working_packet.setTcp_window(tcpPtr->window);
    working_packet.setTcp_checksum(tcpPtr->checksum);
    working_packet.setTcp_urgent_pointer(tcpPtr->urgent_pointer);

    // Define packet payloadPtr
    payloadPtr = (u_char *)(packet + SIZE_ETHERNET + ip_header_length + tcp_header_length);

    // Calculate payloadPtr length
    payload_length = ntohs(ipPtr->length) - (ip_header_length + tcp_header_length);
    working_packet.setPayload_length(payload_length);

    // Make call to function that will display packet payload
    if (payload_length > 0) {
        working_packet.setPayload_length(payload_length);
        working_packet.setPayload_vect(print_payload(payloadPtr, payload_length)); // Set the payload contents by calling print_payload()
    } else {
        // Clear vector. Otherwise vectors that should be empty will have content
        working_packet.clearPayload_vect();
    }

    return working_packet;
}

QVector<short> PacketTracer::print_payload(const u_char *payloadPtr, int payload_length) {
    const u_char *next_char = payloadPtr; // Each character of the payload, initally a pointer to memory location of start of payload
    QVector<short> total_payload;
    int i; // Counter

    for(i = 0; i < payload_length; i++) {
        total_payload.append(*next_char);
        next_char++;
    }

    return total_payload;
}

