#ifndef DEPARTCLIENTVIEWER_H
#define DEPARTCLIENTVIEWER_H

#include <QDialog>
#include <QtSql>
#include "ui_departclientviewer.h"

class DepartClientViewer : public QDialog, private Ui::DepartClientViewer
{
    Q_OBJECT

public:
    explicit DepartClientViewer(QWidget *parent = 0);
    ~DepartClientViewer();

private slots:
    void updateClientView();
    void addDepartment();
    void deleteDepartment();
    void editClient();

private:
    void createDepartmentPanel();
    void createClientPanel();

    QSqlRelationalTableModel *departmentModel;
    QSqlRelationalTableModel *clientModel;
    QPushButton *addBtn;
    QPushButton *deleteBtn;
    QPushButton *editBtn;

};

#endif // DEPARTCLIENTVIEWER_H
