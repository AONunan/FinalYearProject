#ifndef PACKETINFODIALOG_H
#define PACKETINFODIALOG_H

#include <QDialog>
#include "packet.h"

namespace Ui {
class PacketInfoDialog;
}

class PacketInfoDialog : public QDialog {
    Q_OBJECT

public:
    explicit PacketInfoDialog(QVector<Packet> *input_vect_ptr, const int input_packet_index, QWidget *parent = 0);
    ~PacketInfoDialog();

    void show_header_field_names();
    void show_header_details();
    QString find_tcp_flag_string(int flags);
    void print_payload();
    void print_payload_offset();
    void print_payload_hex_ascii();
    void set_up_display();
private slots:
    void on_pushButton_change_view_clicked();

    void on_pushButton_next_packet_clicked();

    void on_pushButton_previous_packet_clicked();

private:
    Ui::PacketInfoDialog *ui;

    Packet displayed_packet; // Packet on screen
    QVector<Packet> *vect_ptr; // Pointer to beginning of QVector
    int current_index; // Index of required packet, obtained from input offset/index
    int vect_length;
    bool currently_showing_field_names;
};

#endif // PACKETINFODIALOG_H
