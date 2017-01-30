#include <QMessageBox>

#include "departclientviewer.h"
#include "clientcontroldialog.h"
#include "departcontroldialog.h"

/*
 *addDepart     появляется пустая строка, редактирование невозможно
 *deleteDepart  удаляется департамент и все привязанные клиенты
 *editDepart    ошибка
 *
 *addClient     ошибка
 *deleteClient  удаляется выделенный клиент
 *editClient    ошибка
*/
DepartClientViewer::DepartClientViewer(const QString &dbname, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    activeDb = dbname;
    createDepartmentPanel();
    createClientPanel();


    addDepartBtn = new QPushButton("Добавить департамент");
    deleteDepartBtn = new QPushButton("Удалить департамент");
    editDepartBtn = new QPushButton("Изменить");

    addClientBtn = new QPushButton("Добавить клиента");
    deleteClientBtn = new QPushButton("Удалить клиента");
    editClientBtn = new QPushButton("Изменить");

    buttonBoxTop->addButton(addDepartBtn, QDialogButtonBox::ActionRole);
    buttonBoxTop->addButton(deleteDepartBtn, QDialogButtonBox::ActionRole);
    buttonBoxTop->addButton(editDepartBtn, QDialogButtonBox::ActionRole);

    buttonBoxBtm->addButton(addClientBtn, QDialogButtonBox::ActionRole);
    buttonBoxBtm->addButton(deleteClientBtn, QDialogButtonBox::ActionRole);
    buttonBoxBtm->addButton(editClientBtn, QDialogButtonBox::ActionRole);

    connect(addDepartBtn, SIGNAL(clicked()), this, SLOT(addDepartment()));
    connect(editDepartBtn, SIGNAL(clicked()), this, SLOT(editDepartment()));
    connect(deleteDepartBtn, SIGNAL(clicked()), this, SLOT(deleteDepartment()));
    connect(addClientBtn, SIGNAL(clicked()), this, SLOT(addClient()));
    connect(editClientBtn, SIGNAL(clicked()), this, SLOT(editClient()));
    connect(deleteClientBtn, SIGNAL(clicked()), this, SLOT(deleteClient()));

    setWindowTitle(tr("Обзор представителей заказчиков"));
    departmentView->setCurrentIndex(departmentModel->index(0, 0));
}

DepartClientViewer::~DepartClientViewer()
{

}

QSqlDatabase DepartClientViewer::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}

void DepartClientViewer::updateClientView()
{
    QModelIndex index = departmentView->currentIndex();
    if (index.isValid()) {
       QSqlRecord record = departmentModel->record(index.row());
       int id = record.value("DepartmentId").toInt();
       clientModel->setFilter(QString("DepartmentName = %1").arg(id));
    } else {
        clientModel->setFilter("DepartmentName = -1");
    }
    clientModel->select();
    clientView->horizontalHeader()->setVisible(clientModel->rowCount() > 0);
}

void DepartClientViewer::addDepartment()
{
    int row = departmentModel->rowCount();
    departmentModel->insertRow(row);
    QModelIndex index = departmentModel->index(row, 1);
    departmentView->setCurrentIndex(index);
    //departmentView->edit(index);//перегрузка функции(вызов редактора департамента)
    //editDepartment();
}

void DepartClientViewer::deleteDepartment()
{
    QModelIndex index = departmentView->currentIndex();
    if (!index.isValid())
        return;

    QSqlDatabase::database().transaction();
    QSqlRecord record = departmentModel->record(index.row());
    int id = record.value(0).toInt();
    int numClients = 0;

    QSqlQuery query(QString("SELECT COUNT(*) FROM Client "
                            "WHERE DepartmentId = %1").arg(id));
    if (query.next())
        numClients = query.value(0).toInt();
    if (numClients > 0) {
        int r = QMessageBox::warning(this, tr("Удалить Департамент"),
                                     tr("Удалить %1 и всех клиентов с ним?")
                                     .arg(record.value(1).toString()),
                                     QMessageBox::Yes | QMessageBox::No);
        if (r == QMessageBox::No) {
            QSqlDatabase::database().rollback();
            return;
        }

        query.exec(QString("DELETE FROM Client "
                           "WHERE DepartmentId = %1").arg(id));
    }

    departmentModel->removeRow(index.row());
    departmentModel->submitAll();
    QSqlDatabase::database().commit();

    createDepartmentPanel();
    updateClientView();
    departmentView->setFocus();
}

void DepartClientViewer::editDepartment()
{
    int departId = -1;
    QModelIndex index = departmentView->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = departmentModel->record(index.row());
        departId = record.value(0).toInt();
    }
    qDebug()<<"index = "<<departId;
    DepartControlDialog dialog(departId, this);
    qDebug()<<"after dialog constructor";
    if(dialog.exec() != QDialog::Accepted) {
        //обновление таблицы и выделение новосозданного объекта
        createDepartmentPanel();
        departmentModel->selectRow(index.row());
        return;
    }
}

void DepartClientViewer::addClient()
{
    int row = clientModel->rowCount();
    clientModel->insertRow(row);
    QModelIndex index = clientModel->index(row, 1);
    clientView->setCurrentIndex(index);
    editClient();
}

void DepartClientViewer::deleteClient()
{
    QModelIndex index = clientView->currentIndex();
    if (!index.isValid())
        return;

    QSqlDatabase::database().transaction();
    QSqlRecord record = clientModel->record(index.row());

    clientModel->removeRow(index.row());
    clientModel->submitAll();
    QSqlDatabase::database().commit();

    updateClientView();
    clientView->setFocus();
}

void DepartClientViewer::editClient()
{
    int clientId = -1;
    QModelIndex index = clientView->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = clientModel->record(index.row());
        clientId = record.value(0).toInt();
    }

    ClientControlDialog dialog(clientId, this);
    dialog.exec();
    updateClientView();
}

void DepartClientViewer::createDepartmentPanel()
{
    departmentModel = new QSqlRelationalTableModel(departmentView, currentDatabase());
    departmentModel->setTable(currentDatabase().driver()->escapeIdentifier("Department", QSqlDriver::TableName));
    departmentModel->setSort(1, Qt::AscendingOrder);
    departmentModel->setHeaderData(1, Qt::Horizontal, tr("Департамент"));
    departmentModel->setHeaderData(2, Qt::Horizontal, tr("Адрес"));
    departmentModel->setHeaderData(3, Qt::Horizontal, tr("Реквизиты"));
    departmentModel->select();
    if (departmentModel->lastError().type() != QSqlError::NoError)
        emit statusMessage(departmentModel->lastError().text());

    departmentView->setModel(departmentModel);
    departmentView->setItemDelegate(new QSqlRelationalDelegate(this));
    departmentView->setSelectionMode(QAbstractItemView::SingleSelection);
    departmentView->setSelectionBehavior(QAbstractItemView::SelectRows);
    departmentView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    departmentView->setColumnHidden(0, true);
    departmentView->resizeColumnsToContents();
    departmentView->horizontalHeader()->setStretchLastSection(true);

    connect(departmentView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(updateClientView()));
}

void DepartClientViewer::createClientPanel()
{
    clientModel = new QSqlRelationalTableModel(clientView, currentDatabase());

    clientModel->setTable("Client");
    clientModel->setRelation(6, QSqlRelation("Department", "DepartmentId", "Name"));
    clientModel->setSort(2, Qt::AscendingOrder);
    clientModel->setHeaderData(1, Qt::Horizontal, tr("Имя"));
    clientModel->setHeaderData(2, Qt::Horizontal, tr("Фамилия"));
    clientModel->setHeaderData(3, Qt::Horizontal, tr("Отчество"));
    clientModel->setHeaderData(4, Qt::Horizontal, tr("Телефон"));
    clientModel->setHeaderData(5, Qt::Horizontal, tr("Email"));
    clientModel->select();

    clientView->setModel(clientModel);
    clientView->setSelectionMode(QAbstractItemView::SingleSelection);
    clientView->setSelectionBehavior(QAbstractItemView::SelectRows);
    clientView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    clientView->setColumnHidden(0,true);
    clientView->setColumnHidden(6,true);
    clientView->resizeColumnsToContents();

}
