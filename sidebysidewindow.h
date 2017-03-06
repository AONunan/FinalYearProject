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

    void populate_entries();
    void update_table();
    QString details_field(Packet packet);
    QString more_details_field(Packet packet);
private slots:
    void on_tableWidget_packets_itemSelectionChanged();
    void on_tableWidget_packets_cellDoubleClicked(int row);

private:
    Ui::SideBySideWindow *ui;

    struct row_entry {
        int index; // Number in sequence
        QString timestamp;
        bool local; // True if client (left hand side), False if server (RHS)
        QString syn_ack_details;
        QString syn_ack_more_details;
    };

    QVector<Packet> input_vect;
    int row_count; // Keep track of current row
    QString server_address;
    QVector<struct row_entry> all_row_entries; // Store all table entries

};

#endif // SIDEBYSIDEWINDOW_H
