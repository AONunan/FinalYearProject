#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pcap.h>
#include <QHostInfo>

#include "packettracer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void update_table(Packet packet);
    void capture_loop();
    QString find_my_ip_address();
private slots:
    void on_button_close_handle_clicked();
    void on_button_capture_packet_clicked();
    void on_tableWidget_packets_cellDoubleClicked(int row);
    void on_pushButton_statistics_clicked();
    void on_pushButton_clear_clicked();
    void on_pushButton_side_by_side_clicked();
    void on_pushButton_test_clicked();
    void on_button_check_clicked();
    void on_button_applyFilter_clicked();
    void on_button_clear_settings_fields_clicked();

private:
    Ui::MainWindow *ui;

    PacketTracer packetTracer;

    int row_count;                          // Keep track of current row to populate with packet details
    QVector<Packet> captured_packets_vect;  // Captured packets
    bool currently_capturing_packets;       // Flag set to TRUE when running packet capture
    bool break_out_of_capture;              // Flag to interrupt packet capture
    QString my_ip_address;                  // This device's IPv4 address
    QString constructed_filter_string;      // Filter string constructed in Filter Settings tab

    char *dev;                              // Network device string
    char errbuf[PCAP_ERRBUF_SIZE];          // Stores errors
    pcap_t *handle;                         // Session handle
    struct bpf_program filter_expression;   // The compiled filter expression
    bpf_u_int32 mask;                       // The netmask of network device
    bpf_u_int32 net;                        // The IP of device device
    struct pcap_pkthdr header;              // The packet header
    const u_char *packet;                   // The actual packet

};

#endif // MAINWINDOW_H
