#ifndef LOGICCHECKDIALOG_H
#define LOGICCHECKDIALOG_H

#include <QDialog>

namespace Ui {
class LogicCheckDialog;
}

class LogicCheckDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogicCheckDialog(QWidget *parent = 0);
    ~LogicCheckDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::LogicCheckDialog *ui;
};

#endif // LOGICCHECKDIALOG_H
