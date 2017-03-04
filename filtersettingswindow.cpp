#include "filtersettingswindow.h"
#include "ui_filtersettingswindow.h"
#include <QDebug>

FilterSettingsWindow::FilterSettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterSettingsWindow)
{
    ui->setupUi(this);
}

FilterSettingsWindow::~FilterSettingsWindow()
{
    delete ui;
}

void FilterSettingsWindow::on_pushButton_temp_clicked()
{
    QString src_host, dst_host,
            src_port, dst_port,
            protocol,
            src_expression, dst_expression, // Left hand side, right hand side of filter expression
            filter_expression; // Final filter expression

    if(ui->lineEdit_src_host->text() != "") {
        src_host = QString("(%1src host %2)").arg(ui->checkBox_src_host->isChecked() ? "not " : "").arg(ui->lineEdit_src_host->text());
    }

    if(ui->lineEdit_dst_host->text() != "") {
        dst_host = QString("(%1dst host %2)").arg(ui->checkBox_dst_host->isChecked() ? "not " : "").arg(ui->lineEdit_dst_host->text());
    }

    if(ui->lineEdit_src_port->text() != "") {
        src_port = QString("(%1src port %2)").arg(ui->checkBox_src_port->isChecked() ? "not " : "").arg(ui->lineEdit_src_port->text());
    }

    if(ui->lineEdit_dst_port->text() != "") {
        dst_port = QString("(%1dst port %2)").arg(ui->checkBox_dst_port->isChecked() ? "not " : "").arg(ui->lineEdit_dst_port->text());
    }

    protocol = QString("(protocol %1)").arg(ui->comboBox_protocol->currentText());

    if(src_host == "" && src_port == "") {
        src_expression = "";
    } else if(src_host != "" && src_port == "") {
        src_expression = src_host;
    } else if(src_host == "" && src_port != "") {
        src_expression = src_port;
    } else if(src_host != "" && src_port != "") {
        src_expression = QString("%1 %2").arg(src_host).arg(src_port);
    }

    if(dst_host == "" && dst_port == "") {
        dst_expression = "";
    } else if(dst_host != "" && dst_port == "") {
        dst_expression = dst_host;
    } else if(dst_host == "" && dst_port != "") {
        dst_expression = dst_port;
    } else if(dst_host != "" && dst_port != "") {
        dst_expression = QString("%1 %2").arg(dst_host).arg(dst_port);
    }

    if(src_expression == "" && dst_expression == "") { // Both sides are empty
        filter_expression = "";
    } else if(src_expression != "" && dst_expression == "") { // Source has content
        filter_expression = src_expression;
    } else if(src_expression == "" && dst_expression != "") { // Destination has content
        filter_expression = dst_expression;
    } else if(src_expression != "" && dst_expression != "") { // Both sides have content
        filter_expression = QString("(%1) and (%2)").arg(src_expression).arg(dst_expression);
    }

    ui->label_temp->setText(filter_expression);
    qDebug() << "FILTER EXP:" << filter_expression;
}

void FilterSettingsWindow::on_button_applyFilter_clicked()
{
    ui->button_applyFilter->setEnabled(false);
}
