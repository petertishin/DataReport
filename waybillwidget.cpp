#include "waybillwidget.h"
#include "repairdialog.h"
#include "departmentdialog.h"
#include "clientdialog.h"
#include "xlsxdocument.h"

#include <QtSql>
#include <QtWidgets>
#include <QSqlRecord>

int uniqueWaybillId;

//Добавить запись данных в таблицу Work
WayBillWidget::WayBillWidget(const QString &dbname, const QStringList &tbls, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    saved = false;

    filter = "";
    activeDb = dbname;
    tables = tbls;

    model = new QSqlRelationalTableModel(this, currentDatabase());
    model->setTable(tables.at(4));

    departModel = new QSqlTableModel(this, currentDatabase());
    departModel->setTable(tables.at(0));
    departModel->select();
    departBox->setModel(departModel);
    departBox->setModelColumn(1);

    clientModel = new QSqlTableModel(this, currentDatabase());
    clientModel->setTable(tables.at(2));
    clientModel->select();
    clientModel->setFilter(tr("DepartmentName = %1").arg(departBox->currentIndex()));//вроде проверил
    clientBox->setModel(clientModel);
    clientBox->setModelColumn(1);

    workModel = new QSqlTableModel(this, currentDatabase());
    workModel->setTable(tables.at(5));
    workModel->select();

    connect(departBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeDepartInfo(int)));
    connect(clientBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeClientInfo(int)));

    changeDepartInfo(departBox->currentIndex());
    changeClientInfo(clientBox->currentIndex());

    connect(newClientBtn, SIGNAL(clicked()), this, SLOT(newClientBtn_clicked()));
    connect(newDepartBtn, SIGNAL(clicked()), this, SLOT(newDepartBtn_clicked()));
    connect(printExcelReport, SIGNAL(pressed()), this, SLOT(printExcelReport_clicked()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(okButton_clicked()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelButton_clicked()));
  //Причина нестабильности программы
    showTable(tables.at(5), filter);
}

WayBillWidget::WayBillWidget(const QSqlRecord &record, const QString &dbname, const QStringList &tbls,QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    saved = true;

    filter = record.value(QString("Serial")).toString();
    activeDb = dbname;
    tables = tbls;

    model = new QSqlRelationalTableModel(this, currentDatabase());
    model->setTable(tables.at(4));    

    departModel = new QSqlTableModel(this, currentDatabase());
    departModel->setTable(tables.at(0));
    departModel->select();
    departBox->setModel(departModel);
    departBox->setModelColumn(1);

    clientModel = new QSqlTableModel(this, currentDatabase());
    clientModel->setTable(tables.at(2));
    clientModel->select();
    clientModel->setFilter(tr("DepartmentName = %1").arg(departBox->currentIndex()));//вроде проверил
    clientBox->setModel(clientModel);
    clientBox->setModelColumn(2);

    workModel = new QSqlTableModel(this, currentDatabase());
    workModel->setTable(tables.at(5));
    workModel->select();

    connect(departBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeDepartInfo(int)));
    connect(clientBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeClientInfo(int)));

    serialNumEdit->setText(record.value(QString("Serial")).toString());
    departBox->setCurrentIndex(record.value(QString("DepartmentName")).toInt());
    clientBox->setCurrentIndex(record.value(QString("ClientFirstname")).toInt());

    changeDepartInfo(departBox->currentIndex());
    changeClientInfo(clientBox->currentIndex());


    connect(newClientBtn, SIGNAL(clicked()), this, SLOT(newClientBtn_clicked()));
    connect(newDepartBtn, SIGNAL(clicked()), this, SLOT(newDepartBtn_clicked()));
    connect(printExcelReport, SIGNAL(pressed()), this, SLOT(printExcelReport_clicked()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(okButton_clicked()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelButton_clicked()));
  //Причина нестабильности программы
    showTable(tables.at(5), filter);
}

WayBillWidget::~WayBillWidget()
{
}

void WayBillWidget::exec()
{

}

QSqlDatabase WayBillWidget::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}

void WayBillWidget::showTable(const QString &t, const QString &f)
{
    QSqlRelationalTableModel *model = new QSqlRelationalTableModel(table, currentDatabase());
    model->setEditStrategy(QSqlTableModel::OnRowChange);    
    model->setTable(currentDatabase().driver()->escapeIdentifier(t, QSqlDriver::TableName));
    model->setFilter(QString("WaybillSerial = " + f));

    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Неисправность"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Описание ремонта"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата приема"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Дата отгрузки"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("№ устройства"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("№ вх. накладной"));

    model->select();
    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    table->addAction(insertRowAction);
    table->addAction(copyRowAction);
    table->addAction(deleteRowAction);
    table->addAction(editRowAction);
    table->addAction(updateTableAction);
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

    connect(table->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged()));

}

void WayBillWidget::insertRow()
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
    saved = false;
    updateActions();
}

void WayBillWidget::deleteRow()
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
    saved = false;
    updateActions();
    qDebug()<<"Типа delete";
}

void WayBillWidget::copyRow()
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
    saved = false;
    updateActions();
}

void WayBillWidget::editRow()
{
    QSqlRelationalTableModel *model = qobject_cast<QSqlRelationalTableModel *>(table->model());
    if (!model)
        return;

    QModelIndexList currentSelection = table->selectionModel()->selectedIndexes();
    QSqlRecord record;
    qDebug()<<"WaybillWidget: curSelectionsSize = "<<currentSelection.size();
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0) {
            record = model->record(currentSelection.at(i).row());
            qDebug()<<i;
            continue;
        }
            emit statusMessage(tr("Failure editing."));
    }

    qDebug()<<"WaybillWidget: Надобы открыть диалоговое окно редактирования эл-тов таблицы";
    qDebug()<<"Типа emit openEditWidget(record);";

    RepairDialog dialog(record, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    /*{

        model->setData(currentSelection.)
        return;
    }*/

    updateActions();
}

void WayBillWidget::updateActions()
{
    showTable(tables.at(5), filter);
}

int WayBillWidget::generateWaybillId()
{
    uniqueWaybillId = model->rowCount();
    uniqueWaybillId += 1;
    return uniqueWaybillId;
}

bool WayBillWidget::save()
{
    QSqlRelationalTableModel *tmodel = qobject_cast<QSqlRelationalTableModel *>(table->model());
    QSqlTableModel *waybillModel = model->relationModel(4);
    QSqlRecord record;

    if(serialNumEdit->text()==NULL)
        generateWaybillId();
    QString serial = serialNumEdit->text();
    QString accesories = accesoriesEdit->toPlainText();
    QString date = dateEdit->text();
    //int type = TypeBox->currentIndex();
    //int state = StateBox->currentIndex();
    //int varranty = VarrantyBox->currentIndex();

    //if(serial.isEmpty() || datep.isEmpty() || datev.isEmpty()) {
    //    QString message(tr("Запись не возможна. Не все поля заполены."));
    //    QMessageBox::information(this,tr("Ошибка заполнения"), message);
    //} else {
        //поиск серийника
        //запись в существующий или новый
        //record.;
        //deviceModel->insertRecord(-1, record);


        if (!tmodel)
            return saved;
        else
            saved = tmodel->submitAll();
        return saved;
    //}
}

void WayBillWidget::changeDepartInfo(int)
{
    QSqlRecord record;
    record = departModel->record(departBox->currentIndex());
    addressEdit->setText(record.value("Address").toString());
    accesoriesEdit->setText(record.value("Accessories").toString());
    clientModel->setFilter(tr("DepartmentName = %1").arg(departBox->currentIndex()));
    clientBox->setCurrentIndex(0);
}

void WayBillWidget::changeClientInfo(int)
{
    QSqlRecord record;
    record = clientModel->record(clientBox->currentIndex());
    telephoneEdit->setText(record.value("Telefon").toString());
    emailEdit->setText(record.value("Email").toString());
}

void WayBillWidget::newDepartBtn_clicked()
{
    QSqlRecord record;
    QSqlField f1("Name", QVariant::String);
    QSqlField f2("Address", QVariant::String);
    QSqlField f3("Accessories", QVariant::String);
    f1.setValue(QVariant(departBox->currentText()));
    f2.setValue(QVariant(addressEdit->toPlainText()));
    f3.setValue(QVariant(accesoriesEdit->toPlainText()));
    record.append(f1);
    record.append(f2);
    record.append(f3);

    DepartmentDialog dialog(model, &record, this);
    dialog.setWindowTitle(QObject::tr("Добавить департамент"));
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    departBox->setCurrentIndex(dialog.getDepartId());
}

void WayBillWidget::newClientBtn_clicked()
{
    QSqlRecord record;
    record = clientModel->record(clientBox->currentIndex());

    ClientDialog dialog(model, record, activeDb, this);
    dialog.setWindowTitle(QObject::tr("Добавить представитель заказчика"));
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }
    //обновление модуля клиента и выставление его как аргумента
}

void WayBillWidget::printExcelReport_clicked()
{
    QString box = "B9";    //адрес ячейки, куда должна вписываться информация
    QSqlRecord rec = clientModel->record(clientBox->currentIndex());
    QXlsx::Document xlsx("../Акт приема-передачи.xlsx");

    xlsx.write("D1", departBox->currentText()); //DepartName
    xlsx.write("D3", clientBox->currentText() + " " + telephoneEdit->text()); //ФИО telephone
    xlsx.write("D6", serialNumEdit->text()); //waybill #
    xlsx.write("I6", dateEdit->text()); //date
    //Заполнение пунктов изделия, комплектации и их номеров
    //for(int i=0; i<table->) {
    //    xlsx.write(box, ""); //DeviceName
    //    box++;
    //}
    xlsx.write("H22", clientBox->currentText()+" "
               +rec.value("Firstname").toString().at(0)+"."
               +rec.value("Thirdname").toString().at(0)+".");
    xlsx.write("G24", clientBox->currentText()+" "
               +rec.value("Firstname").toString().at(0)+"."
               +rec.value("Thirdname").toString().at(0)+".");

    if(xlsx.saveAs("Накладная_"+ serialNumEdit->text() +".xlsx")) {
        emit statusMessage("Акт №"+ serialNumEdit->text() +" сформирован");;
    }
}


void WayBillWidget::okButton_clicked()
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

void WayBillWidget::cancelButton_clicked()
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
