#include "importer.h"

bool sc = false;

//конструктор
Importer::Importer(QObject *parent) :
    QObject(parent)
{
}
//задание аргументов и запрос работы
void Importer::requestWork(const QString fname, const QString dbname, const QStringList dblist)
{
    activeDb = dbname;
    QXlsx::Document doc(fname);

    departModel = new QSqlTableModel(this, currentDatabase());
    departModel->setTable(dblist.at(0));
    departModel->select();
    uniqueDepid = departModel->rowCount();

    typeModel = new QSqlTableModel(this, currentDatabase());
    typeModel->setTable(dblist.at(1));
    typeModel->select();
    uniqueDTid = typeModel->rowCount();

    clientModel = new QSqlTableModel(this, currentDatabase());
    clientModel->setTable(dblist.at(2));
    clientModel->select();
    uniqueClid = clientModel->rowCount();

    deviceModel = new QSqlTableModel(this, currentDatabase());
    deviceModel->setTable(dblist.at(3));
    deviceModel->select();
    uniqueDevid = deviceModel->rowCount();

    waybillModel = new QSqlTableModel(this, currentDatabase());
    waybillModel->setTable(dblist.at(4));
    waybillModel->select();
    uniqueWblid = waybillModel->rowCount();

    workModel = new QSqlTableModel(this, currentDatabase());
    workModel->setTable(dblist.at(5));
    workModel->select();
    uniqueWid = workModel->rowCount();

    requestModel = new QSqlTableModel(this, currentDatabase());
    requestModel->setTable(dblist.at(6));
    requestModel->select();
    uniqueRqid = requestModel->rowCount();

    readRecords();
}
//прерывание потока
void Importer::abort()
{

}
//private функция обрашения к БД
QSqlDatabase Importer::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}
//чтение импортируемого файла и сравнение значений с записями в БД
void Importer::readRecords()
{
    int row, col;
    //порядок проработки документа
    while(!doc.cellAt(row, col)->value().toString().isNull()) {
        findDevTypeId(row, col);//отдельный лист
        findDepartId(row, col);//отдельный лист с клиентами
        findDeviceId(row, col);
        findClientId(row, col);//отдельный лист с департаментами
        findWaybillId(row, col);
        findWorkId(row, col);//начальная страница
        findRequestId(row, col);//отдельная страница

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
        }
        ++_row;
    }
    return addNewDevType(row, col);
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
                } else if(!sc) emit difference(departModel, _rec, _rec);
                  else if(sc) changeRecord(departModel, _rec, row, col);
            } else if(sc) changeRecord(departModel, _rec, row, col);
        }
        ++_row;
    }
    } else return addNewDepart(row, col);
}
int Importer::findDeviceId(int row, int col)
{
    int _row = 0;
    QSqlRecord _rec;

    deviceModel->setFilter("Serial"+doc.cellAt(row, col)->value().toInt());
    if(deviceModel->rowCount() > 0) {
    for(_row=0; _row<deviceModel->rowCount(); _row++) {
        _rec = deviceModel->record(_row);
        if(_rec.value("Serial").toInt() == doc.cellAt(row, col)->value().toInt()){//0
            if(_rec.value("Type").toInt() == doc.cellAt(row, col)->value().toInt()){//1
                if(_rec.value("Produced").toString() == doc.cellAt(row, col)->value().toString()){//3
                    if(_rec.value("Department").toInt() == doc.cellAt(row, col)->value().toInt()){//7
                        return _rec.value("Serial").toInt();
                    } else if(!sc) emit difference(deviceModel, _rec, _rec); //строй всю логику на этом примере!!
                      else if(sc) changeRecord(deviceModel, _rec, row, col);
                } else if(sc) changeRecord(deviceModel, _rec, row, col);
            } else if(sc) changeRecord(deviceModel, _rec, row, col);
        }
        if(_row == deviceModel->rowCount())
            addNewDevice(row, col);
    }
    } else addNewDevice(row, col);
}
int Importer::findClientId(int row, int col)
{
    int _row = 0;
    QSqlRecord _rec;

    clientModel->setFilter("Firstname"+doc.cellAt(row, col)->value().toString());
    if(clientModel->rowCount() > 0){
    for(int i=0; i<clientModel->rowCount(); i++){
        _rec = clientModel->record(i);
        if(_rec.value("Firstname").toString() == doc.cellAt(row, col)->value().toString()){//1
            if(_rec.value("Lastname").toString() == doc.cellAt(row, col)->value().toString()){//2
                if(_rec.value("Thirdname").toString() == doc.cellAt(row, col)->value().toString()){//3
                    //if(_rec.value("DepartmentName").toInt() == doc.cellAt(row, col)->value().toInt()){//6
                        return _rec.value("ClientId").toInt();
                    //else emit saveDifference();
                } else if(!sc) emit difference(clientModel, _rec, _rec);
                  else if(sc) return changeRecord(clientModel, _rec, row, col);
            } else if(sc) return changeRecord(clientModel, _rec, row, col);
        }
        if(_row == clientModel->rowCount())
            return addNewClient(row, col);
    }
    } else return addNewClient(row, col);
}
int Importer::findWaybillId(int row, int col)
{
    int _row = 0;
    QSqlRecord _rec;

    waybillModel->setFilter("Serial"+doc.cellAt(row, col)->value().toString());
    if(waybillModel->rowCount() > 0) {
    for(int i=0; i<waybillModel->rowCount(); i++){
        _rec = waybillModel->record(i);
        if(_rec.value("Serial").toInt() == doc.cellAt(row, col)->value().toInt()){//0
            if(_rec.value("Date").toString() == doc.cellAt(row, col)->value().toString()){//1
                return _rec.value("Serial").toInt();
            } else if(!sc) emit difference(waybillModel, _rec, _rec);
              else if(sc) changeRecord(waybillModel, _rec, row, col);
        } else if(sc) changeRecord(waybillModel, _rec, row, col);

        if(_row == waybillModel->rowCount())
            return addNewWaybill(row, col);
    }
    } else return addNewWaybill(row, col);
}
int Importer::findWorkId(int row, int col)
{
    int _row = 0;
    QSqlRecord _rec;

    workModel->setFilter("DateR"+doc.cellAt(row, col)->value().toString());
    if(workModel->rowCount() > 0) {
    for(int i=0; i<workModel->rowCount(); i++){
        _rec = workModel->record(i);
        if(_rec.value("DateS").toString() == doc.cellAt(row, col)->value().toString()){//4
            if(_rec.value("DeviceSerial").toInt() == doc.cellAt(row, col)->value().toInt()){//5
                return _rec.value("WorkId").toInt();
            } else if(!sc) emit difference(workModel, _rec, _rec);
              else if(sc) changeRecord(workModel, _rec, row, col);
        } else if(sc) changeRecord(workModel, _rec, row, col);

        if(_row == workModel->rowCount())
            return addNewWork(row, col);
    }
    } else return addNewWork(row, col);
}
int Importer::findRequestId(int row, int col)
{
    int _row = 0;
    QSqlRecord _rec;

    requestModel->setFilter("Date"+doc.cellAt(row, col)->value().toString());
    if(requestModel->rowCount() > 0) {
    for(int i=0; i<requestModel->rowCount(); i++){
        _rec = requestModel->record(i);
        if(_rec.value("DateS").toString() == doc.cellAt(row, col)->value().toString()){//4
            if(clientModel->data(clientModel->index(_rec.value("Lastname").toInt(),2)).toString() ==
                    doc.cellAt(row, col)->value().toString()){//3
                return _rec.value("RequestId").toInt();
            } else if(!sc) emit difference(requestModel, _rec, _rec);
              else if(sc) changeRecord(requestModel, _rec, row, col);
        } else if(sc) changeRecord(requestModel, _rec, row, col);

        if(_row == requestModel->rowCount())
            return addNewRequest(row, col);
    }
    } else return addNewRequest(row, col);
}

int Importer::changeRecord(QSqlTableModel* model, QSqlRecord rec, int row, int col)
{
    //функция перезаписи данных в БД
    if(rec.isEmpty()) return 0;

    for(int i=0; i<model->columnCount(); i++,col++) {
        rec.setValue(i, doc.cellAt(row, col)->value());
    }
    if(model->setRecord(rec.value(0).toInt(), rec))
        return rec.value(0).toInt();//возможна ошибка. запись идет
                          //по названиям полей, а не по индексу
}

int Importer::addNewDevType(int row, int col)
{
    int id = generateDevTypeId();

    QSqlField devT1("TypeId", QVariant::Int);
    QSqlField devT2("Type", QVariant::String);

    devT1.setValue(id);
    devT2.setValue(doc.cellAt(row, col++)->value().toString());

    deviceType.append(devT1);
    deviceType.append(devT2);

    if (typeModel->insertRecord(-1, deviceType))
        return id;
    else return -1;
}
int Importer::addNewDepart(int row, int col)
{
    int id = generateDepartId();
    department.clear();

    QSqlField dep1("DepartmentId", QVariant::Int);
    QSqlField dep2("Name", QVariant::String);
    QSqlField dep3("Address", QVariant::String);
    QSqlField dep4("Accessories", QVariant::String);

    dep1.setValue(id);
    dep2.setValue(doc.cellAt(row, col++)->value().toString());
    dep3.setValue(doc.cellAt(row, col++)->value().toString());
    dep4.setValue(doc.cellAt(row, col)->value().toString());

    department.append(dep1);
    department.append(dep2);
    department.append(dep3);
    department.append(dep4);

    if (departModel->insertRecord(-1, department))
        return id;
    else return -1;
}
int Importer::addNewDevice(int row, int col)
{
    int id = generateDeviceId();
    device.clear();

    QSqlField dev1("Serial", QVariant::Int);
    QSqlField dev2("Type",QVariant::Int);
    QSqlField dev3("State",QVariant::Int);
    QSqlField dev4("Produced",QVariant::String);
    QSqlField dev5("Varranty",QVariant::Int);
    QSqlField dev6("Description",QVariant::String);
    QSqlField dev7("DateVar",QVariant::String);
    QSqlField dev8("Department",QVariant::Int);

    dev1.setValue(id);
    dev2.setValue(doc.cellAt(row, col++)->value().toInt());
    dev3.setValue(doc.cellAt(row, col++)->value().toInt());
    dev4.setValue(doc.cellAt(row, col++)->value().toString());
    dev5.setValue(doc.cellAt(row, col++)->value().toInt());
    dev6.setValue(doc.cellAt(row, col++)->value().toString());
    dev7.setValue(doc.cellAt(row, col++)->value().toString());
    dev8.setValue(doc.cellAt(row, col++)->value().toInt());

    device.append(dev1);
    device.append(dev2);
    device.append(dev3);
    device.append(dev4);
    device.append(dev5);
    device.append(dev6);
    device.append(dev7);
    device.append(dev8);

    if (deviceModel->insertRecord(-1, device))
        return id;
    else return -1;
}
int Importer::addNewClient(int row, int col)
{
    int id = generateClientId();
    client.clear();

    QSqlField cl1("ClientId", QVariant::Int);
    QSqlField cl2("Firstname", QVariant::String);
    QSqlField cl3("Lastname", QVariant::String);
    QSqlField cl4("Thirdname", QVariant::String);
    QSqlField cl5("Telefon", QVariant::String);
    QSqlField cl6("Email", QVariant::String);
    QSqlField cl7("DepartmentName", QVariant::Int);

    cl1.setValue(id);
    cl2.setValue(doc.cellAt(row, col++)->value().toString());
    cl3.setValue(doc.cellAt(row, col++)->value().toString());
    cl4.setValue(doc.cellAt(row, col++)->value().toString());
    cl5.setValue(doc.cellAt(row, col++)->value().toString());
    cl6.setValue(doc.cellAt(row, col++)->value().toString());
    cl7.setValue(doc.cellAt(row, col++)->value().toInt());

    client.append(cl1);
    client.append(cl2);
    client.append(cl3);
    client.append(cl4);
    client.append(cl5);
    client.append(cl6);
    client.append(cl7);

    if (clientModel->insertRecord(-1, client))
        return id;
    else return -1;
}
int Importer::addNewWaybill(int row, int col)
{
    int id = generateWaybillId();
    waybill.clear();

    QSqlField wb1("Serial", QVariant::Int);
    QSqlField wb2("Date", QVariant::String);
    QSqlField wb3("DepartmentName", QVariant::Int);
    QSqlField wb4("DepartmentAddress", QVariant::Int);
    QSqlField wb5("DepartmentAccessories", QVariant::Int);
    QSqlField wb6("ClientFirstname", QVariant::Int);
    QSqlField wb7("ClientLastname", QVariant::Int);
    QSqlField wb8("ClientThirdname", QVariant::Int);
    QSqlField wb9("ClientTelefon", QVariant::Int);
    QSqlField wb10("ClientEmail", QVariant::Int);

    wb1.setValue(id);
    wb2.setValue(doc.cellAt(row, col++)->value().toString());
    wb3.setValue(doc.cellAt(row, col++)->value().toInt());
    wb4.setValue(doc.cellAt(row, col++)->value().toInt());
    wb5.setValue(doc.cellAt(row, col++)->value().toInt());
    wb6.setValue(doc.cellAt(row, col++)->value().toInt());
    wb7.setValue(doc.cellAt(row, col++)->value().toInt());
    wb8.setValue(doc.cellAt(row, col++)->value().toInt());
    wb9.setValue(doc.cellAt(row, col++)->value().toInt());
    wb10.setValue(doc.cellAt(row, col++)->value().toInt());

    waybill.append(wb1);
    waybill.append(wb2);
    waybill.append(wb3);
    waybill.append(wb4);
    waybill.append(wb5);
    waybill.append(wb6);
    waybill.append(wb7);
    waybill.append(wb8);
    waybill.append(wb9);
    waybill.append(wb10);

    if (waybillModel->insertRecord(-1, waybill))
        return id;
    else return -1;
}
int Importer::addNewWork(int row, int col)
{
    int id = generateRequestId();
    request.clear();

    QSqlField rq1("RequestId", QVariant::Int);
    QSqlField rq2("Date", QVariant::String);
    QSqlField rq3("ClientFirstname", QVariant::Int);
    QSqlField rq4("ClientLastname", QVariant::Int);
    QSqlField rq5("ClientThirdname", QVariant::Int);
    QSqlField rq6("DepartmentName", QVariant::Int);
    QSqlField rq7("WorkId", QVariant::Int);

    rq1.setValue(id);
    rq2.setValue(doc.cellAt(row, col++)->value().toString());
    rq3.setValue(doc.cellAt(row, col++)->value().toString());
    rq4.setValue(doc.cellAt(row, col++)->value().toString());
    rq5.setValue(doc.cellAt(row, col++)->value().toString());
    rq6.setValue(doc.cellAt(row, col++)->value().toInt());
    rq7.setValue(doc.cellAt(row, col++)->value().toInt());

    request.append(rq1);
    request.append(rq2);
    request.append(rq3);
    request.append(rq4);
    request.append(rq5);
    request.append(rq6);
    request.append(rq7);

    if (requestModel->insertRecord(-1, request))
        return id;
    else return -1;
}
int Importer::addNewRequest(int row, int col)
{
    int id = generateWorkId();
    work.clear();

    QSqlField wr1("WorkId", QVariant::Int);
    QSqlField wr2("Malfunctions", QVariant::String);
    QSqlField wr3("Description", QVariant::String);
    QSqlField wr4("DateR", QVariant::String);
    QSqlField wr5("DateS", QVariant::String);
    QSqlField wr6("DeviceSerial", QVariant::Int);
    QSqlField wr7("WaybillSerial", QVariant::Int);

    wr1.setValue(id);
    wr2.setValue(doc.cellAt(row, col++)->value().toString());
    wr3.setValue(doc.cellAt(row, col++)->value().toString());
    wr4.setValue(doc.cellAt(row, col++)->value().toString());
    wr5.setValue(doc.cellAt(row, col++)->value().toString());
    wr6.setValue(doc.cellAt(row, col++)->value().toInt());
    wr7.setValue(doc.cellAt(row, col++)->value().toInt());

    work.append(wr1);
    work.append(wr2);
    work.append(wr3);
    work.append(wr4);
    work.append(wr5);
    work.append(wr6);
    work.append(wr7);

    if (workModel->insertRecord(-1, work))
        return id;
    else return -1;
}

int Importer::generateDevTypeId()
{
    uniqueDTid += 1;
    return uniqueDTid;
}
int Importer::generateDepartId()
{
    uniqueDepid += 1;
    return uniqueDepid;
}
int Importer::generateDeviceId()
{
    uniqueDevid += 1;
    return uniqueDevid;
}
int Importer::generateClientId()
{
    uniqueClid += 1;
    return uniqueClid;
}
int Importer::generateWaybillId()
{
    uniqueWblid += 1;
    return uniqueWblid;
}
int Importer::generateWorkId()
{
    uniqueWid += 1;
    return uniqueWid;
}
int Importer::generateRequestId()
{
    uniqueRqid += 1;
    return uniqueRqid;
}
