#ifndef DEVTYPECONTROLDIALOG_H
#define DEVTYPECONTROLDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QDataWidgetMapper>
#include "ui_devtypecontroldialog.h"

class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QSqlRelationalTableModel;

class DevTypeControlDialog : public QDialog, private Ui::DevTypeControlDialog
{
    Q_OBJECT
public:
    explicit DevTypeControlDialog(const QString &dbname, QWidget *parent = 0);

    void done(int result);

private slots:
    void addDevType();
    void editDevType();
    void deletedevType();

private:
    QSqlDatabase currentDatabase() const;
    void decreaseTypeCount(QModelIndex typeIndex);
    QModelIndex indexOfType(const QString &type);
    void readTypeData();
    void removeTypeFromDatabase(QModelIndex type);
    void removeTypeFromFile(int id);

    QSqlRelationalTableModel *model;
    QString activeDb;
    QPushButton *editBtn;
    QPushButton *delBtn;
};

#endif // DEVTYPECONTROLDIALOG_H
