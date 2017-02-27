#ifndef PACKETTRACER_H
#define PACKETTRACER_H


#include <QString>
#include <pcap.h>

#include "packet.h"

//#include <ctype.h>
//#include <errno.h>
//#include <sys/types.h>
#include <sys/socket.h> // For converting host addresses to readable format
#include <netinet/in.h> // For converting host addresses to readable format
#include <arpa/inet.h> // For converting ports to readable format


#define SNAP_LEN 1518           /* default snap length (maximum bytes per packet to capture) */
#define SIZE_ETHERNET 14        /* ethernet headers are always exactly 14 bytes [1] */
#define ETHER_ADDR_LEN	6       /* Ethernet addresses are 6 bytes */


#define IP_RF 0x8000            /* reserved fragment flag */
#define IP_DF 0x4000            /* dont fragment flag */
#define IP_MF 0x2000            /* more fragments flag */
#define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */

#define TCP_FIN  0x01
#define TCP_SYN  0x02
#define TCP_RST  0x04
#define TCP_PUSH 0x08
#define TCP_ACK  0x10
#define TCP_URG  0x20
#define TCP_ECE  0x40
#define TCP_CWR  0x80
#define TCP_FLAGS		(TCP_FIN|TCP_SYN|TCP_RST|TCP_ACK|TCP_URG|TCP_ECE|TCP_CWR)


class PacketTracer {
public:
    PacketTracer();
    char* get_network_interface_device();
    void set_mask_and_ip(char *dev, bpf_u_int32 *netPtr, bpf_u_int32 *maskPtr);
    pcap_t* open_for_sniffing(char *dev);
    void apply_filter(pcap_t *handle, bpf_program *filter_expressionPtr, bpf_u_int32 net);
    QVector<short> print_payload(const u_char *payloadPtr, int payload_length);
    Packet captured_packet(pcap_pkthdr *header, const u_char *packet, Packet working_packet);
private:

    // Ethernet header
    struct ethernet_header {
        u_char destination_address[ETHER_ADDR_LEN];
        u_char source_address[ETHER_ADDR_LEN];
        u_short type; // IP, ARP, RARP, etc
    };

    // IP header
    struct ip_header {
        u_char version_and_header_length; // IP version has been bitshifted 4 places to the right. Header length has been bit shifted 2 places to the left. Will require shifting to obtain proper values later
        u_char type_of_service;
        u_short length;
        u_short id;
        u_short offset; // Fragment offset field
        u_char time_to_live;
        u_char protocol;
        u_short checksum;
        struct in_addr source_address, destination_address;
    };


    // TCP header
    typedef u_int tcp_seq;

    struct tcp_header {
        u_short source_port;
        u_short destination_port;
        tcp_seq sequence_number;
        tcp_seq acknowledgement_number;
        u_char offset; // data offset, rsvd
        u_char flags;
        u_short window;
        u_short checksum;
        u_short urgent_pointer;
    };

};

#endif // PACKETTRACER_H
