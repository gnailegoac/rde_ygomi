#include "LogicCheckDialog.h"
#include "ui_LogicCheckDialog.h"
#include <QStandardItemModel>
#include "facade/ApplicationFacade.h"
#include "proxy/MainProxy.h"
#include "model/QIModel.h"
#include "model/Point3D.h"

View::LogicCheckDialog::LogicCheckDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogicCheckDialog)
{
    ui->setupUi(this);

    QStandardItemModel* model = new QStandardItemModel(ui->treeView);
    ui->treeView->setModel(model);

    model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("Error Code")<<QStringLiteral("Count"));

    MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
    const std::shared_ptr<Model::QIModel>& qiModel = mainProxy.GetQIModel();
    if (qiModel != nullptr)
    {
        const std::map<int, std::vector<Model::Point3D>>& errMap = qiModel->getErrPointMap();
        for (const auto& pointListIter : errMap)
        {
            QList<QStandardItem*> list;
            list.push_back(new QStandardItem(QString::number(pointListIter.first)));
            list.push_back(new QStandardItem(QString::number(pointListIter.second.size())));
            model->appendRow(list);
        }
    }
}

View::LogicCheckDialog::~LogicCheckDialog()
{
    delete ui;
}

void View::LogicCheckDialog::on_pushButton_clicked()
{
    destroy();
}

void View::LogicCheckDialog::on_treeView_clicked(const QModelIndex &index)
{
    int errCode;
    if (index.column() ==0)
        errCode = index.data().toInt();
    else
        errCode = index.sibling(index.row(), 0).data().toInt();

    MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
    const std::shared_ptr<Model::QIModel>& qiModel = mainProxy.GetQIModel();
    if (qiModel != nullptr)
    {
        ApplicationFacade::SendNotification(ApplicationFacade::SELECT_ERROR_CODE);
    }
}
