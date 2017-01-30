#include <QMessageBox>

#include "departclientviewer.h"
#include "clientcontroldialog.h"
#include "departcontroldialog.h"


DepartClientViewer::DepartClientViewer(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    addBtn = new QPushButton("Добавить");
    deleteBtn = new QPushButton("Удалить");
    editBtn = new QPushButton("Изменить");

    buttonBox->addButton(addBtn, QDialogButtonBox::ActionRole);
    buttonBox->addButton(deleteBtn, QDialogButtonBox::ActionRole);
    buttonBox->addButton(editBtn, QDialogButtonBox::ActionRole);

    connect(addBtn, SIGNAL(clicked()), this, SLOT(addDepartment()));
    connect(editBtn, SIGNAL(clicked()), this, SLOT(editClient()));
    connect(deleteBtn, SIGNAL(clicked()), this, SLOT(deleteDepartment()));

    setWindowTitle(tr("Обзор представителей заказчиков"));
}

DepartClientViewer::~DepartClientViewer()
{

}

//Перегрузка операции редактирования таблицы (открыть departControlDialog)
void QAbstractItemView::edit(const QModelIndex &index)
{
    DepartControlDialog dialog = new DepartControlDialog(this);
    if(dialog.exec() != QDialog::Accepted) {
        //обновление таблицы и выделение новосозданного объекта
        return;
    }
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
    departmentView->edit(index);//перегрузка функции(вызов редактора департамента)
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

    updateClientView();
    departmentView->setFocus();
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
    departmentModel->setTable("Department");
    departmentModel->setSort(1, Qt::AscendingOrder);
    departmentModel->setHeaderData(1, Qt::Horizontal, tr("Департамент"));
    departmentModel->setHeaderData(2, Qt::Horizontal, tr("Адрес"));
    departmentModel->setHeaderData(3, Qt::Horizontal, tr("Реквизиты"));
    departmentModel->select();

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
