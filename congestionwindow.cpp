#include "congestionwindow.h"
#include "ui_congestionwindow.h"

CongestionWindow::CongestionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CongestionWindow)
{
    ui->setupUi(this);

    // Store the line data
    QLineSeries *series_tcp = new QLineSeries();
    QLineSeries *series_udp = new QLineSeries();

    // Test data
    series_tcp->append(0, 5);
    series_tcp->append(1, 7);
    series_tcp->append(2, 4);

    series_udp->append(0, 9);
    series_udp->append(1, -5);
    series_udp->append(2, 5);

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series_tcp);
    chart->addSeries(series_udp);
    chart->createDefaultAxes();
    chart->setTitle("Testing");

    ui->chart_tahoe->setChart(chart);

}

CongestionWindow::~CongestionWindow()
{
    delete ui;
}
