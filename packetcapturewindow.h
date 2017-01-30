#ifndef PACKETCAPTUREWINDOW_H
#define PACKETCAPTUREWINDOW_H

#include <QDialog>
#include <pcap.h>
#include "packettracer.h"


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


#define IP_RF 0x8000			/* reserved fragment flag */
#define IP_DF 0x4000			/* dont fragment flag */
#define IP_MF 0x2000			/* more fragments flag */
#define IP_OFFMASK 0x1fff	/* mask for fragmenting bits */
#define IP_HL(ip)			   (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)				(((ip)->ip_vhl) >> 4)

#define TH_OFF(th)	  (((th)->th_offx2 & 0xf0) >> 4)
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_FLAGS		(TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)



namespace Ui {
class PacketCaptureWindow;
}

class PacketCaptureWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PacketCaptureWindow(QWidget *parent = 0);
    ~PacketCaptureWindow();

    static void captured_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
    void testFunction();
private slots:
    void on_button_applyFilter_clicked();
    void on_button_close_handle_clicked();
    void on_button_capture_packet_clicked();
    void on_pushButton_test_clicked();
    void on_button_capture_stream_clicked();

private:
    PacketTracer packetTracer;

    Ui::PacketCaptureWindow *ui;

    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;                     // Session handle
    struct bpf_program fp;              // The compiled filter expression
    //char filter_exp[] = "port 443";	// The filter expression
    bpf_u_int32 mask;                   // The netmask of our sniffing device
    bpf_u_int32 net;                    // The IP of our sniffing device
    struct pcap_pkthdr header;          // The header that pcap gives us
    const u_char *packet;               // The actual packet


    /* Ethernet header */
    struct sniff_ethernet {
        u_char  ether_dhost[ETHER_ADDR_LEN];	/* destination host address */
        u_char  ether_shost[ETHER_ADDR_LEN];	/* source host address */
        u_short ether_type;					/* IP? ARP? RARP? etc */
    };

    /* IP header */
    struct sniff_ip {
        u_char  ip_vhl;				/* version << 4 | header length >> 2 */
        u_char  ip_tos;				/* type of service */
        u_short ip_len;				/* total length */
        u_short ip_id;				/* identification */
        u_short ip_off;				/* fragment offset field */
        u_char  ip_ttl;				/* time to live */
        u_char  ip_p;				/* protocol */
        u_short ip_sum;				/* checksum */
        struct  in_addr ip_src,ip_dst;  /* source and dest address */
    };


    /* TCP header */
    typedef u_int tcp_seq;

    struct sniff_tcp {
        u_short th_sport;			/* source port */
        u_short th_dport;			/* destination port */
        tcp_seq th_seq;				/* sequence number */
        tcp_seq th_ack;				/* acknowledgement number */
        u_char  th_offx2;			/* data offset, rsvd */
        u_char  th_flags;
        u_short th_win;				/* window */
        u_short th_sum;				/* checksum */
        u_short th_urp;				/* urgent pointer */
    };

};

#endif // PACKETCAPTUREWINDOW_H
