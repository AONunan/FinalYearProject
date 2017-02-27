#include "sidebysidewindow.h"
#include "ui_sidebysidewindow.h"

#include <QDebug>
#include "packetinfodialog.h"

#define COLUMN_TIMESTAMP 0
#define COLUMN_CLIENT 1
#define COLUMN_SERVER 2

SideBySideWindow::SideBySideWindow(QVector<Packet> vect, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SideBySideWindow)
{
    ui->setupUi(this);

    // Set class member variable to input vect
    input_vect = vect;

    row_count = 0;

    populate_table();
}

SideBySideWindow::~SideBySideWindow()
{
    delete ui;
}

void SideBySideWindow::populate_table() {
    int i;

    QString server_address = "178.62.36.190";

    for(i = 0; i < input_vect.length(); i++) {
        //qDebug() << "*** source addr:" << input_vect[i].getIp_source_address() << "*** dest addr:" << input_vect[i].getIp_destination_address();
        if(input_vect[i].getIp_destination_address() == server_address) { // Client is sending to server
            update_table(input_vect[i], COLUMN_CLIENT);
        } else if(input_vect[i].getIp_source_address() == server_address) { // Server is sending to client
            update_table(input_vect[i], COLUMN_SERVER);
        }
    }
}

void SideBySideWindow::update_table(Packet packet, int column_position) {
    QString timestamp_string = Packet::timestamp_to_string(packet.getCurrent_time()); // Convert to string
    matching_packets_vect.append(packet);

    // Create new row and scroll to bottom of table
    ui->tableWidget_packets->setRowCount(row_count + 1);
    ui->tableWidget_packets->scrollToBottom();

    ui->tableWidget_packets->setItem(row_count, COLUMN_TIMESTAMP, new QTableWidgetItem(timestamp_string));
    ui->tableWidget_packets->setItem(row_count, column_position, new QTableWidgetItem("Test"));

    row_count++;
}

void SideBySideWindow::on_tableWidget_packets_cellDoubleClicked(int row) {
    // Open dialog with packet details with an argument
    PacketInfoDialog infoDialog(matching_packets_vect[row]);
    infoDialog.setModal(true);
    infoDialog.exec();
}
