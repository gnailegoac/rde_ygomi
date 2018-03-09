/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DbValidationDialog.cpp
 *******************************************************************************
 */

#include "DbValidationDialog.h"
#include "ui_DbValidationDialog.h"
#include <QFile>
#include <QHeaderView>
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
    setMaximumSize(QSize(1210, 645));
    resize(410, 240);
    connect(mUi->pBtnShowDetails, SIGNAL(clicked()), this, SLOT(onShowDetails()));
    connect(mUi->pBtnContinue, SIGNAL(clicked()), this, SLOT(onContinue()));
    connect(mUi->pBtnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
    initTableOverView();
    initTableDetails();
}

View::DbValidationDialog::~DbValidationDialog()
{
    delete mUi;
}

void View::DbValidationDialog::onShowDetails()
{
    int deltaW = 805;
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

void View::DbValidationDialog::onContinue()
{
    this->accept();
}

void View::DbValidationDialog::onCancel()
{
    this->reject();
}

void View::DbValidationDialog::initTableOverView()
{
    mUi->tableOverview->setColumnCount(2);
    mUi->tableOverview->horizontalHeader()->setDefaultSectionSize(199);
    QStringList header;
    header << tr("Error Level") << tr("Error Number");
    mUi->tableOverview->setHorizontalHeaderLabels(header);
    QFont font = mUi->tableOverview->horizontalHeader()->font();
    font.setBold(true);
    mUi->tableOverview->horizontalHeader()->setFont(font);
    mUi->tableOverview->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    mUi->tableOverview->horizontalHeader()->setStretchLastSection(true);
    mUi->tableOverview->verticalHeader()->setVisible(false);
    mUi->tableOverview->setFrameShape(QFrame::NoFrame);
    mUi->tableOverview->setShowGrid(true);
    mUi->tableOverview->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mUi->tableOverview->setSelectionBehavior(QAbstractItemView::SelectRows);
    mUi->tableOverview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mUi->tableOverview->setRowCount(2);
    mUi->tableOverview->setRowHeight(0, 30);
    mUi->tableOverview->setRowHeight(1, 30);
    mUi->tableOverview->setItem(0, 0, new QTableWidgetItem(scLevelWarning));
    mUi->tableOverview->setItem(0, 1, new QTableWidgetItem(QString("0")));
    mUi->tableOverview->setItem(1, 0, new QTableWidgetItem(scLevelError));
    mUi->tableOverview->setItem(1, 1, new QTableWidgetItem(QString("0")));
}

void View::DbValidationDialog::initTableDetails()
{
    mUi->tableDetails->setColumnCount(6);
    mUi->tableDetails->horizontalHeader()->setDefaultSectionSize(200);
    QStringList header;
    header << tr("Level") << tr("Description") << tr("DBName") << tr("TabelName") << tr("ErrorValue") << tr("RowId");
    mUi->tableDetails->setHorizontalHeaderLabels(header);
    QFont font = mUi->tableDetails->horizontalHeader()->font();
    font.setBold(true);
    mUi->tableDetails->horizontalHeader()->setFont(font);
    mUi->tableDetails->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    mUi->tableDetails->horizontalHeader()->setStretchLastSection(true);
    mUi->tableDetails->verticalHeader()->setVisible(false);
    mUi->tableDetails->setFrameShape(QFrame::NoFrame);
    mUi->tableDetails->setShowGrid(true);
    mUi->tableDetails->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mUi->tableDetails->setSelectionBehavior(QAbstractItemView::SelectRows);
    mUi->tableDetails->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mUi->tableDetails->setColumnWidth(0, 100);
    mUi->tableDetails->setColumnWidth(1, 400);
    mUi->tableDetails->setColumnWidth(2, 200);
    mUi->tableDetails->setColumnWidth(3, 200);
    mUi->tableDetails->setColumnWidth(4, 200);
    mUi->tableDetails->setColumnWidth(5, 85);
}

bool View::DbValidationDialog::UpdateData(const QString& aFilePath)
{
    QFile file(aFilePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(data.toUtf8(), &parseError);
    if (document.isNull() || parseError.error != QJsonParseError::NoError)
    {
        return false;
    }

    int seriousLevelCount = 0;
    int verifyLevelCount = 0;
    QJsonObject mainObject = document.object();
    for(int i = 50100; i < 50815; ++i)
    {
        QJsonValue value = mainObject.value(QString::number(i));
        if(!value.isObject())
        {
            continue;
        }
        QJsonObject errorCodeObject = value.toObject();
        QJsonArray details = errorCodeObject.value("Details").toArray();
        if(details.empty())
        {
            continue;
        }
        int startRow = mUi->tableDetails->rowCount();
        for(int i = 0; i < details.size(); ++i)
        {
            int rowNum = mUi->tableDetails->rowCount();
            mUi->tableDetails->insertRow(rowNum);
            mUi->tableDetails->setRowHeight(rowNum, 40);
            QJsonObject itemObject = details.at(i).toObject();
            if(itemObject.contains(QString("DBName")))
            {
                QString DBName = itemObject.value("DBName").toString();
                mUi->tableDetails->setItem(rowNum, 2, new QTableWidgetItem(DBName));
            }
            if(itemObject.contains(QString("TableName")))
            {
                QString TableName = itemObject.value("TableName").toString();
                mUi->tableDetails->setItem(rowNum, 3, new QTableWidgetItem(TableName));
            }
            if(itemObject.contains(QString("errorValue")))
            {
                QString errorValue = itemObject.value("errorValue").toString();
                mUi->tableDetails->setItem(rowNum, 4, new QTableWidgetItem(errorValue));
            }
            if(itemObject.contains(QString("rowID")))
            {
                QString rowID = itemObject.value("rowID").toString();
                mUi->tableDetails->setItem(rowNum, 5, new QTableWidgetItem(rowID));
            }
        }
        mUi->tableDetails->setSpan(startRow, 0, details.size(), 1);
        mUi->tableDetails->setSpan(startRow, 1, details.size(), 1);
        if(errorCodeObject.contains(QString("Description")))
        {
            QString description = errorCodeObject.value("Description").toString();
            mUi->tableDetails->setItem(startRow, 1, new QTableWidgetItem(description));
        }
        if(errorCodeObject.contains(QString("Level")))
        {
            QString level = errorCodeObject.value("Level").toString();
            mUi->tableDetails->setItem(startRow, 0, new QTableWidgetItem(level));
            if(!level.compare(scLevelError))
            {
                ++seriousLevelCount;
            }
            if(!level.compare(scLevelWarning))
            {
                ++verifyLevelCount;
            }
        }
    }

    mUi->tableOverview->setItem(0, 1, new QTableWidgetItem(QString::number(verifyLevelCount)));
    mUi->tableOverview->setItem(1, 1, new QTableWidgetItem(QString::number(seriousLevelCount)));
    return true;
}

void View::DbValidationDialog::ResetPos()
{
    move(((QWidget*)parent())->pos());
}

void View::DbValidationDialog::setBtnShowDetailsEnabled(bool aIsEnabled)
{
    mUi->pBtnShowDetails->setEnabled(aIsEnabled);
}

void View::DbValidationDialog::setBtnContinueEnabled(bool aIsEnabled)
{
    mUi->pBtnContinue->setEnabled(aIsEnabled);
}

void View::DbValidationDialog::setBtnCancelEnabled(bool aIsEnabled)
{
    mUi->pBtnCancel->setEnabled(aIsEnabled);
}

void View::DbValidationDialog::setLabelWarningVisible(bool aIsVisible)
{
    mUi->labelWarning->setVisible(aIsVisible);
}

void View::DbValidationDialog::getErrorNumberOfLevel(std::map<QString, std::uint32_t>& aErrorNumberOfLevelMap)
{
    aErrorNumberOfLevelMap[scLevelWarning] = mUi->tableOverview->item(0, 1)->text().toInt();
    aErrorNumberOfLevelMap[scLevelError] = mUi->tableOverview->item(1, 1)->text().toInt();
}
