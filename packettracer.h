#ifndef PACKETTRACER_H
#define PACKETTRACER_H


#include <QString>
#include <pcap.h>

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SNAP_LEN 1518           /* default snap length (maximum bytes per packet to capture) */
#define SIZE_ETHERNET 14        /* ethernet headers are always exactly 14 bytes [1] */
#define ETHER_ADDR_LEN	6       /* Ethernet addresses are 6 bytes */


#define IP_RF 0x8000            /* reserved fragment flag */
#define IP_DF 0x4000            /* dont fragment flag */
#define IP_MF 0x2000            /* more fragments flag */
#define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */

#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_FLAGS		(TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)


class PacketTracer
{
public:
    PacketTracer();
    char* get_network_interface_device();
    void set_mask_and_ip(char *dev, bpf_u_int32 *netPtr, bpf_u_int32 *maskPtr);
    void open_for_sniffing(char *dev, pcap_t *handle);
    void apply_filter(pcap_t *handle, bpf_program *filter_expression, bpf_u_int32 net);
    QString get_filter_expression(QString source_host, QString dest_host, QString source_port, QString dest_port);
    void print_payload(const u_char *payload, int payload_length);
    void get_hex_ascii(const u_char *payload, int length, int offset);
    void captured_packet(pcap_pkthdr *header, const u_char *packet);
private:

    // Ethernet header
    struct sniff_ethernet {
        u_char ether_dhost[ETHER_ADDR_LEN]; // destination host address
        u_char ether_shost[ETHER_ADDR_LEN]; // source host address
        u_short ether_type; // IP, ARP, RARP, etc
    };

    // IP header
    struct sniff_ip {
        u_char ip_vhl; // version << 4 | header length >> 2
        u_char ip_tos; // type of service
        u_short ip_len; // total length
        u_short ip_id; // identification
        u_short ip_off; // fragment offset field
        u_char ip_ttl; // time to live
        u_char ip_p; // protocol
        u_short ip_sum; // checksum
        struct in_addr ip_src, ip_dst;  // source and dest address
    };


    // TCP header
    typedef u_int tcp_seq;

    struct sniff_tcp {
        u_short th_sport; // source port
        u_short th_dport; // destination port
        tcp_seq th_seq; // sequence number
        tcp_seq th_ack; // acknowledgement number
        u_char th_offx2; // data offset, rsvd
        u_char th_flags;
        u_short th_win; // window
        u_short th_sum; // checksum
        u_short th_urp; // urgent pointer
    };

};

#endif // PACKETTRACER_H
