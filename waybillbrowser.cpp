#include "waybillbrowser.h"
#include "devicewidget.h"
#include "qsqlconnectiondialog.h"

#include <QtWidgets>
#include <QtSql>
//добавить в функцию копирования замену ID на пустоту
WaybillBrowser::WaybillBrowser(const QString &dbname, const QStringList &tbls, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    activeDb = dbname;
    tables = tbls;

    table->addAction(updateTableAction);
    table->addAction(insertRowAction);
    table->addAction(deleteRowAction);
    table->addAction(copyRowAction);
    table->addAction(editRowAction);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);


    showTable(tables.at(4));

    emit statusMessage(tr("Waybill/'s Browser Ready."));
}

WaybillBrowser::~WaybillBrowser()
{
}

void WaybillBrowser::exec()
{
    QSqlQueryModel *model = new QSqlQueryModel(table);

    QString query;
    query = "SELECT id";
    if(SerNumCB->isChecked()) query.append(", Serial");
    if(TitleCB->isChecked()) query.append(", Title");
    if(CityCB->isChecked()) query.append(", City");
    if(FirstnameCB->isChecked()) query.append(", Firstname");
    if(LastnameCB->isChecked()) query.append(", Lastname");
    if(TelefonCB->isChecked()) query.append(", Telefon");
    if(ProducedCB->isChecked()) query.append(", Produced");
    if(StateCB->isChecked()) query.append(", State");
    query.append("\n FROM Devices\n");
    //query.append("ORDER BY ", OrderBox->currentText());
    if(FilterLine1->isModified())
    {
        //FilterLine2->isEnabled(true);
        query.append("WHERE ");
        query.append(FilterLine1->text());
        if(FilterLine2->isModified())
        {
            query.append(FilterLine1->text());
        }
    }
    model->setQuery(QSqlQuery(query, currentDatabase()));

    table->setModel(model);
    table->setItemDelegate(new QSqlRelationalDelegate(table));

    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    else if (model->query().isSelect())
        emit statusMessage(tr("Query OK."));
    else
        emit statusMessage(tr("Query OK, number of affected rows: %1").arg(
                           model->query().numRowsAffected()));

    updateActions();
}

QSqlDatabase WaybillBrowser::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}

void WaybillBrowser::showTable(const QString &t)
{
    // Первичный образ оображения таблицы
    QSqlRelationalTableModel *model = new QSqlRelationalTableModel(table, currentDatabase());
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->setTable(currentDatabase().driver()->escapeIdentifier(t, QSqlDriver::TableName));
    model->setRelation(2, QSqlRelation("Department", "DepartmentId", "Name"));
    model->setRelation(3, QSqlRelation("Department", "DepartmentId", "Address"));
    model->setRelation(4, QSqlRelation("Department", "DepartmentId", "Accessories"));
    model->setRelation(5, QSqlRelation("Client", "ClientId", "Lastname"));
    model->setRelation(6, QSqlRelation("Client", "ClientId", "Firstname"));
    model->setRelation(7, QSqlRelation("Client", "ClientId", "Thirdname"));
    model->setRelation(8, QSqlRelation("Client", "ClientId", "Telefon"));
    model->setRelation(9, QSqlRelation("Client", "ClientId", "Email"));


    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Серийный №"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Дата"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Подразделение"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Адрес"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Реквизиты"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Фамилия"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Имя"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Отчество"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("Телефон"));
    model->setHeaderData(9, Qt::Horizontal, QObject::tr("Email"));
    model->select();
    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    table->setModel(model);
    //table->hideColumn(4);
    //table->hideColumn(8);

    connect(table->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged()));
    updateActions();
}

void WaybillBrowser::insertRow()
{
    QSqlRelationalTableModel *model = qobject_cast<QSqlRelationalTableModel *>(table->model());
    if (!model)
        return;

    QModelIndex insertIndex = table->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    model->insertRow(row);
    insertIndex = model->index(row, 0);
    table->setCurrentIndex(insertIndex);
    table->edit(insertIndex);
    model->submitAll();
    updateActions();
}

void WaybillBrowser::deleteRow()
{
    QSqlRelationalTableModel *model = qobject_cast<QSqlRelationalTableModel *>(table->model());
    if (!model)
        return;

    QModelIndexList currentSelection = table->selectionModel()->selectedIndexes();
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0)
            continue;
        model->removeRow(currentSelection.at(i).row());
    }
    model->submitAll();

    updateActions();
}

void WaybillBrowser::copyRow()
{
    QSqlRelationalTableModel *model = qobject_cast<QSqlRelationalTableModel *>(table->model());
    if (!model)
        return;

    QSqlRecord record;
    QModelIndexList currentSelection = table->selectionModel()->selectedIndexes();
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0)
            record = model->record(currentSelection.at(i).row());
            continue;
    }

    QModelIndex insertIndex = table->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    model->insertRow(row);
    insertIndex = model->index(row, 0);
    table->setCurrentIndex(insertIndex);
    //table->edit(insertIndex);
    if(!model->setRecord(row, record))
        emit statusMessage(tr("Failure copy."));
    model->submitAll();
    updateActions();
}

void WaybillBrowser::editRow()
{
    QSqlRelationalTableModel *model = qobject_cast<QSqlRelationalTableModel *>(table->model());
    if (!model)
        return;

    QModelIndexList currentSelection = table->selectionModel()->selectedIndexes();
    qDebug()<<"WaybillBrowser: curSelectionsSize = "<<currentSelection.size();
    QSqlRecord record;
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0) {
            record = model->record(currentSelection.at(i).row());
            qDebug()<<i;
            continue;
        }
            emit statusMessage(tr("Failure editing."));
    }

    qDebug()<<"WaybillBrowser: emit openWaybillWidget(record)";
    emit openEditWidget(record);

    updateActions();
}

void WaybillBrowser::updateActions()
{
    QSqlRelationalTableModel * tm = qobject_cast<QSqlRelationalTableModel *>(table->model());
    bool enableIns = tm;
    bool enableDel = enableIns && table->currentIndex().isValid();
    bool enableEdi = enableIns && table->currentIndex().isValid();
    bool enableCop = enableIns && table->currentIndex().isValid();

    insertRowAction->setEnabled(enableIns);
    deleteRowAction->setEnabled(enableDel);
    editRowAction->setEnabled(enableEdi);
    copyRowAction->setEnabled(enableCop);

    submitAction->setEnabled(tm);
    revertAction->setEnabled(tm);
    selectAction->setEnabled(tm);
}

/*void WaybillBrowser::on_submitAction_triggered()
{
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(table->model());
    if (tm)
        tm->submitAll();
}*/

void WaybillBrowser::on_revertAction_triggered()
{
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(table->model());
    if (tm)
        tm->revertAll();
}

void WaybillBrowser::on_selectAction_triggered()
{
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(table->model());
    if (tm)
        tm->select();
}
