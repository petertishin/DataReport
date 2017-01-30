#include "departmentdialog.h"

int uniqueDepId;

DepartmentDialog::DepartmentDialog(QSqlTableModel *table, QSqlRecord *rec, QWidget *parent) :
    QDialog(parent)
{
    departTable = table;
    record = rec;
    setupUi(this);

    NameEdit->setText(record->value("Name").toString());
    AddressEdit->setText(record->value("Address").toString());
    AccessEdit->setText(record->value("Accessories").toString());

}

DepartmentDialog::DepartmentDialog(QSqlTableModel *table, int num = 0, QWidget *parent) :
    QDialog(parent)
{
    departTable = table;
    setupUi(this);

    QSqlRecord record;
    record = departTable->record(num);
    NameEdit->setText(record.value("Name").toString());
    AddressEdit->setText(record.value("Address").toString());
    AccessEdit->setText(record.value("Accessories").toString());

}

DepartmentDialog::~DepartmentDialog()
{
}

void DepartmentDialog::submit()
{
    QString depName = NameEdit->text();
    QString depAdr = AddressEdit->toPlainText();
    QString depAcs = AccessEdit->toPlainText();

    if(depName.isEmpty() || depAdr.isEmpty() || depAcs.isEmpty()) {
        QString message(tr("Пожжалуйста, заполните обязательные поля."));
        QMessageBox::information(this, tr("Добавить подразделение"), message);
    } else {
        int depId = findDepartId();

        accept();
    }
}

int DepartmentDialog::findDepartId()
{
    int row = 0;

    while (row < departTable->rowCount()) {
        QSqlRecord rec = departTable->record(row);
        if(rec.value("Name").toString() == NameEdit->text())
            return rec.value("DepartmmentId").toInt();
        else
            row++;
    }
    return addNewDepart();
}

int DepartmentDialog::addNewDepart()
{
    QSqlRecord record;

    int id = generateDepartId();

    QSqlField f1("DepartmentId", QVariant::Int);
    QSqlField f2("Name", QVariant::String);
    QSqlField f3("Address", QVariant::String);
    QSqlField f4("Accessories", QVariant::String);

    f1.setValue(QVariant(id));
    f2.setValue(QVariant(NameEdit->text()));
    f3.setValue(QVariant(AddressEdit->toPlainText()));
    f4.setValue(QVariant(AccessEdit->toPlainText()));

    record.append(f1);
    record.append(f2);
    record.append(f3);
    record.append(f4);

    departTable->insertRecord(-1, record);
    return id;
}

int DepartmentDialog::generateDepartId()
{
    uniqueDepId = departTable->rowCount();
    uniqueDepId += 1;
    return uniqueDepId;
}

int DepartmentDialog::getDepartId()
{
    return uniqueDepId;
}
