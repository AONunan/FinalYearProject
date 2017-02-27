#ifndef SIDEBYSIDEWINDOW_H
#define SIDEBYSIDEWINDOW_H

#include <QDialog>

namespace Ui {
class SideBySideWindow;
}

class SideBySideWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SideBySideWindow(QWidget *parent = 0);
    ~SideBySideWindow();

private:
    Ui::SideBySideWindow *ui;
};

#endif // SIDEBYSIDEWINDOW_H
