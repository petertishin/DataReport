#include "devicewidget.h"
#include "repairdialog.h"
#include "departmentdialog.h"

#include <QtSql>
#include <QtWidgets>
#include <QSqlRecord>

DeviceWidget::DeviceWidget(const QSqlRecord &record, const QString &dbname, const QStringList &tbls, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    saved = false;

    filter = record.value(QString("Serial")).toString();
    activeDb = dbname;
    tables = tbls;

    model = new QSqlRelationalTableModel(this, currentDatabase());
    model->setTable(tables.at(3));

    QSqlTableModel *typeModel = new QSqlTableModel(this, currentDatabase());
    typeModel->setTable(tables.at(1));
    typeModel->select();
    TypeBox->setModel(typeModel);
    TypeBox->setModelColumn(1);

    QSqlTableModel *stateModel = new QSqlTableModel(this, currentDatabase());
    stateModel->setTable(tables.at(7));
    stateModel->select();
    StateBox->setModel(stateModel);
    StateBox->setModelColumn(1);

    QSqlTableModel *varModel = new QSqlTableModel(this, currentDatabase());
    varModel->setTable(tables.at(8));
    varModel->select();
    VarrantyBox->setModel(varModel);
    VarrantyBox->setModelColumn(1);

    SerNumText->setValue(record.value(QString("Serial")).toInt());
    TypeBox->setCurrentIndex(record.value(QString("Type")).toInt());
    StateBox->setCurrentIndex(record.value(QString("State")).toInt());
    VarrantyBox->setCurrentIndex(record.value(QString("Varranty")).toInt());
    DepartmentBox->setCurrentIndex(record.value(QString("Department")).toInt());

    QString datep = record.value(QString("Produced")).toString();
    int yp = datep.section('.', 2, 2).toInt();
    int mp = datep.section('.', 1, 1).toInt();
    int dp = datep.section('.', 0, 0).toInt();
    ProducedText->setDate(QDate(yp, mp, dp));
    QString datev = record.value(QString("DateVar")).toString();
    int yv = datev.section('.', 2, 2).toInt();
    int mv = datev.section('.', 1, 1).toInt();
    int dv= datev.section('.', 0, 0).toInt();
    DateVarText->setDate(QDate(yv, mv, dv));

    if(DateVarText->date() > QDate::currentDate())
        VarrantyBox->setCurrentIndex(0);

    connect(newDepBtn, SIGNAL(pressed()), this, SLOT(newDepartment()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(okButton_clicked()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelButton_clicked()));

    showTable(tables.at(5), filter);

    emit statusMessage(tr("Ready."));
}

DeviceWidget::~DeviceWidget()
{
}

void DeviceWidget::exec()
{

}

QSqlDatabase DeviceWidget::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}

void DeviceWidget::showTable(const QString &t, const QString &f)
{
    QSqlRelationalTableModel *model = new QSqlRelationalTableModel(table, currentDatabase());
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->setTable(currentDatabase().driver()->escapeIdentifier(t, QSqlDriver::TableName));
    model->setFilter(QString("DeviceSerial = " + f));
    qDebug()<<"DeviceSerial = " + f;
    //model->setRelation(5, QSqlRelation("Device", "DeviceSerial", "Serial"));
    //model->setRelation(6, QSqlRelation("Waybill", "WaybillId", "Serial"));

    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Неисправность"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Описание ремонта"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата приема"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Дата отгрузки"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("№ устройства"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("№ накладной"));

    model->select();
    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());

    table->addAction(insertRowAction);
    table->addAction(deleteRowAction);
    table->addAction(editRowAction);

    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setAlternatingRowColors(true);
    table->setModel(model);
    table->hideColumn(0);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->resizeColumnToContents(3);
    table->resizeColumnToContents(4);
    table->resizeColumnToContents(5);
    table->resizeColumnToContents(6);
    table->setItemDelegate(new QSqlRelationalDelegate(table));

    connect(table->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged()));
    updateActions();
}

void DeviceWidget::insertRow()
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
    saved = false;
    model->submitAll();
}

void DeviceWidget::deleteRow()
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
    saved = false;
    updateActions();
}

void DeviceWidget::editRow()
{
    QSqlRelationalTableModel *model = qobject_cast<QSqlRelationalTableModel *>(table->model());
    if (!model)
        return;

    QModelIndexList currentSelection = table->selectionModel()->selectedIndexes();
    qDebug()<<"DeviceWidget: curSelectionsSize = "<<currentSelection.size();
    QSqlRecord record;
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0) {
            record = model->record(currentSelection.at(i).row());
            continue;
        }
            emit statusMessage(tr("Failure editing."));
    }
    saved = false;

    qDebug()<<"EditDevice: Надобы открыть диалоговое окно редактирования эл-тов таблицы";
    RepairDialog dialog(record, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    //emit openNewTab(record);
    /*
    // Отправка сигнала с QRecord главному виджету для редактирования записи
    EditDevice *EditDevice = new EditDevice(record, activeDb, tables);
    tabId = tabWidget->addTab(EditDevice, record.value(QString("Serial")).toString());
    if( tabId != tabWidget->currentIndex())
        tabWidget->setCurrentIndex(tabId);
    connect(EditDevice, SIGNAL(statusMessage(QString)), this, SLOT(sendStatusMessage(QString)));
    connect(EditDevice, SIGNAL(closeTab()), this, SLOT(closeTab()));
    */

    updateActions();
}

void DeviceWidget::newDepartment()
{
    DepartmentDialog dialog(model, DepartmentBox->currentIndex(), this);
    if(dialog.exec() != QDialog::Accepted)
        return;

}

void DeviceWidget::updateActions()
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
}

bool DeviceWidget::save()
{
    QSqlRelationalTableModel *tmodel = qobject_cast<QSqlRelationalTableModel *>(table->model());
    QSqlTableModel *deviceModel = model->relationModel(3);
    QSqlRecord record;

    QString serial = SerNumText->text();
    QString datep = ProducedText->text();
    QString datev = DateVarText->text();
    QString description = DescriptionEdit->text();
    int type = TypeBox->currentIndex();
    int state = StateBox->currentIndex();
    int varranty = VarrantyBox->currentIndex();
    int department = DepartmentBox->currentIndex();

    if(serial.isEmpty() || datep.isEmpty() || datev.isEmpty()) {
        QString message(tr("Запись не возможна. Не все поля заполены."));
        QMessageBox::information(this,tr("Ошибка заполнения"), message);
    } else {
        //поиск серийника
        //запись в существующий или новый
        //record.;
        //deviceModel->insertRecord(-1, record);


        if (!tmodel)
            return saved;
        else
            saved = tmodel->submitAll();
        return saved;
    }

}

void DeviceWidget::okButton_clicked()
{
    if(!saved) {
        if(save())
            emit closeTab();
        else
            QMessageBox::warning(this, tr("DataReport"),
                                 tr("Unable to save data."),
                                 QMessageBox::Ok);
    } else
        emit closeTab();
}

void DeviceWidget::cancelButton_clicked()
{
     if(!saved) {
         int ret = QMessageBox::question(this, tr("DataReport"),
                                         tr("Данные были изменены.\n"
                                            "Сохранить изменения перед тем, как закрыть вкладку?"),
                                         QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                         QMessageBox::Save);

         switch (ret) {
           case QMessageBox::Save:
               if(save())
                   emit closeTab();
               break;
           case QMessageBox::Discard:
               emit closeTab();
               break;
           case QMessageBox::Cancel:
               // Cancel was clicked
               break;
           default:
               break;
         }
     } else
         emit closeTab();
}
