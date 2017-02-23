#include "filtersettingswindow.h"
#include "ui_filtersettingswindow.h"

FilterSettingsWindow::FilterSettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterSettingsWindow)
{
    ui->setupUi(this);
}

FilterSettingsWindow::~FilterSettingsWindow()
{
    delete ui;
}
