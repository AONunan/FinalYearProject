#ifndef CONGESTIONWINDOW_H
#define CONGESTIONWINDOW_H

#include <QDialog>

namespace Ui {
class CongestionWindow;
}

class CongestionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CongestionWindow(QWidget *parent = 0);
    ~CongestionWindow();

private:
    Ui::CongestionWindow *ui;
};

#endif // CONGESTIONWINDOW_H
