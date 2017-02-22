#include "statwindow.h"
#include "ui_statwindow.h"
#include <QtCharts>

StatWindow::StatWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatWindow)
{
    ui->setupUi(this);

    display_tcp_vs_udp();
}

StatWindow::~StatWindow()
{
    delete ui;
}

void StatWindow::display_tcp_vs_udp() {
    // Store the line data
    QLineSeries *seriesTcp = new QLineSeries();
    QLineSeries *seriesUdp = new QLineSeries();

    seriesTcp->append(0, 5);
    seriesTcp->append(1, 7);
    seriesTcp->append(2, 4);

    seriesUdp->append(0, 9);
    seriesUdp->append(1, -5);
    seriesUdp->append(2, 5);

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(seriesTcp);
    chart->addSeries(seriesUdp);
    chart->createDefaultAxes();
    chart->setTitle("TCP vs UDP");

    QChartView *chartView = new QChartView(chart, ui->widget_graph);
    chartView->setRenderHint(QPainter::Antialiasing);


}
