#ifndef REPAIRDIALOG_H
#define REPAIRDIALOG_H

#include <QDialog>
#include <QSqlRecord>
#include "ui_repairdialog.h"


class RepairDialog : public QDialog, private Ui::RepairDialog
{
    Q_OBJECT

public:
    explicit RepairDialog(const QSqlRecord &record, QWidget *parent = 0);
    ~RepairDialog();

    QString getSerial();
    QString getMalfunctions();
    QString getRDate();
    QString getSDate();
    QString getDescription();

};

#endif // REPAIRDIALOG_H
