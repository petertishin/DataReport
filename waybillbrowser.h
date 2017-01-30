#ifndef WAYBILLBROWSER_H
#define WAYBILLBROWSER_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include "ui_waybillbrowser.h"

QT_FORWARD_DECLARE_CLASS(QTableView)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QSqlDatabase)
QT_FORWARD_DECLARE_CLASS(QSqlError)

class WaybillBrowser: public QWidget, private Ui::WaybillBrowser
{
    Q_OBJECT
public:
    WaybillBrowser(const QString &dbname, const QStringList &tbls, QWidget *parent = 0);
    virtual ~WaybillBrowser();

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

    void on_insertRowAction_triggered() { insertRow(); }
    void on_deleteRowAction_triggered() { deleteRow(); }
    void on_copyRowAction_triggered() { copyRow(); }
    void on_editRowAction_triggered() { editRow(); }
    void on_revertAction_triggered();
    void on_selectAction_triggered();
    void on_updateTableAction_triggered() { showTable(tables.at(4)); }
    void on_updateBtn_clicked() { showTable(tables.at(4)); }

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

class CustomModel: public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    explicit CustomModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase()):QSqlRelationalTableModel(parent, db) {}
    QVariant data(const QModelIndex &idx, int role) const
    {
        if (role == Qt::BackgroundRole && isDirty(idx))
            return QBrush(QColor(Qt::yellow));
        return QSqlRelationalTableModel::data(idx, role);
    }
};

#endif // WAYBILLBROWSER_H
