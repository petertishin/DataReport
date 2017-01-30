#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QtSql>
#include "ui_clientdialog.h"

class ClientDialog : public QDialog, private Ui::ClientDialog
{
    Q_OBJECT

public:
    ClientDialog(QSqlRelationalTableModel *client, const QSqlRecord &clientData, const QString &dbname,
                          QWidget *parent = 0);
    ~ClientDialog();
    QString getFirstname();
    QString getLastName();
    QString getThirdname();
    QString getTelephone();
    QString getEmail();
    int getDepartName();

private slots:
    void editingFinished();//изменение статуса сохранения после редактирования
    bool save();
    void submit();//подтверждение данных и их запись в базу
    void createNewDepart();

//связь устанавливается при создании диалога и при закрытии разрушается
private:
    QSqlDatabase currentDatabase() const;
    int findClientId();
    int addNewClient();
    int addNewDepart(const QString &name);
    int generateClientId();


    QSqlRelationalTableModel *clientTable;
    QSqlTableModel *departModel;
    QString activeDb;
    bool saved;
    int clientId;

};

#endif // CLIENTDIALOG_H
