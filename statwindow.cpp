#include "statwindow.h"
#include "ui_statwindow.h"
#include <QtCharts>

StatWindow::StatWindow(QVector<Packet> vect, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatWindow)
{
    ui->setupUi(this);

    if(vect.length() > 0) {
        display_tcp_vs_udp(vect);
    } else {
        qDebug() << "Cannot display graph without first capturing packets.";
    }
}

StatWindow::~StatWindow()
{
    delete ui;
}

void StatWindow::display_tcp_vs_udp(QVector<Packet> vect) {
    int i, j, // Counters
        initial_time, final_time;

    QVector<QVector<int> > two_dimensional_data_vect;
    /* The two_dimensional_data_vect keeps track of how many packets were captured each second
     * and will look like the following:
     *
     * | Timestamp | Count |
     * | .......10 |    35 | <--- Inner QVector
     * | .......11 |    23 |
     * | .......12 |     0 |
     * | .......13 |    45 |
     *
     */
    qDebug() << "### TCP vs UDP ###";

    initial_time =  vect[0].getCurrent_time(); // First packet's timestamp
    final_time = vect[vect.length() - 1].getCurrent_time(); // Final packet's timestamp

    // Populate 2d_data_vect timestamp column
    for(i = initial_time; i <= final_time; i++) {
        QVector<int> row_vect; // Inner QVector
        row_vect.append(i); // Timestamp
        row_vect.append(0); // Count of packets at that timestamp
        two_dimensional_data_vect.append(row_vect);
    }

    // Populate 2d_data_vect count column
    for(i = 0; i < vect.length(); i++) {
        for(j = 0; j < two_dimensional_data_vect.length(); j++) {
            if(vect[i].getCurrent_time() == two_dimensional_data_vect[j][0]) { // Check if packet timestamp matches 2d_data_vect timestamp
                two_dimensional_data_vect[j][1]++; // If so, add 1 to the count
                break; // No need to check any more so break out of FOR loop
            }
        }
    }

    qDebug() << "2-D vector is length:" << two_dimensional_data_vect.length();

    // Display results
    for(i = 0; i < two_dimensional_data_vect.length(); i++) {
        qDebug() << "col 1:" << Packet::timestamp_to_string(two_dimensional_data_vect[i][0]) << "- col 2:" << two_dimensional_data_vect[i][1];
    }

    // Store the line data
    QLineSeries *seriesTcp = new QLineSeries();

    // Add data to plotting info
    for(i = 0; i < two_dimensional_data_vect.length(); i++) {
        seriesTcp->append(two_dimensional_data_vect[i][0], two_dimensional_data_vect[i][1]); // Add timestamp and count
    }

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(seriesTcp);
    chart->createDefaultAxes();
    chart->setTitle("TCP vs UDP");

    QChartView *chartView = new QChartView(chart, ui->widget_graph);
    chartView->setRenderHint(QPainter::Antialiasing);

}

void StatWindow::display_graph_temp() {

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
