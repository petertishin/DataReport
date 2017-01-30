#include <QtSql>

#include "departcontroldialog.h"

DepartControlDialog::DepartControlDialog(int id, QWidget *parent) :
    QDialog(parent)
{
    addBtn = new QPushButton(tr("Добавить"));
    delBtn = new QPushButton(tr("Удалить"));

    buttonBox->addButton(addBtn, QDialogButtonBox::ActionRole);
    buttonBox->addButton(delBtn, QDialogButtonBox::ActionRole);

    tableModel = new QSqlRelationalTableModel(this);
    tableModel->setTable("Department");
    tableModel->setSort(1, Qt::AscendingOrder);
    tableModel->select();

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tableModel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(nameEdit, 1);
    mapper->addMapping(addressEdit, 2);
    mapper->addMapping(accessoriesEdit, 3);

    if (id != -1) {
        for (int row=0; row<tableModel->rowCount(); ++row) {
            QSqlRecord record = tableModel->record(row);
            if (record.value(0).toInt() == id) {
                mapper->setCurrentIndex(row);
                break;
            }
        }
    } else {
        mapper->toFirst();
    }

    connect(firstBtn, SIGNAL(clicked()), mapper, SLOT(toFirst()));
    connect(previousBtn, SIGNAL(clicked()), mapper, SLOT(toPrevious()));
    connect(nextBtn, SIGNAL(clicked()), mapper, SLOT(toNext()));
    connect(lastBtn, SIGNAL(clicked()), mapper, SLOT(toLast()));
    connect(addBtn, SIGNAL(clicked()), this, SLOT(addDepart()));
    connect(delBtn, SIGNAL(clicked()), this, SLOT(deleteDepart()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    if (id == -1) {
        nextBtn->setFocus();
    } else {
        nameEdit->setFocus();
    }

    setWindowTitle(tr("Управление департаментами"));
}

void DepartControlDialog::done(int result)
{
    mapper->submit();
    QDialog::done(result);
}

void DepartControlDialog::addDepart()
{
    int row = mapper->currentIndex();
    mapper->submit();
    tableModel->insertRow(row);
    mapper->setCurrentIndex(row);

    nameEdit->clear();
    addressEdit->clear();
    accessoriesEdit->clear();
    nameEdit->setFocus();
}

void DepartControlDialog::deleteDepart()
{
    int row = mapper->currentIndex();
    tableModel->removeRow(row);
    mapper->submit();
    mapper->setCurrentIndex(qMin(row, tableModel->rowCount() - 1));
}
