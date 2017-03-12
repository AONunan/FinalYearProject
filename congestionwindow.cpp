#include "congestionwindow.h"
#include "ui_congestionwindow.h"

CongestionWindow::CongestionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CongestionWindow)
{
    ui->setupUi(this);

    reset_variables();
}

void CongestionWindow::reset_variables() {
    tahoe_data.clear();
    reno_data.clear();

    // Add origin point (0, 0) to data
    QVector<int> origin_point;
    origin_point.append(0); // x-axis
    origin_point.append(0); // y-aix

    tahoe_data.append(origin_point);
    reno_data.append(origin_point);

    x_axis = 1;
    tahoe_mss = 1;
}

void CongestionWindow::draw_plots() {
    ui->pushButton_clear->setEnabled(true); // Enable the Clear button

    int i;

    QLineSeries *series_tahoe = new QLineSeries();
    QLineSeries *series_reno = new QLineSeries();

    // Add all data to line series
    for(i = 0; i < tahoe_data.length(); i++) {
        series_tahoe->append(tahoe_data[i][0], tahoe_data[i][1]);
        series_reno->append(reno_data[i][0], reno_data[i][1]);
    }

    QChart *tahoe_chart = new QChart();
    tahoe_chart->legend()->hide();
    tahoe_chart->addSeries(series_tahoe);
    tahoe_chart->createDefaultAxes();
    ui->chart_widget_tahoe->setChart(tahoe_chart);

    QChart *reno_chart = new QChart();
    reno_chart->legend()->hide();
    reno_chart->addSeries(series_reno);
    reno_chart->createDefaultAxes();
    ui->chart_widget_reno->setChart(reno_chart);
}

CongestionWindow::~CongestionWindow()
{
    delete ui;
}

void CongestionWindow::on_pushButton_send_clicked()
{
    update_tahoe_points();
    update_reno_points();

    draw_plots();
    x_axis++;
}

void CongestionWindow::update_tahoe_points() {
    QVector<int> row_tahoe;
    row_tahoe.append(x_axis); // x-axis
    row_tahoe.append(tahoe_mss); // y-axis
    tahoe_data.append(row_tahoe);

    tahoe_mss *= 2; // Double the MSS
}

void CongestionWindow::update_reno_points() {
    QVector<int> row_reno;
    row_reno.append(x_axis); // x-axis
    row_reno.append(pow(x_axis, 3)); // y-axis
    reno_data.append(row_reno);
}

void CongestionWindow::on_pushButton_ack_clicked()
{

}

void CongestionWindow::on_pushButton_drop_clicked()
{

}

void CongestionWindow::on_pushButton_clear_clicked()
{
    ui->pushButton_clear->setEnabled(false); // Disable the Clear button
    qDebug() << "Cleared the clear button";

    // Reset all variables
    reset_variables();

    draw_plots(); // Redraw empty plots
}
