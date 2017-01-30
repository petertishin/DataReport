#include "importer.h"

bool sc;

Importer::Importer(QObject *parent) :
    QObject(parent)
{
}

void Importer::requestWork(const QString fname, const QString dbname, const QStringList dblist)
{
    activeDb = dbname;
    doc = QXlsx::Document(fname);

    departModel = new QSqlTableModel(this, currentDatabase());
    departModel->setTable(dblist.at(0));
    departModel->select();

    typeModel = new QSqlTableModel(this, currentDatabase());
    typeModel->setTable(dblist.at(1));
    typeModel->select();

    clientModel = new QSqlTableModel(this, currentDatabase());
    clientModel->setTable(dblist.at(2));
    clientModel->select();

    deviceModel = new QSqlTableModel(this, currentDatabase());
    deviceModel->setTable(dblist.at(3));
    deviceModel->select();

    waybillModel = new QSqlTableModel(this, currentDatabase());
    waybillModel->setTable(dblist.at(4));
    waybillModel->select();

    workModel = new QSqlTableModel(this, currentDatabase());
    workModel->setTable(dblist.at(5));
    workModel->select();

    requestModel = new QSqlTableModel(this, currentDatabase());
    requestModel->setTable(dblist.at(6));
    requestModel->select();

    readRecords();
}

void Importer::abort()
{

}

QSqlDatabase Importer::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}

void Importer::readRecords()
{
    bool save=false, toAll=false;
    int row, col;

    while(!doc.cellAt(row, col)->value().toString().isNull()) {
        findDevTypeId(row, col);
        findDepartId(row, col);
        findDeviceId(row, col);
        findClientId(row, col);
        findWaybillId(row, col);
        findWorkId(row, col);
        findRequestId(row, col);



        //ПЕРЕНЕСТИ ДАННУЮ ОБРАБОТКУ В find____Id(row, col)
        for(int i=0; i<typeModel->rowCount(); i++) {
            if(typeModel->record(i).value("Type").toString() == deviceType->value(1).toString()){
                break;
            } addNewDevice(row, col);
        }

        for(int i=0; i<departModel->rowCount(); i++) {
            if(departModel->record(i).value("Name").toString() == department->value(1).toString()){
                if(departModel->record(i).value("Address").toString() == department->value(1).toString()){
                    if(departModel->record(i).value("Accessories").toString() == department->value(1).toString()){
                        break;
                    } else emit saveDifference();//record,
                } else emit saveDifference(/*arg*/);
            } else addNewDepart(row, col);
        }

        //разбор устройства, добавить автозаполение состояния...
        for(int i=0; i<deviceModel->rowCount(); i++) {
            if(deviceModel->record(i).value("Serial").toInt() == device->value(0).toInt()){
                if(deviceModel->record(i).value("Type").toInt() == device->value(1).toInt()){
                    if(deviceModel->record(i).value("Produced").toString() == device->value(3).toString()){
                        if(deviceModel->record(i).value("Department").toInt() == device->value(7).toInt()){
                            break;
                        } else emit saveDifference();
                    } else emit saveDifference();
                } else emit saveDifference();
            } else addNewDevice(row, col);
        }

        for(int i=0; i<clientModel->rowCount(); i++){
            if(clientModel->record(i).value("Firstname").toString() == client->value(1).toString()){
                if(clientModel->record(i).value("Lastname").toString() == client->value(2).toString()){
                    if(clientModel->record(i).value("Thirdname").toString() == client->value(3).toString()){
                        //if(clientModel->record(i).value("DepartmentName").toInt() == client->value(6).toInt()){
                            break;
                        //else emit saveDifference();
                    } else emit saveDifference();
                } else emit saveDifference();
            } else addNewDepart(row, col);
        }

        for(int i=0; i<waybillModel->rowCount(); i++) {
            if(waybillModel->record(i).value("Serial").toInt() == waybill->value(0).toInt()){
                if(waybillModel->record(i).value("Date").toString() == waybill->value(1).toString()){
                    break;
                } else emit saveDifference();
            } else addNewWaybill(row, col);
        }

        for(int i=0; i<workModel->rowCount(); i++) {
            if((workModel->record(i).value("WaybillSerial").toInt() == work->value(6).toInt()) &&
               (workModel->record(i).value("DeviceSerial").toInt() == work->value(5).toInt())){
                if(workModel->record(i).value("DateR").toString() == work->value(3).toString()){
                    break;
                } else emit saveDifference();
            } else addNewWork(row, col);
        }

        for(int i=0; i<requestModel->rowCount(); i++) {
            if((requestModel->record(i).value("Date").toString() == request->value(1).toString()) &&
               (requestModel->record(i).value("WorkId").toInt() == request->value(6).toInt())){
                break;
            } else addNewRequest(row, col);
        }

    }
}

int Importer::findDevTypeId(int row, int col)
{
    int _row = 0;
    QSqlRecord _rec;

    while(_row < typeModel->rowCount()) {
        _rec = typeModel->record(_row);
        if(_rec.value("Type").toString() == doc.cellAt(row, col)->value().toString()) {
            return _rec.value("TypeId").toInt();
        } else changeRecord();//изменить процесс замены
    }
}
int Importer::findDepartId(int row, int col)
{
    int _row = 0;
    QSqlRecord _rec;

    departModel->setFilter("Name="+doc.cellAt(row, col)->value().toString());
    if(departModel->rowCount() > 0) {
    while(_row < departModel->rowCount()) {
        _rec = departModel->record(_row);
        if(_rec.value("Name").toString() == doc.cellAt(row, col)->value().toString()) {
            if((_rec.value("Address").toString() == doc.cellAt(row, col)->value().toString()) ||
               (_rec.value("Accessories").toString() == doc.cellAt(row, col)->value().toString())) {
                if((_rec.value("Address").toString() == doc.cellAt(row, col)->value().toString()) &&
                   (_rec.value("Accessories").toString() == doc.cellAt(row, col)->value().toString())){
                    return _rec.value("DepartmentId").toInt();
                } else if(!sc) emit difference();
                  else if(sc) emit changeRecord(departModel, _rec, row, col);
            } else if(sc) emit changeRecord(departModel, _rec, row, col);
        }
        ++_row;
    }
    } else addNewDepart(row, col);
}
int Importer::findDeviceId(int row, int col)
{
    int _row = 0;
    QSqlRecord _rec;

    deviceModel->setFilter("Serial"+doc.cellAt(row, col)->value().toInt());
    for(_row=0; _row<deviceModel->rowCount(); _row++) {
        _rec = deviceModel->record(_row);
        if(_rec.value("Serial").toInt() == doc.cellAt(row, col)->value().toInt()){//0
            if(_rec.value("Type").toInt() == doc.cellAt(row, col)->value().toInt()){//1
                if(_rec.value("Produced").toString() == doc.cellAt(row, col)->value().toString()){//3
                    if(_rec.value("Department").toInt() == doc.cellAt(row, col)->value().toInt()){//7
                        return _rec.value("Serial").toInt();
                    } else if(!sc) emit difference(); //строй всю логику на этом примере!!
                      else if(sc) changeRecord(deviceModel, _rec, row, col);
                } else if(sc) changeRecord(deviceModel, _rec, row, col);
            } else if(sc) changeRecord(deviceModel, _rec, row, col);
        }
        if(_row == deviceModel->rowCount())
            addNewDevice(row, col);
    }
}
int Importer::findClientId(int row, int col)
{
    int _row = 0;
    QSqlRecord _rec;

    clientModel->setFilter("Firstname"+doc.cellAt(row, col)->value().toString());
    for(int i=0; i<clientModel->rowCount(); i++){
        _rec = clientModel->record(i);
        if(_rec.value("Firstname").toString() == client->value(1).toString()){
            if(_rec.value("Lastname").toString() == client->value(2).toString()){
                if(_rec.value("Thirdname").toString() == client->value(3).toString()){
                    //if(_rec.value("DepartmentName").toInt() == client->value(6).toInt()){
                        return _rec.value("ClientId").toInt();
                    //else emit saveDifference();
                } else if(!sc) emit difference();
                  else if(sc) changeRecord(clientModel, _rec, row, col);
            } else if(sc) changeRecord(clientModel, _rec, row, col);
        } else addNewClient(row, col);
    }
}
int Importer::findWaybillId(int row, int col)
{

}
int Importer::findWorkId(int row, int col)
{

}
int Importer::findRequestId(int row, int col)
{

}

int Importer::changeRecord(QSqlTableModel* model, QSqlRecord* rec, int row, int col)
{

}

int Importer::addNewDevType(int row, int col)
{
    int id = generateDevTypeId();

    QSqlField devT1("TypeId", QVariant::Int);
    QSqlField devT2("Type", QVariant::String);

    devT1.setValue(id);
    devT2.setValue(doc->cellAt(row, col++)->value().toString());

    deviceType->append(devT1);
    deviceType->append(devT2);

    if (typeModel->insertRecord(-1, deviceType))
        return id;
    else return -1;
}
int Importer::addNewDepart(int row, int col)
{
    int id = generateDepartId();

    QSqlField dep1("DepartmentId", QVariant::Int);
    QSqlField dep2("Name", QVariant::String);
    QSqlField dep3("Address", QVariant::String);
    QSqlField dep4("Accessories", QVariant::String);

    dep1.setValue(id);
    dep2.setValue(doc->cellAt(row, col++)->value().toString());
    dep3.setValue(doc->cellAt(row, col++)->value().toString());
    dep4.setValue(doc->cellAt(row, col)->value().toString());

    department->append(dep1);
    department->append(dep2);
    department->append(dep3);
    department->append(dep4);

    return departModel->insertRecord(-1, department);
}
int Importer::addNewDevice(int row, int col)
{

}
int Importer::addNewClient(int row, int col)
{

}
int Importer::addNewWaybill(int row, int col)
{

}
int Importer::addNewWork(int row, int col)
{

}
int Importer::addNewRequest(int row, int col)
{

}

int Importer::generateDevTypeId()
{
    uniqueDTid = typeModel->rowCount();
    uniqueDTid += 1;
    return uniqueDTid;
}
int Importer::generateDepartId()
{
    uniqueDepid = departModel->rowCount();
    uniqueDepid += 1;
    return uniqueDepid;
}
int Importer::generateDeviceId()
{
    uniqueDevid = deviceModel->rowCount();
    uniqueDevid += 1;
    return uniqueDevid;
}
int Importer::generateClientId()
{
    uniqueClid = clientModel->rowCount();
    uniqueClid += 1;
    return uniqueClid;
}
int Importer::generateWaybillId()
{
    uniqueWblid = waybillModel->rowCount();
    uniqueWblid += 1;
    return uniqueWblid;
}
int Importer::generateWorkId()
{
    uniqueWid = workModel->rowCount();
    uniqueWid += 1;
    return uniqueWid;
}
int Importer::generateRequestId()
{
    uniqueRqid = requestModel->rowCount();
    uniqueRqid += 1;
    return uniqueRqid;
}
