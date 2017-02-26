#ifndef STATWINDOW_H
#define STATWINDOW_H

#include <QDialog>
#include "packet.h"

namespace Ui {
class StatWindow;
}

class StatWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StatWindow(QVector<Packet> vect, QWidget *parent = 0);
    ~StatWindow();

    void display_tcp_vs_udp(QVector<Packet> vect);
    void display_graph_temp();
private:
    Ui::StatWindow *ui;
};

#endif // STATWINDOW_H
