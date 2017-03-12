#ifndef CONGESTIONWINDOW_H
#define CONGESTIONWINDOW_H

#include <QDialog>
#include <QtCharts>

namespace Ui {
class CongestionWindow;
}

class CongestionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CongestionWindow(QWidget *parent = 0);
    ~CongestionWindow();

private:
    Ui::CongestionWindow *ui;

//    QGridLayout *main_layout;
//    QChartView *chart_view;
};

#endif // CONGESTIONWINDOW_H
