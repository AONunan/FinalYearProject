#ifndef SIDEBYSIDEWINDOW_H
#define SIDEBYSIDEWINDOW_H

#include <QDialog>
#include "packet.h"

namespace Ui {
class SideBySideWindow;
}

class SideBySideWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SideBySideWindow(QVector<Packet> vect, const QString input_server_address, QWidget *parent = 0);
    ~SideBySideWindow();

    void populate_table();
    void update_table(Packet packet, int column_position);
    QString find_details_field(Packet packet);
private slots:
    void on_tableWidget_packets_cellDoubleClicked(int row);

private:
    Ui::SideBySideWindow *ui;
    QVector<Packet> input_vect, matching_packets_vect;
    int row_count; // Keep track of current row
    u_int syn_ack_ack_number; // SYN-ACK packet's acknowledgement number
    QString server_address;
};

#endif // SIDEBYSIDEWINDOW_H
