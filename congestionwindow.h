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
    void update_tahoe_points();
    void update_reno_points();
    void reset_variables();
private slots:
    void on_pushButton_send_clicked();
    void on_pushButton_ack_clicked();
    void on_pushButton_drop_clicked();

    void on_pushButton_clear_clicked();
    void on_pushButton_info_congestion_vs_flow_clicked();
    void on_pushButton_info_cwnd_clicked();
    void on_pushButton_info_t_vs_r_clicked();
    void on_pushButton_info_ssthresh_clicked();
    void on_pushButton_info_slow_start_clicked();
    void on_pushButton_info_cong_avoid_clicked();

private:
    Ui::CongestionWindow *ui;

    QVector<QVector<int>> tahoe_data;
    QVector<QVector<int>> reno_data;

    int x_axis;

    int tahoe_cwnd; // Congestion Window size
    int tahoe_mss; // Max Segment Size
    bool tahoe_slow_start; // Flag to decide if we are in slow start
    int tahoe_ssthresh; // Slow Start threshold

    int reno_cwnd; // Congestion Window size
    int reno_mss; // Max Segment Size
    bool reno_slow_start; // Flag to decide if we are in slow start
    int reno_ssthresh; // Slow Start threshold
};

#endif // CONGESTIONWINDOW_H
