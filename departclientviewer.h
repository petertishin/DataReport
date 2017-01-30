#ifndef DEPARTCLIENTVIEWER_H
#define DEPARTCLIENTVIEWER_H

#include <QDialog>
#include <QtSql>
#include "ui_departclientviewer.h"

QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QSqlDatabase)
QT_FORWARD_DECLARE_CLASS(QSqlError)

class DepartClientViewer : public QDialog, private Ui::DepartClientViewer
{
    Q_OBJECT

public:
    explicit DepartClientViewer(const QString &dbname, QWidget *parent = 0);
    ~DepartClientViewer();
    QSqlDatabase currentDatabase() const;

private slots:
    void updateClientView();
    void addDepartment();
    void editDepartment();
    void deleteDepartment();
    void addClient();
    void deleteClient();
    void editClient();

signals:
    void statusMessage(const QString &message);

private:
    void createDepartmentPanel();
    void createClientPanel();

    QSqlRelationalTableModel *departmentModel;
    QSqlRelationalTableModel *clientModel;
    QPushButton *addDepartBtn;
    QPushButton *deleteDepartBtn;
    QPushButton *editDepartBtn;
    QPushButton *addClientBtn;
    QPushButton *deleteClientBtn;
    QPushButton *editClientBtn;
    QString activeDb;
};

#endif // DEPARTCLIENTVIEWER_H
