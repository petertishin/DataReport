#include "devicebrowser.h"
#include "devicewidget.h"
#include "qsqlconnectiondialog.h"

#include <QtWidgets>
#include <QtSql>

DeviceBrowser::DeviceBrowser(const QString &dbname, const QStringList &tbls, QWidget *parent) :
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

    //connect(createBtn, SIGNAL(clicked()), this, SLOT(creatorDialog()));
    //connect(modifyBtn, SIGNAL(clicked()), this, SLOT(modifyDialog()));
    //connect(newRowBtn, SIGNAL(clicked()), this, SLOT(on_insertRowAction_triggered()));
    /*
    if (QSqlDatabase::drivers().isEmpty())
        QMessageBox::information(this, tr("No database drivers found"),
                                 tr("This demo requires at least one Qt database driver. "
                                    "Please check the documentation how to build the "
                                    "Qt SQL plugins."));
    */

    showTable(tables.at(3));

    emit statusMessage(tr("Ready."));
}

DeviceBrowser::~DeviceBrowser()
{
}

void DeviceBrowser::exec()
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

QSqlDatabase DeviceBrowser::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}

void DeviceBrowser::showTable(const QString &t)
{
    // Первичный образ оображения таблицы
    QSqlRelationalTableModel *model = new QSqlRelationalTableModel(table, currentDatabase());
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->setTable(currentDatabase().driver()->escapeIdentifier(t, QSqlDriver::TableName));

    model->setRelation(1, QSqlRelation("Type", "TypeId", "Type"));
    model->setRelation(2, QSqlRelation("State", "StateId", "State"));
    model->setRelation(4, QSqlRelation("Varranty", "VarrantyId", "Varranty"));

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Серийный №"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Устройство"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Состояние"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Произведено"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Состояние гарантии"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Причина списания"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Гарантия до"));

    model->select();
    table->setShowGrid(false);
    table->setAlternatingRowColors(true);
    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    table->setModel(model);
    table->resizeColumnToContents(0);
    table->resizeColumnToContents(1);
    table->resizeColumnToContents(2);
    table->resizeColumnToContents(3);
    table->resizeColumnToContents(4);
    table->resizeColumnToContents(5);
    table->resizeColumnToContents(6);

    connect(table->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged()));
    updateActions();
}

void DeviceBrowser::insertRow()
{
    QSqlRelationalTableModel *model = qobject_cast<QSqlRelationalTableModel *>(table->model());
    if (!model)
        return;

    QModelIndex insertIndex = table->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    model->insertRow(row);
    insertIndex = model->index(row, 0);
    table->setCurrentIndex(insertIndex);
    //table->edit(insertIndex);
    model->submitAll();
}

void DeviceBrowser::deleteRow()
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

//придумать "вставку копией" с икрементацией серийного номера последней записи
void DeviceBrowser::copyRow()
{
    QSqlRelationalTableModel *model = qobject_cast<QSqlRelationalTableModel *>(table->model());
    if (!model)
        return;

    int newSerial;
    QSqlRecord recordLast;
    QSqlRecord recordOrig;
    QModelIndexList currentSelection = table->selectionModel()->selectedIndexes();
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0) {
            recordOrig = model->record(currentSelection.at(i).row());
            continue;
        }
    }

    recordLast = model->record(model->rowCount());
    newSerial = recordLast.value("Serial").toInt()+1;
    recordOrig.setValue("Serial", QVariant(newSerial));

    QModelIndex insertIndex = table->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    model->insertRow(row);
    insertIndex = model->index(row, 0);
    table->setCurrentIndex(insertIndex);
    if(!model->setRecord(row, recordOrig))
        emit statusMessage(tr("Failure copy."));
    model->submitAll();
    updateActions();
}

void DeviceBrowser::editRow()
{
    QSqlRelationalTableModel *model = qobject_cast<QSqlRelationalTableModel *>(table->model());
    if (!model)
        return;

    QModelIndexList currentSelection = table->selectionModel()->selectedIndexes();
    qDebug()<<"DeviceBrowser: curSelectionsSize = "<<currentSelection.size();
    QSqlRecord record;
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0) {
            record = model->record(currentSelection.at(i).row());
            continue;
        }
            emit statusMessage(tr("Failure editing."));
    }

    qDebug()<<"DeviceBrowser: emit OpenEditWidget(record)";
    emit openEditWidget(record);
    //EditDevice *editWidget = new EditDevice(record, activeDb, tables);//***
    //проверка повторного открытия вкладки с тем же содержимым
    /*for(int i=0; i<=tabWidget->count(); i++)
        if(tabsNames.at(i) == record.value(QString("Serial")).toString())
        {
            tabWidget->setCurrentIndex(i);
        }else*/
    //        tabId = tabWidget->addTab(editWidget, record.value(QString("Serial")).toString());//***
    //if( tabId != tabWidget->currentIndex())//***
    //    tabWidget->setCurrentIndex(tabId);//***

    //connect(editWidget, SIGNAL(statusMessage(QString)), this, SLOT(sendStatusMessage(QString)));
    //connect(editWidget, SIGNAL(openNewTab(QSqlRecord)), this, SLOT(openNewTab(QSqlRecord)));
    //connect(editWidget, SIGNAL(closeTab()), this, SLOT(closeTab()));


    updateActions();
}

void DeviceBrowser::updateActions()
{
    QSqlRelationalTableModel * tm = qobject_cast<QSqlRelationalTableModel *>(table->model());
    bool enableUpd = tm;
    bool enableIns = tm;
    bool enableDel = enableIns && table->currentIndex().isValid();
    bool enableEdi = enableIns && table->currentIndex().isValid();
    bool enableCop = enableIns && table->currentIndex().isValid();

    updateTableAction->setEnabled(enableUpd);
    insertRowAction->setEnabled(enableIns);
    deleteRowAction->setEnabled(enableDel);
    editRowAction->setEnabled(enableEdi);
    copyRowAction->setEnabled(enableCop);

    submitDatabase();
}

void DeviceBrowser::submitDatabase()
{
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(table->model());
    if (tm)
        tm->submitAll();
}

void DeviceBrowser::sendStatusMessage(const QString &message)
{
    emit statusMessage(message);
}

void DeviceBrowser::openNewTab(const QSqlRecord &record)
{
    QString s = record.fieldName(0);
    if(s == "WorkId")
        emit statusMessage("WorkId request");
    else if(s == "RequestId")
        emit statusMessage("RequestId request");
    else if(s == "WaybillId")
        emit statusMessage("WaybillId request");
    else if(s == "ClientId")
        emit statusMessage("ClientId request");
    else if(s == "DepartmentId")
        emit statusMessage("DepartmentId request");
    else
        emit statusMessage("Failure OpenNewTab");

    /*
    for(int i=0; i<=tabWidget->count(); i++)
        if(tabsNames.at(i) == record.value(QString("Serial")).toString())
        {
            tabWidget->setCurrentIndex(i);
        }else
            tabId = tabWidget->addTab(page, record.value(QString("Serial")).toString());
    if( tabId != tabWidget->currentIndex())
        tabWidget->setCurrentIndex(tabId);
    */
}
