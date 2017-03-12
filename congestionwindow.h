#ifndef CONGESTIONWINDOW_H
#define CONGESTIONWINDOW_H

#include <QDialog>
#include <QtCharts>
#include <QVector>

namespace Ui {
class CongestionWindow;
}

class CongestionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CongestionWindow(QWidget *parent = 0);
    ~CongestionWindow();

    void draw_plots();
private slots:
    void on_pushButton_send_clicked();
    void on_pushButton_ack_clicked();
    void on_pushButton_drop_clicked();

private:
    Ui::CongestionWindow *ui;

    QVector<QVector<int>> tahoe_data;
    QVector<QVector<int>> reno_data;

    int temp;
};

#endif // CONGESTIONWINDOW_H
