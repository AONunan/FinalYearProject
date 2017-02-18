#ifndef PACKETINFODIALOG_H
#define PACKETINFODIALOG_H

#include <QDialog>

namespace Ui {
class PacketInfoDialog;
}

class PacketInfoDialog : public QDialog {
    Q_OBJECT

public:
    explicit PacketInfoDialog(const QString my_var, QWidget *parent = 0);
    ~PacketInfoDialog();

private:
    Ui::PacketInfoDialog *ui;
};

#endif // PACKETINFODIALOG_H
