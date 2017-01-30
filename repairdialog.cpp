#include "repairdialog.h"


RepairDialog::RepairDialog(const QSqlRecord &record, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    this->setWindowTitle(QObject::tr("Данные о ремонте"));

    SerialEdit->setText(record.value(QObject::tr("DeviceSerial")).toString());

    QString dateR = record.value(QObject::tr("DateR")).toString();
    int yr = dateR.section('.', 2, 2).toInt();
    int mr = dateR.section('.', 1, 1).toInt();
    int dr = dateR.section('.', 0, 0).toInt();
    RDateEdit->setDate(QDate(yr, mr, dr));

    QString dateS = record.value(QObject::tr("DateS")).toString();
    int ys = dateS.section('.', 2, 2).toInt();
    int ms = dateS.section('.', 1, 1).toInt();
    int ds = dateS.section('.', 0, 0).toInt();
    SDateEdit ->setDate(QDate(ys, ms, ds));

    MalfEdit->setPlainText(record.value(QObject::tr("Malfunctions")).toString());
    DescrEdit->setPlainText(record.value(QObject::tr("Description")).toString());


    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

RepairDialog::~RepairDialog()
{
}

QString RepairDialog::getSerial()
{
    return SerialEdit->text();
}

QString RepairDialog::getMalfunctions()
{
    return MalfEdit->toPlainText();
}

QString RepairDialog::getRDate()
{
    return RDateEdit->date().toString("dd.mm.yyyy");
}

QString RepairDialog::getSDate()
{
    return SDateEdit->date().toString("dd.mm.yyyy");
}

QString RepairDialog::getDescription()
{
    return DescrEdit->toPlainText();
}
