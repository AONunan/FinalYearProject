#include "statwindow.h"
#include "ui_statwindow.h"

StatWindow::StatWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatWindow)
{
    ui->setupUi(this);
}

StatWindow::~StatWindow()
{
    delete ui;
}
