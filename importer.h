#ifndef IMPORTER_H
#define IMPORTER_H

#include <QObject>
#include <QThread>
#include <QtSql>
#include "xlsxdocument.h"

class Importer : public QObject
{
    Q_OBJECT
public:
    explicit Importer(QObject *parent = 0);

    void requestWork(const QString fname, const QString dbname, const QStringList dblist);

signals:
    void progress(int i);

public slots:


private:
    QSqlDatabase currentDatabase() const;
    void readRecords(QXlsx::Document *doc);
    bool checkForExp(QSqlTableModel* table, QSqlRecord* rec);

    QString activeDb;

    int uniqueDTid;
    int uniqueDepid;
    int uniqueDevid;
    int uniqueClid;
    int uniqueWblid;
    int uniqueWid;
    int uniqueRqid;

    QSqlRecord* deviceType;
    QSqlRecord* department;
    QSqlRecord* device;
    QSqlRecord* client;
    QSqlRecord* waybill;
    QSqlRecord* work;
    QSqlRecord* request;

    QSqlTableModel* typeModel;
    QSqlTableModel* departModel;
    QSqlTableModel* deviceModel;
    QSqlTableModel* clientModel;
    QSqlTableModel* waybillModel;
    QSqlTableModel* workModel;
    QSqlTableModel* requestModel;
};

#endif // IMPORTER_H
