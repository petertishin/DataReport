#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include <ui_devicewidget.h>

QT_FORWARD_DECLARE_CLASS(QTableView)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QSqlError)

class DeviceWidget : public QWidget, private Ui::DeviceWidget
{
    Q_OBJECT
public:
    DeviceWidget(const QSqlRecord &record, const QString &dbname, const QStringList &tbls, QWidget *parent = 0);
    virtual ~DeviceWidget();

    QSqlDatabase currentDatabase() const;
    void exec();
    void insertRow();
    void deleteRow();
    void copyRow();
    void editRow();
    void updateActions();

public slots:
    void showTable(const QString &table, const QString &filter);
    void currentChanged() { updateActions(); }

    //void on_insertRowAction_triggered() { insertRow(); }
    void on_deleteRowAction_triggered() { deleteRow(); }
    //void on_copyRowAction_triggered() { copyRow(); }
    void on_editRowAction_triggered() { editRow(); }
    //void on_revertAction_triggered();
    //void on_selectAction_triggered();
    //void on_updateTableAction_triggered() { showTable(tables.at(5), filter); }
    //void on_updateBtn_clicked() { showTable(tables.at(5), filter); }

private slots:
    bool save();
    void newDepartment();
    void okButton_clicked();
    void cancelButton_clicked();

signals:
    void statusMessage(const QString &message);
    //void openNewTab(const QSqlRecord &record);
    void closeTab();

private:
    bool saved;
    QString filter;
    QString activeDb;
    QStringList tables;
    QSqlRelationalTableModel *model;
};

#endif // EDITDEVICE_H
