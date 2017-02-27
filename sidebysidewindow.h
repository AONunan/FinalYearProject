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
    explicit SideBySideWindow(QVector<Packet> vect, QWidget *parent = 0);
    ~SideBySideWindow();

    void populate_table();
    void update_table(int timestamp, int column_position);
private:
    Ui::SideBySideWindow *ui;
    QVector<Packet> input_vect;
    int row_count; // Keep track of current row
};

#endif // SIDEBYSIDEWINDOW_H
