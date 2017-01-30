#ifndef CLIENTCONTROLDIALOG_H
#define CLIENTCONTROLDIALOG_H

#include <QDialog>
#include "ui_clientcontroldialog.h"

class QComboBox;
class QDataWidgetMapper;
class QDateEdit;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSqlRelationalTableModel;

class ClientControlDialog : public QDialog, private Ui::ClientControlDialog
{
    Q_OBJECT
public:
    ClientControlDialog(int id, QWidget *parent = 0);

    void done(int result);

private slots:
    void addClient();
    void deleteClient();

private:
    QSqlRelationalTableModel *tableModel;
    QDataWidgetMapper *mapper;
    QPushButton *addBtn;
    QPushButton *delBtn;
    QDialogButtonBox *buttonBox;
};

#endif // CLIENTCONTROLDIALOG_H
