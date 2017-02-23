#ifndef FILTERSETTINGSWINDOW_H
#define FILTERSETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
class FilterSettingsWindow;
}

class FilterSettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FilterSettingsWindow(QWidget *parent = 0);
    ~FilterSettingsWindow();

private slots:
    void on_pushButton_temp_clicked();

private:
    Ui::FilterSettingsWindow *ui;
};

#endif // FILTERSETTINGSWINDOW_H
