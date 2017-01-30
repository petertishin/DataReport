#ifndef ACCESSDIALOG_H
#define ACCESSDIALOG_H

#include <QDialog>
#include <QtSql>
#include "ui_accessdialog.h"


class AccessDialog : public QDialog, private Ui::AccessDialog
{
    Q_OBJECT

public:
    explicit AccessDialog(QSqlTableModel *table, QSqlRecord recordO,
                          QSqlRecord recordN, QWidget *parent = 0);
    ~AccessDialog();

    bool getSC();

public slots:
    void submit();

private:
    QSqlTableModel *model;
    QSqlRecord recO;
    QSqlRecord recN;
};

#endif // ACCESSDIALOG_H
