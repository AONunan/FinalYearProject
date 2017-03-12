#include "congestionwindow.h"
#include "ui_congestionwindow.h"

CongestionWindow::CongestionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CongestionWindow)
{
    ui->setupUi(this);

    // Store the line data
    QLineSeries *series_tcp = new QLineSeries();

    series_tahoe = new QLineSeries();
    series_reno = new QLineSeries();

    // Test data
    series_tcp->append(0, 5);
    series_tcp->append(1, 7);
    series_tcp->append(2, 4);

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series_tcp);
    chart->createDefaultAxes();
    chart->setTitle("Testing");

    ui->chart_reno->setChart(chart);

    QChart *tahoe_points = new QChart();
    tahoe_points->legend()->hide();
    tahoe_points->addSeries(series_tahoe);
    tahoe_points->createDefaultAxes();
    ui->chart_tahoe->setChart(tahoe_points);


    temp = 0;

}

CongestionWindow::~CongestionWindow()
{
    delete ui;
}

void CongestionWindow::on_pushButton_send_clicked()
{
    QVector<int> row;
    row.append(1 * temp);
    row.append(2 * temp);
    temp++;
    tahoe_data.append(row);

    series_tahoe->append(tahoe_data[tahoe_data.length() - 1][0], tahoe_data[tahoe_data.length() - 1][1]);
}

void CongestionWindow::on_pushButton_ack_clicked()
{

}

void CongestionWindow::on_pushButton_drop_clicked()
{

}
