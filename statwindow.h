#ifndef STATWINDOW_H
#define STATWINDOW_H

#include <QGridLayout>
#include <QtCharts>
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

    void display_tcp_vs_udp();
    void display_graph_temp();
private slots:
    void on_comboBox_display_choice_currentIndexChanged(const QString &menu_value);

private:
    Ui::StatWindow *ui;
    QGridLayout *mainLayout;
    QChartView *chartView;
    QVector<Packet> input_vect;

};

#endif // STATWINDOW_H
