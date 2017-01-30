#ifndef DEVICEBROWSER_H
#define DEVICEBROWSER_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include "ui_devicebrowser.h"

QT_FORWARD_DECLARE_CLASS(QTableView)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QSqlDatabase)
QT_FORWARD_DECLARE_CLASS(QSqlError)

class DeviceBrowser: public QWidget, private Ui::DeviceBrowser
{
    Q_OBJECT
public:
    DeviceBrowser(const QString &dbname, const QStringList &tbls, QWidget *parent = 0);
    virtual ~DeviceBrowser();

    QSqlDatabase currentDatabase() const;

    void insertRow();
    void deleteRow();
    void copyRow();
    void editRow();
    void updateActions();

public slots:
    void exec();
    void showTable(const QString &table);
    void currentChanged() { updateActions(); }
    void submitDatabase();

    void sendStatusMessage(const QString &message);
    void openNewTab(const QSqlRecord &record);//change to enum(open___)

    void on_insertRowAction_triggered() { insertRow(); }
    void on_deleteRowAction_triggered() { deleteRow(); }
    void on_copyRowAction_triggered() { copyRow(); }
    void on_editRowAction_triggered() { editRow(); }
    void on_updateTableAction_triggered() { tables.at(3); }
    void on_updateBtn_clicked()
    {
        showTable(tables.at(3));
    }

signals:
    void statusMessage(const QString &message);
    void openEditWidget(const QSqlRecord &record);
    void editSendInfo(const QModelIndexList &selection);
    //void editSendInfo(const QSqlRecord &record);

private:
    int tabId;
    QString activeDb;
    QStringList tables;
    QStringList tabsNames;
};

#endif // DEVICEBROWSER_H
