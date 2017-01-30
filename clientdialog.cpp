#include "departmentdialog.h"
#include "clientdialog.h"

int uniqueClientId;

ClientDialog::ClientDialog(QSqlRelationalTableModel *client, const QSqlRecord &clientData, const QString &dbname, QWidget *parent) :
    QDialog(parent)
{
    clientTable = client;
    activeDb = dbname;
    saved = false;
    uniqueClientId = clientTable->rowCount();
    setupUi(this);

    departModel = new QSqlTableModel(this, currentDatabase());
    departModel->setTable(currentDatabase().tables().at(0));
    departModel->select();

    comboBox->setModel(departModel);
    comboBox->setModelColumn(1);
    comboBox->setCurrentIndex(clientData.value("DepartmentName").toInt());

    FirstnameEdit->setText(clientData.value("Firstname").toString());
    qDebug()<<clientData.value("Firstname").toInt();
    LastnameEdit->setText(clientData.value("Lastname").toString());
    ThirdnameEdit->setText(clientData.value("Thirdname").toString());
    TelephoneEdit->setText(clientData.value("Telefon").toString());
    EmailEdit->setText(clientData.value("Email").toString());

    connect(newDepBtn, SIGNAL(clicked()), this, SLOT(createNewDepart()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(submit()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    connect(FirstnameEdit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
    connect(LastnameEdit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
    connect(ThirdnameEdit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
    connect(TelephoneEdit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
    connect(EmailEdit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(editingFinished()));
}

ClientDialog::~ClientDialog()
{
}

QSqlDatabase ClientDialog::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}

void ClientDialog::editingFinished()
{
    saved = false;
    saveButton->setEnabled(!saved);
}

bool ClientDialog::save()
{
    QString Firstname = FirstnameEdit->text();
    QString Lastname = LastnameEdit->text();
    //QString Thirdname = ThirdnameEdit->text();
    //QString Telephone = TelephoneEdit->text();
    //QString Email = EmailEdit->text();
    //int DepId = comboBox->currentIndex();

    if(Firstname.isEmpty() || Lastname.isEmpty()) {
        QString message(tr("Пожалуйста, заполните обязательные поля."));
        QMessageBox::information(this, tr("Добавить представитель заказчика"), message);
    } else {
        clientId = findClientId();
        saved = true;
        saveButton->setEnabled(!saved);
    }

    return saved;
}

void ClientDialog::submit()
{
    QString Firstname = FirstnameEdit->text();
    QString Lastname = LastnameEdit->text();
    //QString Thirdname = ThirdnameEdit->text();
    //QString Telephone = TelephoneEdit->text();
    //QString Email = EmailEdit->text();
    //int DepId = comboBox->currentIndex();

    if(Firstname.isEmpty() || Lastname.isEmpty()) {
        QString message(tr("Пожалуйста, заполните обязательные поля."));
        QMessageBox::information(this, tr("Добавить представитель заказчика"), message);
    } else {
        clientId = findClientId();//начало процесса записи через поиск
        saved = true;
        accept();
    }
}

void ClientDialog::createNewDepart()
{
    QSqlTableModel *departmentTable = clientTable->relationModel(6);
    DepartmentDialog *dialog = new DepartmentDialog(departmentTable,comboBox->currentIndex(), this);//добавить аргументов
    int accepted = dialog->exec();

    if (accepted == 1) {
        int departId = dialog->getDepartId();
        comboBox->setCurrentIndex(departId);
    }

}

int ClientDialog::findClientId()
{
    int row = 0;

    while (row < clientTable->rowCount()) {
        QSqlRecord rec = clientTable->record(row);
        if (rec.value("Firstname").toString() == FirstnameEdit->text() &&
            rec.value("Lastname").toString() == LastnameEdit->text() &&
            rec.value("Thirdname").toString() == ThirdnameEdit->text() &&
            rec.value("Department").toInt() == comboBox->currentIndex())
            return rec.value("ClientId").toInt();
        else
            row++;
    }
    return addNewClient();
}

int ClientDialog::addNewClient()
{
    QSqlRecord record;

    int id = generateClientId();

    QSqlField f1("ClientId", QVariant::Int);
    QSqlField f2("Lastname", QVariant::String);
    QSqlField f3("Firstname", QVariant::String);
    QSqlField f4("Thirdname", QVariant::String);
    QSqlField f5("Telefon", QVariant::String);
    QSqlField f6("Email", QVariant::String);
    QSqlField f7("DepartmentName", QVariant::Int);

    f1.setValue(QVariant(id));
    f2.setValue(QVariant(LastnameEdit->text()));
    f3.setValue(QVariant(FirstnameEdit->text()));
    f4.setValue(QVariant(ThirdnameEdit->text()));
    f5.setValue(QVariant(TelephoneEdit->text()));
    f6.setValue(QVariant(EmailEdit->text()));
    f7.setValue(QVariant(comboBox->currentIndex()));

    record.append(f1);
    record.append(f2);
    record.append(f3);
    record.append(f4);
    record.append(f5);
    record.append(f6);
    record.append(f7);

    clientTable->insertRecord(-1, record);
    return id;
}

int ClientDialog::generateClientId()
{
    uniqueClientId += 1;
    return uniqueClientId;
}

QString ClientDialog::getFirstname()
{
    return FirstnameEdit->text();
}

QString ClientDialog::getLastName()
{
    return LastnameEdit->text();
}

QString ClientDialog::getThirdname()
{
    return ThirdnameEdit->text();
}

QString ClientDialog::getTelephone()
{
    return TelephoneEdit->text();
}

QString ClientDialog::getEmail()
{
    return EmailEdit->text();
}

int ClientDialog::getDepartName()
{
    return comboBox->currentIndex();
}
