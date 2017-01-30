#include <QtGui>
#include <QtSql>

#include "clientcontroldialog.h"

ClientControlDialog::ClientControlDialog(int id, QWidget *parent)
    : QDialog(parent)
{
    addBtn = new QPushButton(tr("Добавить"));
    delBtn = new QPushButton(tr("Удалить"));

    buttonBox->addButton(addBtn, QDialogButtonBox::ActionRole);
    buttonBox->addButton(delBtn, QDialogButtonBox::ActionRole);

    tableModel = new QSqlRelationalTableModel(this);
    tableModel->setTable("Client");
    tableModel->setRelation(6, QSqlRelation("Department", "DepartmentId", "Name"));
    tableModel->setSort("Lastname", Qt::AscendingOrder);
    tableModel->select();

    QSqlTableModel *relationModel = tableModel->relationModel("DepartmentId");
    departmentComboBox->setModel(relationModel);
    departmentComboBox->setModelColumn(relationModel->fieldIndex("Name"));

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tableModel);
    mapper->setItemDelegate(new QSqlRelationDelegate(this));
    mapper->addMapping(firstnameEdit, 1);
    mapper->addMapping(lastnameEdit, 2);
    mapper->addMapping(thirdnameEdit, 3);
    mapper->addMapping(departmentComboBox, 4);
    mapper->addMapping(telephoneEdit,5);
    mapper->addMapping(emailEdit,6);

    if (id != -1) {
        for (int row=0; row<tableModel->rowCount(); ++row) {
            QSqlRecord record = tableModel->recoed(row);
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
    connect(addBtn, SIGNAL(clicked()), this, SLOT(addClient()));
    connect(delBtn, SIGNAL(clicked()), this, SLOT(deleteClient()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
/*
    QHBoxLayout *topButtonLayout = new QHBoxLayout;
       topButtonLayout->setContentsMargins(20, 0, 20, 5);
       topButtonLayout->addStretch();
       topButtonLayout->addWidget(firstButton);
       topButtonLayout->addWidget(previousButton);
       topButtonLayout->addWidget(nextButton);
       topButtonLayout->addWidget(lastButton);
       topButtonLayout->addStretch();

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addLayout(topButtonLayout, 0, 0, 1, 3);
    mainLayout->addWidget(firstnameLabel, 1, 0);
    mainLayout->addWidget(firstnameEdit, 1, 1, 1, 2);
    mainLayout->addWidget(lastnameLabel, 2, 0);
    mainLayout->addWidget(lastnameEdit, 2, 1, 1, 2);
    mainLayout->addWidget(thirdnameLabel, 3, 0);
    mainLayout->addWidget(thirdnameEdit, 3, 1, 1, 2);
    mainLayout->addWidget(departmentLabel, 4, 0);
    mainLayout->addWidget(departmentComboBox, 4, 1, 1, 2);
    mainLayout->addWidget(groupBox, 5, 0, 3, 3);
    mainLayout->addWidget(buttonBox, 8, 0, 1, 3);
    mainLayout->setRowMinimumHeight(7, 10);
    mainLayout->setRowStretch(7, 1);
    mainLayout->setColumnStretch(2, 1);
    setLayout(mainLayout);
*/
    if (id == -1) {
        nextBtn->setFocus();
    } else {
        firstnameEdit->setFocus();
    }

    setWindowTitle(tr("Управление клиентами"));
}

void ClientControlDialog::done()
{
    mapper->submit();
    QDialog::done(result);
}

void ClientControlDialog::addClient()
{
    int row = mapper->currentIndex();
    mapper->submit();
    tableModel->insertRow(row);
    mapper->setCurrentIndex(row);

    firstnameEdit->clear();
    lastnameEdit->clear();
    thirdnameEdit->clear();
    telephoneEdit->clear();
    emailEdit->clear();
    firstnameEdit->setFocus();
}

void ClientControlDialog::deleteClient()
{
    int row = mapper->currentIndex();
    tableModel->removeRow(row);
    mapper->submit();
    mapper->setCurrentIndex(qMin(row, tableModel->rowCount() - 1));
}
