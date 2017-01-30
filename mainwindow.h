#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include "importer.h"
#include "ui_mainwindow.h"

QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QSqlError)

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    QSqlDatabase currentDatabase() const;
    void about();
    QSqlError addConnection(const QString &driver, const QString &dbName, const QString &host,
                  const QString &user, const QString &passwd, int port = -1);
    void createMemoryDatabase(QSqlDatabase &db);

public slots:
    void addConnection();
    void showDepartClientViewer();
    void showDeviceControlDialog();
    void on_addConnectionAction_triggered() { addConnection(); }
    void on_aboutAction_triggered() { about();}
    void sendStatusMessage(const QString &message);
    void closeTab();
    void closeTab(int index);
    void openWaybillBrowser();
    void openDeviceBrowser();
    void openRepair();
    void openWaybillEdit(const QSqlRecord &record);
    void openDeviceEdit(const QSqlRecord &record);
    void openDialExcelBrowser();
    void openAccessDial(QSqlTableModel *table, QSqlRecord recordO, QSqlRecord recordN);

signals:
    void statusMessage(const QString &message);

private:
    int tabId;
    QString activeDb;
    QStringList tables;
    Importer* importer;
};

#endif // MAINWINDOW_H
