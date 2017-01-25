#ifndef PACKETCAPTUREWINDOW_H
#define PACKETCAPTUREWINDOW_H

#include <QDialog>

namespace Ui {
class PacketCaptureWindow;
}

class PacketCaptureWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PacketCaptureWindow(QWidget *parent = 0);
    ~PacketCaptureWindow();

private:
    Ui::PacketCaptureWindow *ui;
};

#endif // PACKETCAPTUREWINDOW_H
