#include "accessdialog.h"

AccessDialog::AccessDialog(QSqlTableModel *table, QSqlRecord recordO,
                           QSqlRecord recordN, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    model = table;
    recO = recordO;
    recN = recordN;

    QLabel *label1;
    QLabel *label2;

    verticalLayout->addWidget(label1);
    verticalLayout_2->addWidget(label2);

    for(int i=0; i<table->columnCount(); i++) {
        label1->setText(label1->text()+recordO.value(i).toString()+'\n');
        label2->setText(label2->text()+recordN.value(i).toString()+'\n');
    }

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(submit()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

}

AccessDialog::~AccessDialog()
{
}

void AccessDialog::submit()
{
    recN.setValue(0,recO.value(0));
    if(model->setRecord(recN.value(0).toInt(),recN))
        accept();
}

bool AccessDialog::getSC()
{
    return checkBox->isChecked();
}
