#include "congestionwindow.h"
#include "ui_congestionwindow.h"

CongestionWindow::CongestionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CongestionWindow)
{
    ui->setupUi(this);
}

CongestionWindow::~CongestionWindow()
{
    delete ui;
}
