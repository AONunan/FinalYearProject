#include "statwindow.h"
#include "ui_statwindow.h"

StatWindow::StatWindow(QVector<Packet> vect, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatWindow)
{
    ui->setupUi(this);

    // Set class member variable to input vect
    input_vect = vect;

    chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing); // Turn on antialiasing to make chart lines appear nicer

    // Add the combobox and the chartview to the UI
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(ui->comboBox_display_choice);
    mainLayout->addWidget(chartView);

    if(input_vect.length() > 0) {
        display_tcp_vs_udp();
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
    //mainLayout->removeWidget();
    if(menu_value == "TCP vs UDP") {
        display_tcp_vs_udp();
    } else if(menu_value == "Chart 2") {
        display_graph_temp();
    } else {
        qDebug() << "Unknown choice";
    }
}

void StatWindow::display_tcp_vs_udp() {
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

    initial_time =  input_vect[0].getCurrent_time(); // First packet's timestamp
    final_time = input_vect[input_vect.length() - 1].getCurrent_time(); // Final packet's timestamp

    // Populate 2d_data_vect timestamp column
    for(i = initial_time; i <= final_time; i++) {
        QVector<int> row_vect; // Inner QVector
        row_vect.append(i); // Timestamp
        row_vect.append(0); // Count of packets at that timestamp
        two_dimensional_data_vect.append(row_vect);
    }

    // Populate 2d_data_vect count column
    for(i = 0; i < input_vect.length(); i++) {
        for(j = 0; j < two_dimensional_data_vect.length(); j++) {
            if(input_vect[i].getCurrent_time() == two_dimensional_data_vect[j][0]) { // Check if packet timestamp matches 2d_data_vect timestamp
                two_dimensional_data_vect[j][1]++; // If so, add 1 to the count
                break; // No need to check any more so break out of FOR loop
            }
        }
    }

    // Display results
    for(i = 0; i < two_dimensional_data_vect.length(); i++) {
        qDebug() << "col 1:" << Packet::timestamp_to_string(two_dimensional_data_vect[i][0]) << "- col 2:" << two_dimensional_data_vect[i][1];
    }

    // Store the line data
    QLineSeries *seriesTcp = new QLineSeries();

    // Add data to plotting info
    for(i = 0; i < two_dimensional_data_vect.length(); i++) {
        if(two_dimensional_data_vect[i][1] != 0) { // Exclude 0 values from graph
            seriesTcp->append(two_dimensional_data_vect[i][0], two_dimensional_data_vect[i][1]); // Add timestamp and count
        }
    }

    // Generate the chart
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(seriesTcp);
    chart->createDefaultAxes();
    chart->setTitle("TCP vs UDP");

    chartView->setChart(chart);
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
    chart->setTitle("Testing");

    chartView->setChart(chart);
}
