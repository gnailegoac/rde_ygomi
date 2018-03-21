#include "LogicCheckDialog.h"
#include "ui_LogicCheckDialog.h"

LogicCheckDialog::LogicCheckDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogicCheckDialog)
{
    ui->setupUi(this);

}

LogicCheckDialog::~LogicCheckDialog()
{
    delete ui;
}

void LogicCheckDialog::on_pushButton_clicked()
{
    destroy();
}
