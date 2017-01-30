#ifndef DEPARTCONTROLDIALOG_H
#define DEPARTCONTROLDIALOG_H

#include <QDialog>
#include <QDataWidgetMapper>
#include "ui_departcontroldialog.h"

class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSqlRelationalTableModel;

class DepartControlDialog : public QDialog, private Ui::DepartControlDialog
{
    Q_OBJECT
public:
    explicit DepartControlDialog(int id, QWidget *parent = 0);

    void done(int result);

private slots:
    void addDepart();
    void deleteDepart();

private:
    QSqlRelationalTableModel *tableModel;
    QDataWidgetMapper *mapper;
    QPushButton *addBtn;
    QPushButton *delBtn;
};

#endif // DEPARTCONTROLDIALOG_H
