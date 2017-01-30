#ifndef PACKETTRACER_H
#define PACKETTRACER_H


#include <QString>
#include <pcap.h>

class PacketTracer
{
public:
    PacketTracer();
    void testFunction();
    char* getNetworkInterfaceDevice();
    void setMaskAndIp(char *dev, bpf_u_int32 *netPtr, bpf_u_int32 *maskPtr);
    void openForSniffing(char *dev, pcap_t *handle);
    void applyFilter(pcap_t *handle, bpf_program *fpPtr, bpf_u_int32 net);
    QString getFilterExpression(QString source_host, QString dest_host, QString source_port, QString dest_port);
    void packetHandler(u_char *args, const pcap_pkthdr *header, const u_char *packet);
    void print_payload(const u_char *payload, int len);
    void print_hex_ascii_line(const u_char *payload, int len, int offset);
private:

};

#endif // PACKETTRACER_H
