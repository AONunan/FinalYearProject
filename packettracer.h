#ifndef PACKETTRACER_H
#define PACKETTRACER_H


#include <QString>
#include <pcap.h>

class PacketTracer
{
public:
    PacketTracer();
    void test_function();
    char* get_network_interface_device();
    void set_mask_and_ip(char *dev, bpf_u_int32 *netPtr, bpf_u_int32 *maskPtr);
    void open_for_sniffing(char *dev, pcap_t *handle);
    void apply_filter(pcap_t *handle, bpf_program *fpPtr, bpf_u_int32 net);
    QString get_filter_expression(QString source_host, QString dest_host, QString source_port, QString dest_port);
    void print_payload(const u_char *payload, int payload_length);
    void get_hex_ascii(const u_char *payload, int length, int offset);
private:

};

#endif // PACKETTRACER_H
