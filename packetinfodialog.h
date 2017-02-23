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

private:
    Ui::PacketInfoDialog *ui;
};

#endif // PACKETINFODIALOG_H
