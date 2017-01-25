#include "packetcapturewindow.h"
#include "ui_packetcapturewindow.h"

PacketCaptureWindow::PacketCaptureWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketCaptureWindow)
{
    ui->setupUi(this);
}

PacketCaptureWindow::~PacketCaptureWindow()
{
    delete ui;
}
