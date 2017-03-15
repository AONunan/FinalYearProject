#include "congestionwindow.h"
#include "ui_congestionwindow.h"
#include <QMessageBox>
#include <math.h> // For floor()

CongestionWindow::CongestionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CongestionWindow) {
    ui->setupUi(this);

    reset_variables();
}

/*
 * Reset all variables backt to default values and reset the UI
 * Called when window is opened and on pressing Clear
 */
void CongestionWindow::reset_variables() {
    ui->pushButton_send->setEnabled(true);
    ui->pushButton_clear->setEnabled(false);
    ui->pushButton_ack->setEnabled(false);
    ui->pushButton_drop->setEnabled(false);
    ui->textBrowser_tahoe_variables->setText("");
    ui->textBrowser_tahoe_action->setText("");
    ui->textBrowser_reno_variables->setText("");
    ui->textBrowser_reno_action->setText("");

    tahoe_data.clear();
    reno_data.clear();

    x_axis = 1;

    tahoe_cwnd = 1;
    tahoe_slow_start = true; // Begin in Slow Start state
    tahoe_ssthresh = 12;

    reno_cwnd = 1;
    reno_slow_start = true; // Begin in Slow Start state
    reno_ssthresh = 12;
}

/*
 * Draw the Tahoe and Reno plots with the most up to date plot data
 * Also updates the variables window
 */
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

    // Set Tahoe variables window
    ui->textBrowser_tahoe_variables->setText(construct_vars_window(tahoe_cwnd, tahoe_ssthresh, tahoe_slow_start));

    // Set Reno variables window
    ui->textBrowser_reno_variables->setText(construct_vars_window(reno_cwnd, reno_ssthresh, reno_slow_start));
}

/*
 * Construct var window string for Tahoe and Reno
 */
QString CongestionWindow::construct_vars_window(int cwnd, int ssthresh, bool slow_start) {
    return QString("Congestion Window = %1\n"
                   "Slow Start threshold = %2\n"
                   "Strategy = %3").arg(cwnd).arg(ssthresh).arg((slow_start) ? "Slow Start" : "Congestion Avoidance");
}

CongestionWindow::~CongestionWindow() {
    delete ui;
}

/*
 * On pressing the Send button, do the following:
 *     Update the action window and modify the variables
 *     Update the plot points
 *     Disable Send button, enable ACK and Drop button
 */
void CongestionWindow::on_pushButton_send_clicked() {
    // Update Tahoe action window
    ui->textBrowser_tahoe_action->setText(send_data(tahoe_cwnd, tahoe_ssthresh, tahoe_slow_start));

    // Update Reno action window
    ui->textBrowser_reno_action->setText(send_data(reno_cwnd, reno_ssthresh, reno_slow_start));

    update_data_points();

    ui->pushButton_send->setEnabled(false);
    ui->pushButton_ack->setEnabled(true);
    ui->pushButton_drop->setEnabled(true);

    draw_plots();
    x_axis++;
}

/*
 * Construct string description of sent text.
 */
QString CongestionWindow::send_data(int cwnd, int ssthresh, bool slow_start) {
    QString explanation;

    if(slow_start) {
        explanation = QString("We are in Slow Start mode, as the Congestion Window is less than/equal to the Slow Start Threshold (%1 <= %2). We see an exponential increase in the Congestion Window.\n\n").arg(cwnd).arg(ssthresh);

    } else {
        explanation = QString("We are in Congestion Avoidance mode, as the Congestion Window is greater than the Slow Start Threshold (%1 > %2). We see a linear increase in the Congestion Window.\n\n").arg(cwnd).arg(ssthresh);
    }

    explanation += QString("We send %1 segments.").arg(cwnd);

    return explanation;
}

/*
 * Add the most recent data to the plot data
 */
void CongestionWindow::update_data_points() {
    QVector<int> row_tahoe;
    row_tahoe.append(x_axis); // x-axis
    row_tahoe.append(tahoe_cwnd); // y-axis
    tahoe_data.append(row_tahoe);

    QVector<int> row_reno;
    row_reno.append(x_axis); // x-axis
    row_reno.append(reno_cwnd); // y-axis
    reno_data.append(row_reno);
}

/*
 * On pressing ACK, update the description window and update the cwdn and decide if still in slow start
 * Enable/disable buttons
 */
void CongestionWindow::on_pushButton_ack_clicked() {
    ui->textBrowser_tahoe_action->setText(ack_data(&tahoe_cwnd, tahoe_ssthresh, &tahoe_slow_start));
    ui->textBrowser_reno_action->setText(ack_data(&reno_cwnd, reno_ssthresh, &reno_slow_start));

    ui->pushButton_send->setEnabled(true);
    ui->pushButton_ack->setEnabled(false);
    ui->pushButton_drop->setEnabled(false);
}

/*
 * Construct string used when ACKing data
 * Modify cwnd and slow_start bool. Pass by reference to update these values
 */
QString CongestionWindow::ack_data(int *cwndPtr, int ssthresh, bool *slow_startPtr) {
    QString explanation;

    explanation = "Successful acknowledgement of data.\n\n";

    if(*slow_startPtr) {
        explanation += QString("We are in Slow Start mode.\n"
                                     "The Congestion Window is doubled (%1 to %2).\n\n").arg(*cwndPtr).arg(*cwndPtr * 2);

        *cwndPtr *= 2; // Double the cwnd

        if(*cwndPtr > ssthresh) {
            explanation += QString("The Congestion Window is now greater than the Slow start Threshold (%1 > %2), therefore we switch to Congestion Avoidance.").arg(*cwndPtr).arg(ssthresh);
            *slow_startPtr = false; // Switch to congestion avoidance
        } else {
            explanation += QString("The congestion window is still less than/equal to than the slow start threshold (%1 <= %2), therefore we remain in Slow Start.").arg(*cwndPtr).arg(ssthresh);
        }

    } else { // Congestion avoidance
        explanation += QString("We are in Congestion Avoidance mode.\n"
                                     "The Congestion Window is increased by 1 (%1 to %2)").arg(*cwndPtr).arg(*cwndPtr + 1);
        *cwndPtr += 1; // Increment
    }

    return explanation;
}

/*
 * On dropping a packet, different action depending if Tahoe or Reno
 */
void CongestionWindow::on_pushButton_drop_clicked() {
    /******************** Tahoe ********************/
    QString tahoe_explanation;

    tahoe_explanation = QString("A packet has been dropped.\n\n"
                        "Set the new Slow Start threshold to half the current Congestion Window (%1 / 2 = %2).\n"
                        "Reset the congestion window back to 1.\n"
                        "Return to Slow Start mode (fast retransmit).").arg(tahoe_cwnd).arg(tahoe_cwnd / 2);

    ui->textBrowser_tahoe_action->setText(tahoe_explanation);

    tahoe_ssthresh = tahoe_cwnd / 2; // Divide by 2
    tahoe_cwnd = 1;
    tahoe_slow_start = true;


    /******************** Reno ********************/
    QString reno_explanation;

    reno_explanation = QString("A packet has been dropped.\n\n"
                               "Set the new Slow Start threshold AND the new Congestion Window to half the current Congestion Window (%1 / 2 = %2).\n"
                               "Remain in Congestion Avoidance mode (fast recovery).").arg(reno_cwnd).arg(reno_cwnd / 2);

    ui->textBrowser_reno_action->setText(reno_explanation);

    reno_ssthresh = reno_cwnd / 2; // Divide by 2
    reno_cwnd = reno_ssthresh;

    ui->pushButton_send->setEnabled(true);
    ui->pushButton_ack->setEnabled(false);
    ui->pushButton_drop->setEnabled(false);
}

/*
 * On pressing Clear, reset everything
 */
void CongestionWindow::on_pushButton_clear_clicked() {
    // Reset all variables
    reset_variables();

    draw_plots(); // Redraw empty plots
}

/*
 * From this point down, all functions deal with info pop-ups
 */

void CongestionWindow::on_pushButton_info_congestion_vs_flow_clicked() {
    QMessageBox::information(this, ui->pushButton_info_congestion_vs_flow->text(), // Get title from text on button
                             "Flow control is set by the window size field in the receiver's TCP header. This limits how much data the receiver is willing to receive at any given time.\n\n"
                             "But what if the receive window is too large? For example, if the receive window allows for 50 packets but the physical limitations of the network can only carry across 20 packets before a timeout occurs. The sender sends all 50 packets but 30 are lost due to congestion on the router for example. These missing packets will need to be resent. This can lead to a huge amount of resending packets which slows down the network.\n\n"
                             "Congestion control limits the flow of packets on the sender's side by testing the limits to see how much the network is able to handle.");
}

void CongestionWindow::on_pushButton_info_cwnd_clicked() {
    QMessageBox::information(this, ui->pushButton_info_cwnd->text(),
                             "The congestion window is dynamically adjusted based on how much data the network is able to transfer across before packet loss occurs. This, of course, changes over time and TCP uses congestion control strategies (TCP Tahoe and TCP Reno discussed here) to deal with this problem.");
}

void CongestionWindow::on_pushButton_info_t_vs_r_clicked() {
    QMessageBox::information(this, ui->pushButton_info_t_vs_r->text(),
                             "TCP Tahoe is a congestion control mechanism that was introduced in 1987. It implements a slow start strategy at the beginning of a connection or at packet loss. Once the congestion window is greater than the slow start threshold (ssthresh), it switches to congestion avoidance.\n"
                             "On packet loss, the congestion window is reset and the ssthresh is set to half the previous congestion window. It re-enters the slow start state.\n\n"
                             "TCP Reno was introduced in 1990. It too uses slow start at the beginning of a connection, and congestion avoidance once the congestion window is greater than ssthresh.\n"
                             "However, unlike Tahoe, Reno does not re-enter the slow start state. It sets both the ssthresh AND the congestion window to half the previous congestion window and resumes congestion avoidance. This is known as fast retransmit and recovery.");
}

void CongestionWindow::on_pushButton_info_ssthresh_clicked() {
    QMessageBox::information(this, ui->pushButton_info_ssthresh->text(),
                             "This value is used to determine whether or not TCP should use slow start or congestion avoidance.\n\n"
                             "It is initially set to a large number and adjusted as required.\n\n"
                             "If congestion window <= ssthresh:\n"
                             "    Use slow start\n"
                             "If congestion window > ssthresh:\n"
                             "    Use congestion avoidance");
}

void CongestionWindow::on_pushButton_info_slow_start_clicked() {
    QMessageBox::information(this, ui->pushButton_info_slow_start->text(),
                             "Slow start is a congestion control strategy. It begins by setting the congestion window to a low number (1 or 2 for example). As ACKs are received, the congestion window is doubled for each round-trip, thus leading to an exponential-growth.\n\n"
                             "This will continue until either:\n"
                             "(i)   a packet is dropped\n"
                             "(ii)  the receivers window size is reached (flow control)\n"
                             "(iii) the slow start threshold is reached\n\n"
                             "If the ssthresh is reached, the congestion control strategy switches to congestion avoidance.");
}

void CongestionWindow::on_pushButton_info_cong_avoid_clicked() {
    QMessageBox::information(this, ui->pushButton_info_cong_avoid->text(),
                             "Congestion avoidance takes a more conservative approach. When the congestion window is above the ssthresh, it means that the network limit could be close.\n\n"
                             "Congestion avoidance increases the congestion window size linearly (rather than exponentially like slow start).");
}
