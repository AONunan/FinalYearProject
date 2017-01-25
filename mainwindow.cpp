#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "packetcapturewindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_packet_tracer_clicked()
{
    PacketCaptureWindow packetTracerWin;

    // set modal to true
    // note: modal means that once the second window opens,
    // you can't access the parent window until the child
    // window is closed
    packetTracerWin.setModal(true);

    // execute
    packetTracerWin.exec();
}
