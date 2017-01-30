#ifndef STARTPAGE_H
#define STARTPAGE_H

#include <QWidget>
#include "ui_startpage.h"

class StartPage : public QWidget, private Ui::StartPage
{
    Q_OBJECT

public:
    explicit StartPage(const QString &dbname, const QStringList &tbls, QWidget *parent = 0);
    ~StartPage();

    void showHistory();

public slots:
    void on_waybillBtn_clicked();
    void on_deviceBtn_clicked();
    void on_repairBtn_clicked();
    void on_updateBtn_clicked();

signals:
    void openWaybillBrowser();
    void openDeviceBrowser();
    void openRepair();
    void openDialExcelBrowser();

private:
    QStringList history;//абстрактно для отображения последних изменений
};

#endif // STARTPAGE_H
