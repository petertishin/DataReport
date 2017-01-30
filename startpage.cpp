#include "startpage.h"

#include <QtSql>
#include <QtWidgets>

StartPage::StartPage(const QString &dbname, const QStringList &tbls, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

StartPage::~StartPage()
{
}

void StartPage::on_waybillBtn_clicked()
{
    emit openWaybillBrowser();
}

void StartPage::on_deviceBtn_clicked()
{
    emit openDeviceBrowser();
}

void StartPage::on_repairBtn_clicked()
{
    emit openRepair();
}

void StartPage::on_updateBtn_clicked()
{
    emit openDialExcelBrowser();
}

void StartPage::showHistory()
{

}
