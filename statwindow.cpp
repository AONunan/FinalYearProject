#include "statwindow.h"
#include "ui_statwindow.h"

#define TIMESTAMP_COLUMN 0
#define COUNT_COLUMN 1

StatWindow::StatWindow(QVector<Packet> vect, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatWindow)
{
    ui->setupUi(this);

    // Set class member variable to input vect
    input_vect = vect;

    chart_view = new QChartView();
    chart_view->setRenderHint(QPainter::Antialiasing); // Turn on antialiasing to make chart lines appear nicer

    // Add the combobox and the chartview to the UI
    main_layout = new QGridLayout(this);
    main_layout->addWidget(ui->comboBox_display_choice);
    main_layout->addWidget(chart_view);

    if(input_vect.length() > 0) {
        display_tcp_vs_udp_line();
    } else {
        qDebug() << "Cannot display graph without first capturing packets.";
    }
}

StatWindow::~StatWindow()
{
    delete ui;
}

void StatWindow::on_comboBox_display_choice_currentIndexChanged(const QString &menu_value)
{
    if(menu_value == "TCP vs UDP (Line Chart)") {
        display_tcp_vs_udp_line();
    } else if(menu_value == "TCP vs UDP (Bar Chart)") {
        display_tcp_vs_udp_bar();
    } else if(menu_value == "Chart 2") {
        display_graph_temp();
    } else {
        qDebug() << "Unknown choice";
    }
}

void StatWindow::display_tcp_vs_udp_line() {
    int i, j, // Counters
        initial_time, final_time;

    QVector<QVector<int> > tcp_count_vect, udp_count_vect;
    /* The tcp_count_vect keeps track of how many packets were captured each second
     * and will look like the following:
     *
     * | Timestamp | Count |
     * | .......10 |    35 | <--- Inner QVector
     * | .......11 |    23 |
     * | .......12 |     0 |
     * | .......13 |    45 |
     *
     */

    initial_time =  input_vect[0].getCurrent_time(); // First packet's timestamp
    final_time = input_vect[input_vect.length() - 1].getCurrent_time(); // Final packet's timestamp

    // Populate 2d_data_vect timestamp column
    for(i = initial_time; i <= final_time; i++) {
        QVector<int> row_vect; // Inner QVector
        row_vect.append(i); // Timestamp
        row_vect.append(0); // Count of packets at that timestamp
        tcp_count_vect.append(row_vect);
        udp_count_vect.append(row_vect);
    }

    // Populate 2d_data_vect count column
    for(i = 0; i < input_vect.length(); i++) {
        if(input_vect[i].getProtocol() == "TCP") {
            for(j = 0; j < tcp_count_vect.length(); j++) {
                if(input_vect[i].getCurrent_time() == tcp_count_vect[j][TIMESTAMP_COLUMN]) { // Check if packet timestamp matches 2d_data_vect timestamp
                    tcp_count_vect[j][COUNT_COLUMN]++; // If so, add 1 to the count
                    break; // No need to check any more so break out of FOR loop
                }
            }
        } else if(input_vect[i].getProtocol() == "UDP") {
            for(j = 0; j < udp_count_vect.length(); j++) {
                if(input_vect[i].getCurrent_time() == tcp_count_vect[j][TIMESTAMP_COLUMN]) { // Check if packet timestamp matches 2d_data_vect timestamp
                    udp_count_vect[j][COUNT_COLUMN]++; // If so, add 1 to the count
                    break; // No need to check any more so break out of FOR loop
                }
            }
        }
    }

    // Store the line data
    QLineSeries *series_tcp = new QLineSeries();
    QLineSeries *series_udp = new QLineSeries();

    // Add origin points
    series_tcp->append(tcp_count_vect[0][TIMESTAMP_COLUMN], 0);
    series_tcp->append(udp_count_vect[0][TIMESTAMP_COLUMN], 0);

    // Add data to plotting info
    for(i = 0; i < tcp_count_vect.length(); i++) {
        if(tcp_count_vect[i][COUNT_COLUMN] != 0) { // Exclude 0 values from graph
            series_tcp->append(tcp_count_vect[i][TIMESTAMP_COLUMN], tcp_count_vect[i][COUNT_COLUMN]); // Add timestamp and count
        }

        if(udp_count_vect[i][COUNT_COLUMN] != 0) { // Exclude 0 values from graph
            series_udp->append(udp_count_vect[i][TIMESTAMP_COLUMN], udp_count_vect[i][COUNT_COLUMN]); // Add timestamp and count
        }
    }

    // Generate the chart
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series_tcp);
    chart->addSeries(series_udp);
    chart->createDefaultAxes();
    chart->setTitle("TCP vs UDP");

    // Set the chart_view widget to the current chart details
    chart_view->setChart(chart);
}

void StatWindow::display_tcp_vs_udp_bar() {
    int i, tcp_count, udp_count;

    tcp_count = 0;
    udp_count = 0;

    for(i = 0; i < input_vect.length(); i++) {
        if(input_vect[i].getProtocol() == "TCP") {
            tcp_count++;
        } else if(input_vect[i].getProtocol() == "UDP") {
            udp_count++;
        }
    }

    QBarSet *tcp_set = new QBarSet("TCP");
    QBarSet *udp_set = new QBarSet("UDP");

    *tcp_set << tcp_count;
    *udp_set << udp_count;

    QBarSeries *series = new QBarSeries();
    series->append(tcp_set);
    series->append(udp_set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("TCP vs UDP");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // Set the chart_view widget to the current chart details
    chart_view->setChart(chart);

    qDebug() << "TCP count:" << tcp_count;
    qDebug() << "UDP count:" << udp_count;
}

void StatWindow::display_graph_temp() {
    // Store the line data
    QLineSeries *series_tcp = new QLineSeries();
    QLineSeries *series_udp = new QLineSeries();

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

    chart_view->setChart(chart);
}
