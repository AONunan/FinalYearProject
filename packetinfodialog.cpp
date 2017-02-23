#include "packetinfodialog.h"
#include "ui_packetinfodialog.h"

PacketInfoDialog::PacketInfoDialog(const Packet packet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketInfoDialog) {
    ui->setupUi(this);
    ui->label->setText(packet.getProtocol());
}

PacketInfoDialog::~PacketInfoDialog() {
    delete ui;
}
