#include "importer.h"

Importer::Importer(QObject *parent) :
    QObject(parent)
{
}

void Importer::requestWork(const QString fname, const QString dbname, const QStringList dblist)
{
    activeDb = dbname;
    QXlsx::Document xlsx(fname);

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

    readRecords(xlsx);
}

void Importer::abort()
{

}

QSqlDatabase Importer::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}

void Importer::readRecords(QXlsx::Document *doc)
{
    int row, col;
    while(!doc->cellAt(row, col)->value().toString().isNull()) {
        findDepartId(row, col);




        for(int i=0; i<departModel->rowCount(); i++) {
            if(departModel->record(i).value("Name").toString() = department->value(1).toString()){
                if(departModel->record(i).value("Address").toString() = department->value(1).toString()){
                    if(departModel->record(i).value("Accessories").toString() = department->value(1).toString())
                        break;
                    else
                } else
            } else departModel
        }

        /*
        deviceType
        client
        device
        waybill
        work
        request
        */
    }
}

int Importer::findDepartId(int row, int col)
{
    int _row = 0;

    departModel->setFilter("Name="+doc->cellAt(row, col)->value().toString());
    while(_row < departModel->rowCount()) {
        QSqlRecord _rec = departModel->record(_row);
        if(_rec.value("Name").toString() == doc->cellAt(row, col)->value().toString()) {
            if(_rec.value("Address").toString() == doc->cellAt(row, col)->value().toString() ||
                    _rec.value("Accessories").toString() == doc->cellAt(row, col)->value().toString()) {
                if(_rec.value("Accessories").toString() == doc->cellAt(row, col)->value().toString()) {
                    return _rec.value("DepartmentId").toInt();
                } else changeRecord()
            }
        }
    }
}

bool Importer::addNewDepart()
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

int Importer::generateDepartId()
{
    uniqueDepartId = departModel->rowCount();
    uniqueDepartId += 1;
    return uniqueDepartId;
}
