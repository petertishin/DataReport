#ifndef DEPARTMENTDIALOG_H
#define DEPARTMENTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QtSql>
#include <ui_departmentdialog.h>

class DepartmentDialog : public QDialog, private Ui::DepartmentDialog
{
    Q_OBJECT

public:
    DepartmentDialog(QSqlTableModel *table, QSqlRecord *rec, QWidget *parent = 0);
    DepartmentDialog(QSqlTableModel *table, int num, QWidget *parent = 0);
    ~DepartmentDialog();
    int getDepartId();

//получение всего необходимого осуществляется череч конструктор
//public slots:
//    void receiveDepData(const QSqlRecord &depData);//при вызове диалога

private slots:
    void submit();//подтверждение данных и их запись в базу


private:
    int findDepartId();
    int addNewDepart();
    int generateDepartId();

    QSqlTableModel *departTable;
    QSqlRecord *record;
};

#endif // DEPARTMENTDIALOG_H
