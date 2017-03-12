#include "congestionwindow.h"
#include "ui_congestionwindow.h"
#include <QMessageBox>
#include <math.h> // For floor()

CongestionWindow::CongestionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CongestionWindow)
{
    ui->setupUi(this);

    reset_variables();
}

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

    // Add origin point (0, 0) to data
    QVector<int> origin_point;
    origin_point.append(0); // x-axis
    origin_point.append(0); // y-aix

    tahoe_data.append(origin_point);
    reno_data.append(origin_point);

    x_axis = 1;

    tahoe_mss = 1;
    tahoe_cwnd = tahoe_mss;
    tahoe_slow_start = true; // Begin in Slow Start state
    tahoe_ssthresh = 20;

    reno_mss = 1;
    reno_cwnd = tahoe_mss;
    reno_slow_start = true; // Begin in Slow Start state
    reno_ssthresh = 20;
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

    /******************** Tahoe ********************/
    QString tahoe_var_details = QString("Congestion Window = %1\n"
                                        "Max Segment Size = %2\n"
                                        "Slow Start threshold = %3\n"
                                        "Strategy = %4").arg(tahoe_cwnd).arg(tahoe_mss).arg(tahoe_ssthresh).arg((tahoe_slow_start) ? "Slow Start" : "Congestion Avoidance");
    ui->textBrowser_tahoe_variables->setText(tahoe_var_details);

    /******************** Reno ********************/
    QString reno_var_details = QString("Congestion Window = %1\n"
                                        "Max Segment Size = %2\n"
                                        "Slow Start threshold = %3\n"
                                        "Strategy = %4").arg(reno_cwnd).arg(reno_mss).arg(reno_ssthresh).arg((reno_slow_start) ? "Slow Start" : "Congestion Avoidance");
    ui->textBrowser_reno_variables->setText(reno_var_details);
}

CongestionWindow::~CongestionWindow()
{
    delete ui;
}

void CongestionWindow::on_pushButton_send_clicked()
{
    /******************** Tahoe ********************/
    QString tahoe_explanation;

    if(tahoe_slow_start) {
        tahoe_explanation = QString("We are in Slow Start mode, as the Congestion Window is less than/equal to the Slow Start Threshold (%1 <= %2). We see an exponential increase in the Congestion Window.\n\n").arg(tahoe_cwnd).arg(tahoe_ssthresh);

    } else {
        tahoe_explanation = QString("We are in Congestion Avoidance mode, as the Congestion Window is greater than the Slow Start Threshold (%1 > %2). We see a linear increase in the Congestion Window.\n\n").arg(tahoe_cwnd).arg(tahoe_ssthresh);
    }

    tahoe_explanation += QString("We send %1 data.").arg(tahoe_cwnd);

    ui->textBrowser_tahoe_action->setText(tahoe_explanation);


    /******************** Reno ********************/
    QString reno_explanation;

    if(reno_slow_start) {
        reno_explanation = QString("We are in Slow Start mode, as the Congestion Window is less than/equal to the Slow Start Threshold (%1 <= %2). We see an exponential increase in the Congestion Window.\n\n").arg(reno_cwnd).arg(reno_ssthresh);

    } else {
        reno_explanation = QString("We are in Congestion Avoidance mode, as the Congestion Window is greater than the Slow Start Threshold (%1 > %2). We see a linear increase in the Congestion Window.\n\n").arg(reno_cwnd).arg(reno_ssthresh);
    }

    reno_explanation += QString("We send %1 data.").arg(reno_cwnd);

    ui->textBrowser_reno_action->setText(reno_explanation);

    update_tahoe_points();
    update_reno_points();

    ui->pushButton_send->setEnabled(false);
    ui->pushButton_ack->setEnabled(true);
    ui->pushButton_drop->setEnabled(true);

    draw_plots();
    x_axis++;
}

void CongestionWindow::update_tahoe_points() {
    QVector<int> row_tahoe;
    row_tahoe.append(x_axis); // x-axis
    row_tahoe.append(tahoe_cwnd); // y-axis
    tahoe_data.append(row_tahoe);


}

void CongestionWindow::update_reno_points() {
    QVector<int> row_reno;
    row_reno.append(x_axis); // x-axis
    row_reno.append(reno_cwnd); // y-axis
    reno_data.append(row_reno);
}

void CongestionWindow::on_pushButton_ack_clicked()
{
    QString tahoe_explanation;
    QString reno_explanation;

    /******************** Tahoe ********************/
    tahoe_explanation = "Successful acknowledgement of data.\n\n";
    reno_explanation = tahoe_explanation;

    if(tahoe_slow_start) {
        tahoe_explanation += QString("We are in Slow Start mode.\n"
                                     "The Congestion Window is increased by the value of the Max Segment Size (%1 + %2 = %3).\n"
                                     "The Max Segment Size doubles (%2 to %4).\n\n").arg(tahoe_cwnd).arg(tahoe_mss).arg(tahoe_cwnd + tahoe_mss).arg(2 * tahoe_mss);

        tahoe_cwnd += tahoe_mss;
        tahoe_mss *= 2; // Double the MSS

        if(tahoe_cwnd > tahoe_ssthresh) {
            tahoe_explanation += QString("The Congestion Window is now greater than the Slow start Threshold (%1 > %2), therefore we switch to Congestion Avoidance.").arg(tahoe_cwnd).arg(tahoe_ssthresh);
            tahoe_slow_start = false; // Switch to congestion avoidance
        } else {
            tahoe_explanation += QString("The congestion window is still less than/equal to than the slow start threshold (%1 <= %2), therefore we remain in Slow Start.").arg(tahoe_cwnd).arg(tahoe_ssthresh);
        }

    } else { // Congestion avoidance
        tahoe_explanation += QString("We are in Congestion Avoidance mode.\n"
                                     "Congestion Window += (MSS^2) / cwnd\n"
                                     "Congestion Window += %1^2 / %2\n"
                                     "Congestion Window = %3").arg(tahoe_mss).arg(tahoe_cwnd).arg(floor(tahoe_cwnd + (pow(tahoe_mss, 2) / tahoe_cwnd)));
        tahoe_cwnd += pow(tahoe_mss, 2) / tahoe_cwnd; // cwnd += mss^2 / cwnd
    }

    ui->textBrowser_tahoe_action->setText(tahoe_explanation);


    /******************** Reno ********************/
    if(reno_slow_start) {
        reno_explanation += QString("We are in Slow Start mode.\n"
                                     "The Congestion Window is increased by the value of the Max Segment Size (%1 + %2 = %3).\n"
                                     "The Max Segment Size doubles (%2 to %4).\n\n").arg(reno_cwnd).arg(reno_mss).arg(reno_cwnd + reno_mss).arg(2 * reno_mss);

        reno_cwnd += reno_mss;
        reno_mss *= 2; // Double the MSS

        if(reno_cwnd > reno_ssthresh) {
            reno_explanation += QString("The Congestion Window is now greater than the Slow start Threshold (%1 > %2), therefore we switch to Congestion Avoidance.").arg(reno_cwnd).arg(reno_ssthresh);
            reno_slow_start = false; // Switch to congestion avoidance
        } else {
            reno_explanation += QString("The congestion window is still less than/equal to than the slow start threshold (%1 <= %2), therefore we remain in Slow Start.").arg(reno_cwnd).arg(reno_ssthresh);
        }

    } else { // Congestion avoidance
        reno_explanation += QString("We are in Congestion Avoidance mode.\n"
                                     "Congestion Window += (MSS^2) / cwnd\n"
                                     "Congestion Window += %1^2 / %2\n"
                                     "Congestion Window = %3").arg(reno_mss).arg(reno_cwnd).arg(floor(reno_cwnd + (pow(reno_mss, 2) / reno_cwnd)));
        reno_cwnd += pow(reno_mss, 2) / reno_cwnd; // cwnd += mss^2 / cwnd
    }

    ui->textBrowser_reno_action->setText(reno_explanation);

    ui->pushButton_send->setEnabled(true);
    ui->pushButton_ack->setEnabled(false);
    ui->pushButton_drop->setEnabled(false);
}

void CongestionWindow::on_pushButton_drop_clicked()
{
    /******************** Tahoe ********************/
    QString tahoe_explanation;

    tahoe_explanation = QString("A packet has been dropped.\n\n"
                        "Set the new Slow Start threshold to half the current Congestion Window (%1 / 2 = %2).\n"
                        "Reset the congestion window back to 1.\n"
                        "Return to Slow Start mode (fast retransmit).").arg(tahoe_cwnd).arg(tahoe_cwnd / 2);

    ui->textBrowser_tahoe_action->setText(tahoe_explanation);

    tahoe_ssthresh = tahoe_cwnd / 2; // Divide by 2
    tahoe_mss = 1;
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

void CongestionWindow::on_pushButton_clear_clicked()
{
    ui->pushButton_clear->setEnabled(false); // Disable the Clear button
    qDebug() << "Cleared the clear button";

    // Reset all variables
    reset_variables();

    draw_plots(); // Redraw empty plots
}

void CongestionWindow::on_pushButton_info_congestion_vs_flow_clicked()
{
    QMessageBox::information(this, ui->pushButton_info_congestion_vs_flow->text(), // Get title from text on button
                             "Flow control is set by the window size field in the receiver's TCP header. This limits how much data the receiver is willing to receive at any given time.\n\n"
                             "But what if the receive window is too large? For example, if the receive window allows for 50 packets but the physical limitations of the network can only carry across 20 packets before a timeout occurs. The sender sends all 50 packets but 30 are lost due to congestion on the router for example. These missing packets will need to be resent. This can lead to a huge amount of resending packets which slows down the network.\n\n"
                             "Congestion control limits the flow of packets on the sender's side by testing the limits to see how much the network is able to handle.");
}

void CongestionWindow::on_pushButton_info_cwnd_clicked()
{
    QMessageBox::information(this, ui->pushButton_info_cwnd->text(),
                             "The congestion window is dynamically adjusted based on how much data the network is able to transfer across before packet loss occurs. This, of course, changes over time and TCP uses congestion control strategies (TCP Tahoe and TCP Reno discussed here) to deal with this problem.");
}

void CongestionWindow::on_pushButton_info_t_vs_r_clicked()
{
    QMessageBox::information(this, ui->pushButton_info_t_vs_r->text(),
                             "TCP Tahoe is a congestion control mechanism that was introduced in 1987. It implements a slow start strategy at the beginning of a connection or at packet loss. Once the congestion window is greater than the slow start threshold (ssthresh), it switches to congestion avoidance.\n"
                             "On packet loss, the congestion window is reset and the ssthresh is set to half the previous congestion window. It re-enters the slow start state.\n\n"
                             "TCP Reno was introduced in 1990. It too uses slow start at the beginning of a connection, and congestion avoidance once the congestion window is greater than ssthresh.\n"
                             "However, unlike Tahoe, Reno does not re-enter the slow start state. It sets both the ssthresh AND the congestion window to half the previous congestion window and resumes congestion avoidance. This is known as fast retransmit and recovery.");
}

void CongestionWindow::on_pushButton_info_ssthresh_clicked()
{
    QMessageBox::information(this, ui->pushButton_info_ssthresh->text(),
                             "This value is used to determine whether or not TCP should use slow start or congestion avoidance.\n\n"
                             "It is initially set to a large number and adjusted as required.\n\n"
                             "If congestion window <= ssthresh:\n"
                             "    Use slow start\n"
                             "If congestion window > ssthresh:\n"
                             "    Use congestion avoidance");
}

void CongestionWindow::on_pushButton_info_slow_start_clicked()
{
    QMessageBox::information(this, ui->pushButton_info_slow_start->text(),
                             "Slow start is a congestion control strategy. It begins by setting the congestion window to a low number (1 or 2 for example). As ACKs are received, the congestion window is doubled for each round-trip, thus leading to an exponential-growth.\n\n"
                             "This will continue until either:\n"
                             "(i)   a packet is dropped\n"
                             "(ii)  the receivers window size is reached (flow control)\n"
                             "(iii) the slow start threshold is reached\n\n"
                             "If the ssthresh is reached, the congestion control strategy switches to congestion avoidance.");
}

void CongestionWindow::on_pushButton_info_cong_avoid_clicked()
{
    QMessageBox::information(this, ui->pushButton_info_cong_avoid->text(),
                             "Congestion avoidance takes a more conservative approach. When the congestion window is above the ssthresh, it means that the network limit could be close.\n\n"
                             "Congestion avoidance increases the congestion window size linearly (rather than exponentially like slow start).");
}
