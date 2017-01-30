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
    void difference(QSqlRecord &rec);//сигнал для открытия диалога о перезаписи совместимостей

public slots:
    bool saveDifference();

private:
    QSqlDatabase currentDatabase() const;
    void readRecords();
    bool checkForExp(QSqlTableModel* table, QSqlRecord* rec);

    int findDepartId(int row, int col);
    int findDevTypeId(int row, int col);
    int findDeviceId(int row, int col);
    int findClientId(int row, int col);
    int findWaybillId(int row, int col);
    int findWorkId(int row, int col);
    int findRequestId(int row, int col);

    int changeRecord(QSqlTableModel* model, QSqlRecord* rec, int row, int col);

    int addNewDepart(int row, int col);
    int addNewDevType(int row, int col);
    int addNewDevice(int row, int col);
    int addNewClient(int row, int col);
    int addNewWaybill(int row, int col);
    int addNewWork(int row, int col);
    int addNewRequest(int row, int col);

    int generateDepartId();
    int generateDevTypeId();
    int generateDeviceId();
    int generateClientId();
    int generateWaybillId();
    int generateWorkId();
    int generateRequestId();

    QString activeDb;
    QXlsx::Document doc;
    QStringList sheetList;

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
