#include "DbValidationDialog.h"
#include "ui_DbValidationDialog.h"
#include <QHeaderView>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

View::DbValidationDialog::DbValidationDialog(QWidget* parent) :
    QDialog(parent),
    mUi(new Ui::DbValidationDialog)
{
    mUi->setupUi(this);
    setWindowTitle("Validation Information");
    resize(410, 240);
    connect(mUi->pBtnShowDetails, SIGNAL(clicked()), this, SLOT(onShowDetails()));
    initTableOverView();
    setMaximumSize(QSize(810, 645));
}

View::DbValidationDialog::~DbValidationDialog()
{
    delete mUi;
}

void View::DbValidationDialog::onShowDetails()
{
    int deltaW = 405;
    int deltaH = 405;
    if(mUi->pBtnShowDetails->isChecked())
    {
        QSize size = this->size();
        this->resize(size.width() + deltaW, size.height() + deltaH);
    }
    else
    {
        QSize size = this->size();
        this->resize(size.width() - deltaW, size.height() - deltaH);
    }
    move(((QWidget*)parent())->pos());
}

void View::DbValidationDialog::initTableOverView()
{
    mUi->tableOverview->setColumnCount(2);
    mUi->tableOverview->horizontalHeader()->setDefaultSectionSize(200);
    QStringList header;
    header << tr("Error Level") << tr("Error Number");
    mUi->tableOverview->setHorizontalHeaderLabels(header);
    QFont font = mUi->tableOverview->horizontalHeader()->font();
    font.setBold(true);
    mUi->tableOverview->horizontalHeader()->setFont(font);
    mUi->tableOverview->horizontalHeader()->setDefaultAlignment (Qt::AlignLeft);
    mUi->tableOverview->horizontalHeader()->setStretchLastSection(true);
    mUi->tableOverview->verticalHeader()->setVisible(false);
    mUi->tableOverview->setFrameShape(QFrame::NoFrame);
    mUi->tableOverview->setShowGrid(false);
    mUi->tableOverview->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mUi->tableOverview->setSelectionBehavior(QAbstractItemView::SelectRows);
    mUi->tableOverview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mUi->tableOverview->setRowCount(2);
    mUi->tableOverview->setRowHeight(0, 30);
    mUi->tableOverview->setRowHeight(1, 30);
    mUi->tableOverview->setItem(0, 0, new QTableWidgetItem(QString("Need To Verify")));
    mUi->tableOverview->setItem(0, 1, new QTableWidgetItem(QString("0")));
    mUi->tableOverview->setItem(1, 0, new QTableWidgetItem(QString("Serious Error")));
    mUi->tableOverview->setItem(1, 1, new QTableWidgetItem(QString("0")));
}

void View::DbValidationDialog::UpdateData(const QString& filePath)
{
    QFile file(filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = file.readAll();
    file.close();

    int seriousLevelCount = 0;
    int verifyLevelCount = 0;
    QString space = "&nbsp;&nbsp;&nbsp;&nbsp;";
    QString textContent = "<html>\n<body>\n";
    textContent += "<p style=""line-height:120%"">";
    QJsonObject mainObject = QJsonDocument::fromJson(data.toUtf8()).object();
    for(int i = 50100; i < 50815; ++i)
    {
        QJsonValue value = mainObject.value(QString::number(i));
        if(!value.isObject())
        {
            continue;
        }
        QJsonObject ErrorCodeObject = value.toObject();
        QString description = ErrorCodeObject.value("Description").toString();
        QString level = ErrorCodeObject.value("Level").toString();
        textContent += "<font size=""4"" color=""red""><b>" + tr("Error Code: ") + QString::number(i) + "</b></font><br>";
        textContent +=  space + tr("Description: ") + description + "<br>";
        textContent +=  space + tr("Level: ") + level;
        if(!level.compare("Serious Error"))
        {
            ++seriousLevelCount;
        }
        if(!level.compare("Need To Verify"))
        {
            ++verifyLevelCount;
        }
        if(!ErrorCodeObject.value("Details").isArray())
        {
            textContent += "<br><br>";
            continue;
        }
        QJsonArray details = ErrorCodeObject.value("Details").toArray();
        textContent += "<br>" + space + tr("Details: ");
        for(int i = 0; i < details.size(); ++i)
        {
            textContent += "<br>";
            QJsonObject itemObject = details.at(i).toObject();
            QString DBName = itemObject.value("DBName").toString();
            QString TableName = itemObject.value("TableName").toString();
            QString errorValue = itemObject.value("errorValue").toString();
            QString rowID = itemObject.value("rowID").toString();
            textContent +=  space + space + tr("DBName: ") + DBName + "<br>";
            textContent +=  space + space + tr("TableName: ") + TableName + "<br>";
            textContent +=  space + space + tr("errorValue: ") + errorValue + "<br>";
            textContent +=  space + space + tr("rowID: ") + rowID;
        }
        textContent += "<br><br>";
    }
    textContent += "</p>\n</body>\n</html>";

    mUi->textBrowser->setText(textContent);
    mUi->tableOverview->setItem(0, 1, new QTableWidgetItem(QString::number(verifyLevelCount)));
    mUi->tableOverview->setItem(1, 1, new QTableWidgetItem(QString::number(seriousLevelCount)));
}

void View::DbValidationDialog::ResetPos()
{
    move(((QWidget*)parent())->pos());
}

bool View::DbValidationDialog::IsInterrupt()
{
    if((mUi->tableOverview->item(1, 1)->text()).compare("0") == 0)
    {
        return false;
    }
    return true;
}
