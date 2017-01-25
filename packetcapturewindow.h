#ifndef PACKETCAPTUREWINDOW_H
#define PACKETCAPTUREWINDOW_H

#include <QDialog>
#include <pcap.h>

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


private slots:
    void on_button_applyFilter_clicked();
    void on_button_close_handle_clicked();
    void on_button_capture_packet_clicked();
    void on_pushButton_test_clicked();
    void on_button_capture_stream_clicked();

private:
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
};

#endif // PACKETCAPTUREWINDOW_H
