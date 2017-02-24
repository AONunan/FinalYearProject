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
    explicit PacketInfoDialog(const Packet packet, QWidget *parent = 0);
    ~PacketInfoDialog();

    void show_header_fields();
    void show_header_details();
private slots:
    void on_pushButton_print_payload_clicked();

    void on_pushButton_change_view_clicked();

private:
    Ui::PacketInfoDialog *ui;

    Packet displayed_packet; // Packet on screen
    bool currently_showing_field_names;
};

#endif // PACKETINFODIALOG_H
