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

void PacketCaptureWindow::on_button_applyFilter_clicked()
{

}

void PacketCaptureWindow::on_button_close_handle_clicked()
{

}

void PacketCaptureWindow::on_button_capture_packet_clicked()
{

}

void PacketCaptureWindow::on_pushButton_test_clicked()
{

}

void PacketCaptureWindow::on_button_capture_stream_clicked()
{

}
