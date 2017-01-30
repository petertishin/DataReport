#include "devtypecontroldialog.h"

int uniqueDevTypeId;


DevTypeControlDialog::DevTypeControlDialog(const QString &dbname, QWidget *parent)
{
    setupUi(this);
    activeDb = dbname;

    //editBtn = new QPushButton(tr("Edit"));
    //deleteBtn = new QPushButton(tr("Delete"));

    //buttonBox->addButton(editBtn, QDialogButtonBox::ActionRole);
    //buttonBox->addButton(deleteBtn, QDialogButtonBox::ActionRole);

    //connect(newTypeBtn, SIGNAL(clicked()), this, SLOT(addDevType()));
    //connect(editBtn, SIGNAL(clicked()), this, SLOT(editDevType()));
    //connect(deleteBtn, SIGNAL(clicked()), this, SLOT(deletedevType()));

    setWindowTitle(tr("Тип устройств"));
}

QSqlDatabase DevTypeControlDialog::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}


void DevTypeControlDialog::done(int result)
{
    QDialog::done(result);
}

void DevTypeControlDialog::addDevType()
{
    QSqlRecord record;

    //int id = generateDevTypeId();

    //QSqlField f1("");
    ///QSqlField f1("");
    //QSqlField f1("");

    //f1.setValue(QVariant(id));
    //f1.setValue(QVariant());
    //f1.setValue(QVariant());
}

void DevTypeControlDialog::editDevType()
{

}

void DevTypeControlDialog::deletedevType()
{

}
