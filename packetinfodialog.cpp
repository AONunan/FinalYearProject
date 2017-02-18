#include "packetinfodialog.h"
#include "ui_packetinfodialog.h"

PacketInfoDialog::PacketInfoDialog(const QString my_var, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketInfoDialog) {
    ui->setupUi(this);
    ui->label->setText(my_var);
}

PacketInfoDialog::~PacketInfoDialog() {
    delete ui;
}
