#ifndef WAYBILLWIDGET_H
#define WAYBILLWIDGET_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include "ui_waybillwidget.h"

QT_FORWARD_DECLARE_CLASS(QTableView)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QSqlError)

class WayBillWidget : public QWidget, private Ui::WayBillWidget
{
    Q_OBJECT
public:
    WayBillWidget(const QString &dbname, const QStringList &tbls, QWidget *parent = 0);
    WayBillWidget(const QSqlRecord &record, const QString &dbname, const QStringList &tbls, QWidget *parent = 0);
    virtual ~WayBillWidget();

    QSqlDatabase currentDatabase() const;
    void exec();

public slots:
    void showTable(const QString &table, const QString &filter);
    void currentChanged() { updateActions(); }

    void on_insertRowAction_triggered() { insertRow(); }
    void on_deleteRowAction_triggered() { deleteRow(); }
    void on_copyRowAction_triggered() { copyRow(); }
    void on_editRowAction_triggered() { editRow(); }
    //void on_revertAction_triggered();
    //void on_selectAction_triggered();
    void on_updateTableAction_triggered() { showTable(tables.at(5), filter); }
    //void on_updateBtn_clicked() { showTable(tables.at(5), filter); }

private slots:
    bool save();
    void changeDepartInfo(int);
    void changeClientInfo(int);
    void newDepartBtn_clicked();
    void newClientBtn_clicked();
    void printExcelReport_clicked();
    void okButton_clicked();
    void cancelButton_clicked();

signals:
    void statusMessage(const QString &message);
    //void openEditWidget(const QSqlRecord &record);
    void closeTab();
    //void editSendInfo(const QModelIndexList &selection);
    //void editSendInfo(const QSqlRecord &record);

private:
    int generateWaybillId();
    void insertRow();
    void deleteRow();
    void copyRow();
    void editRow();
    void updateActions();

    bool saved;
    QString filter;//фильтр по ремонту на изделия
    QString activeDb;
    QStringList tables;
    QSqlTableModel *departModel;
    QSqlTableModel *clientModel;
    QSqlTableModel *workModel;
    QSqlRelationalTableModel *model;//отображает список всех ремонтных работ
};

#endif // WAYBILLWIDGET_H
