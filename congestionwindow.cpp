#include "congestionwindow.h"
#include "ui_congestionwindow.h"

CongestionWindow::CongestionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CongestionWindow)
{
    ui->setupUi(this);

    temp = 0;

}

void CongestionWindow::draw_plots() {
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
    QVector<int> row_tahoe;
    row_tahoe.append(temp);
    row_tahoe.append(pow(temp, 2));
    tahoe_data.append(row_tahoe);

    QVector<int> row_reno;
    row_reno.append(temp);
    row_reno.append(pow(temp, 3));
    reno_data.append(row_reno);

    temp++;

    draw_plots();
}

void CongestionWindow::on_pushButton_ack_clicked()
{

}

void CongestionWindow::on_pushButton_drop_clicked()
{

}
