#include "qsqlconnectiondialog.h"
#include "ui_qsqlconnectiondialog.h"

#include <QtXml>
#include <QSqlDatabase>

QSqlConnectionDialog::QSqlConnectionDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
/*
    QDomDocument doc("utility");
    QFile file("utility.xml");
    if(!file.open(QIODevice::ReadWrite))
        return;
    if(!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            qDebug()<<qPrintable(e.tagName());
        }
        n = n.nextSibling();
    }
*/
    QStringList drivers = QSqlDatabase::drivers();
    QStringList logins;
    QStringList passwords;
    QStringList databases;

    // remove compat names
    drivers.removeAll("QMYSQL3");
    drivers.removeAll("QOCI8");
    drivers.removeAll("QODBC3");
    drivers.removeAll("QPSQL7");
    drivers.removeAll("QTDS7");

    if (!drivers.contains("QSQLITE"))
        ui.dbCheckBox->setEnabled(false);

    ui.comboDriver->addItems(drivers);
}

QSqlConnectionDialog::~QSqlConnectionDialog()
{
}

QString QSqlConnectionDialog::driverName() const
{
    return ui.comboDriver->currentText();
}

QString QSqlConnectionDialog::databaseName() const
{
    return ui.editDatabase->text();
}

QString QSqlConnectionDialog::userName() const
{
    return ui.editUsername->text();
}

QString QSqlConnectionDialog::password() const
{
    return ui.editPassword->text();
}

QString QSqlConnectionDialog::hostName() const
{
    return ui.editHostname->text();
}

int QSqlConnectionDialog::port() const
{
    return ui.portSpinBox->value();
}

bool QSqlConnectionDialog::createNewDatabase() const
{
    return ui.newdbCheckBox->isChecked();
}

bool QSqlConnectionDialog::useInMemoryDatabase() const
{
    return ui.dbCheckBox->isChecked();
}

/*QDomElement QSqlConnectionDialog::database(QDomDocument& domDoc,
                                           const QString& dbname,
                                           const QString& dbdriver//,
                                           //const QString& dbhost,
                                           )
{
    static int nNumber = 1;

    QDomElement domElement = domElement(domDoc,
                                        "datatbase",
                                        QString().setNum(nNumber));

    domElement.appendChild(makeElement(domDoc, "dbname", "", dbname));
    domElement.appendChild(makeElement(domDoc, "dbdriver", "", dbdriver));
    //domElement.appendChild(makeElement(domDoc, "dbhost", "", dbhost));

    nNumber++;

    return domElement;
}

QDomElement QSqlConnectionDialog::makeElement(QDomDocument& domDoc,
                                              const QString& strName,
                                              const QString strAttr = QString::null,
                                              const QString& strText = QString::null)
{
    QDomElement domElement = domDoc.createElement(strName);

    if(!strAttr.isEmpty()) {
        QDomAttr domAttr = domDoc.createAttribute("number");
        domAttr.setValue(strAttr);
        domElement.setAttributeNode(domAttr);
    }

    if(!strText.isEmpty()) {
        QDomText domText = domDoc.createTextNode(strText);
        domElement.appendChild(domText);
    }

    return domElement;
}

void QSqlConnectionDialog::writeInXml(QString db) {
    QDomElement elem = doc.createElement("user");
    elem.setAttribute("database", db);
    //.appendChild(elem);
}
*/
void QSqlConnectionDialog::on_okButton_clicked()
{
    if (ui.comboDriver->currentText().isEmpty()) {
        QMessageBox::information(this, tr("No database driver selected"),
                                 tr("Please select a database driver"));
        ui.comboDriver->setFocus();
    } else {
        accept();
    }
}
