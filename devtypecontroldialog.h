#ifndef DEVTYPECONTROLDIALOG_H
#define DEVTYPECONTROLDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QDataWidgetMapper>
#include "ui_devtypecontroldialog.h"

class DevTypeControlDialog : public QDialog, private Ui::DevTypeControlDialog
{
    Q_OBJECT
public:
    explicit DevTypeControlDialog(QWidget *parent = 0);

    void done(int result);

private slots:
    void addDevType();
    void editDevType();
    void deletedevType();

private:
    QSqlRelationalTableModel *tableModel;
    QDataWidgetMapper *mapper;
    QPushButton *addBtn;
    QPushButton *editBtn;
    QPushButton *deleteBtn;

};

#endif // DEVTYPECONTROLDIALOG_H
