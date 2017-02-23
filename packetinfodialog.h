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

private slots:
    void on_pushButton_print_payload_clicked();

private:
    Ui::PacketInfoDialog *ui;

    Packet displayed_packet; // Packet on screen
};

#endif // PACKETINFODIALOG_H
