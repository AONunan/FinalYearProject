#include "sidebysidewindow.h"
#include "ui_sidebysidewindow.h"

SideBySideWindow::SideBySideWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SideBySideWindow)
{
    ui->setupUi(this);
}

SideBySideWindow::~SideBySideWindow()
{
    delete ui;
}
