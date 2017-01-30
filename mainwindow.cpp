#include "mainwindow.h"
#include "startpage.h"
#include "waybillbrowser.h"
#include "waybillwidget.h"
#include "devicebrowser.h"
#include "devicewidget.h"
#include "qsqlconnectiondialog.h"
//#include "xlsxdocument.h"

#include <QFileDialog>
#include <QtXml>
#include <QtSql>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    tabId = TabWidget->currentIndex();//start tab
    StartPage *startpage = new StartPage(activeDb, tables, tab);
    TabWidget->setCurrentWidget(startpage);
    
    connect(startpage, SIGNAL(openWaybillBrowser()), this, SLOT(openWaybillBrowser()));
    connect(startpage, SIGNAL(openDeviceBrowser()), this, SLOT(openDeviceBrowser()));
    connect(startpage, SIGNAL(openRepair()), this, SLOT(openRepair()));
    connect(startpage, SIGNAL(openDialExcelBrowser()), this, SLOT(openDialExcelBrowser()));
    connect(TabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

    emit statusMessage(tr("Ready."));
}

MainWindow::~MainWindow()
{
}

QSqlDatabase MainWindow::currentDatabase() const
{
    return QSqlDatabase();
}

void MainWindow::addConnection()
{
    QSqlConnectionDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    if (dialog.useInMemoryDatabase()) {
        QSqlDatabase::database("in_mem_db", false).close();
        QSqlDatabase::removeDatabase("in_mem_db");
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "in_mem_db");

        db.setDatabaseName(":memory:");
        if (!db.open())
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                                                         "opening the connection: ") + db.lastError().text());
        activeDb = db.connectionName();
        this->createMemoryDatabase(db);

        tables = db.tables();
    }else if(dialog.createNewDatabase()) {
        //QString dbname = dialog.databaseName();
        //QSqlDatabase db = QSqlDatabase::addDatabase(dialog.databaseName());
        //db.setDatabaseName(dbname);
        //db.open();
        QString dbname = "C:/QtProjects/tutorials/DataBase/SQLbasetysql.sqlite";
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbname);
        db.open();

        QSqlQuery q("", db);

        q.exec("CREATE TABLE Department (DepartmentId int primary key, Name varchar, Address varchar, Accessories varchar)");

        q.exec("CREATE TABLE Type (TypeId int primary key, Type varchar)");

        q.exec("CREATE TABLE Client (ClientId int primary key, Firstname varchar, Lastname varchar, Thirdname varchar, "
                                    "Telefon varchar, Email varchar, DepartmentName int)");

        q.exec("CREATE TABLE Device (Serial int primary key, Type int, State int, Produced varchar, "
                                    "Varranty int, Description varchar, DateVar varchar)");

        q.exec("CREATE TABLE Waybill (WaybillId int primary key, Serial number, Date varchar, "
                                     "DepartmentName int, DepartmentAddress int, DepartmentAccessories int, "
                                     "ClientFirstname int, ClientLastname int, ClientThirdname int, "
                                     "ClientTelefon int, ClientEmail int)");

        q.exec("CREATE TABLE Work (WorkId int primary key, Malfunctions varchar, Description varchar, DateR varchar, "
                                  "DateS varchar, DeviceSerial int, InWaybillSerial int)");

        q.exec("CREATE TABLE Request (RequestId int primary key, Date varchar,"
                                     "ClientFirstname int, ClientLastname int, ClientThirdname int, "
                                     "DepartmentName int, WorkId int)");

        q.exec("CREATE TABLE State (StateId int primary key, State varchar)");

        q.exec("CREATE TABLE Varranty (VarrantyId int primary key, Varranty varchar)");

    } else {
        QSqlError err = addConnection(dialog.driverName(), dialog.databaseName(), dialog.hostName(),
                           dialog.userName(), dialog.password(), dialog.port());
        if (err.type() != QSqlError::NoError)
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                       "opening the connection: ") + err.text());
    }
}

QSqlError MainWindow::addConnection(const QString &driver, const QString &dbName, const QString &host,
                                    const QString &user, const QString &passwd, int port)
{
    static int cCount = 0;

    QSqlError err;
    QSqlDatabase db = QSqlDatabase::addDatabase(driver, QString("Browser%1").arg(++cCount));
    db.setDatabaseName(dbName);
    activeDb = dbName;
    db.setHostName(host);
    db.setPort(port);
    if (!db.open(user, passwd)) {
        err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QString("Browser%1").arg(cCount));
    }

    tables = db.tables();
    return err;
}

void MainWindow::createMemoryDatabase(QSqlDatabase &db)
{
    QSqlQuery q("", db);

    q.exec("CREATE TABLE Department (DepartmentId int primary key, Name varchar, Address varchar, Accessories varchar)");

    q.exec("CREATE TABLE Type (TypeId int primary key, Type varchar)");

    q.exec("CREATE TABLE Client (ClientId int primary key, Firstname varchar, Lastname varchar, Thirdname varchar, "
                                "Telefon varchar, Email varchar, DepartmentName int)");

    q.exec("CREATE TABLE Device (Serial int primary key, Type int, State int, Produced varchar, "
                                "Varranty int, Description varchar, DateVar varchar, Department int)");

    q.exec("CREATE TABLE Waybill (Serial int primary key, Date varchar, "
                                 "DepartmentName int, DepartmentAddress int, DepartmentAccessories int, "
                                 "ClientFirstname int, ClientLastname int, ClientThirdname int, "
                                 "ClientTelefon int, ClientEmail int)");

    q.exec("CREATE TABLE Work (WorkId int primary key, Malfunctions varchar, Description varchar, DateR varchar, "
                              "DateS varchar, DeviceSerial int, WaybillSerial int)");

    q.exec("CREATE TABLE Request (RequestId int primary key, Date varchar,"
                                 "ClientFirstname int, ClientLastname int, ClientThirdname int, "
                                 "DepartmentName int, WorkId int)");

    q.exec("CREATE TABLE State (StateId int primary key, State varchar)");

    q.exec("CREATE TABLE Varranty (VarrantyId int primary key, Varranty varchar)");

    //----------------------------------------------------------------------------------------------------------------
    q.exec("INSERT INTO Department VALUES (0, 'Моск. обл. ГУМВД (ОПБ)', 'Москва, Ленинский пр-т, д.1', '114560 VIN BIN AAA 555 111 сегодня не работаем')");
    q.exec("INSERT INTO Department VALUES (1, 'Моск. центр ГУМВД (ОПБ)', 'Москва, Ломоносова пр-т, д.31', '125466 VIN BIN AAA 666 777 добро пожаловать')");

    q.exec("INSERT INTO Type VALUES (0, 'N90')");

    q.exec("INSERT INTO State VALUES (0, 'В использовании')");
    q.exec("INSERT INTO State VALUES (1, 'В ремонте')");

    q.exec("INSERT INTO Varranty VALUES (0, 'Снят')");
    q.exec("INSERT INTO Varranty VALUES (1, 'На гарантии')");

    q.exec("INSERT INTO Client VALUES (0, 'Семен', 'Трусов', 'Сергеевич', '+7(354) 353 2376', 'TruSS@pop2.ru', 0)");
    q.exec("INSERT INTO Client VALUES (1, 'Павел', 'Невняшкин', 'Могаевич', '+7(753) 326 7643', 'pavMogu@man.ru', 0)");

    q.exec("INSERT INTO Device VALUES (201300201, 0, 0, '01.12.2013', 0, 'Истечение срока', '01.12.2015', 0)");

    q.exec("INSERT INTO Work VALUES (0, 'Не работает', 'Зарядили', '31.03.2014', '20.04.2014', 201300201, 140001)");
    q.exec("INSERT INTO Work VALUES (1, 'Не осуществляется зарядка', 'Замена аккумуляторной батареи', '25.10.2014', '27.11.2014', 201300201, 140003)");
    q.exec("INSERT INTO Work VALUES (2, 'Не работает', 'Замена платы питания', '01.08.2015', '20.09.2015', 201300201, 140004)");
    q.exec("INSERT INTO Work VALUES (3, 'Монитор не работает', 'Замена монитора', '31.02.2016', ' ', 201300201, 160001)");

    q.exec("INSERT INTO Waybill VALUES (140001, '31.03.2014', 0, 0, 0, 0, 0, 0, 0, 0)");
    q.exec("INSERT INTO Waybill VALUES (140002, '20.04.2014', 0, 0, 0, 0, 0, 0, 0, 0)");
    q.exec("INSERT INTO Waybill VALUES (140003, '25.10.2014', 0, 0, 0, 1, 1, 1, 1, 1)");
    q.exec("INSERT INTO Waybill VALUES (140004, '27.11.2014', 0, 0, 0, 1, 1, 1, 1, 1)");
    q.exec("INSERT INTO Waybill VALUES (150001, '01.08.2015', 0, 0, 0, 0, 0, 0, 0, 0)");
    q.exec("INSERT INTO Waybill VALUES (150002, '20.09.2015', 0, 0, 0, 1, 1, 1, 1, 1)");
    q.exec("INSERT INTO Waybill VALUES (160001, '31.02.2016', 0, 0, 0, 0, 0, 0, 0, 0)");

    q.exec("INSERT INTO Request VALUES (0, '19.03.2014', 0, 0, 0, 0, 0)");
    q.exec("INSERT INTO Request VALUES (1, '19.11.2014', 1, 1, 1, 0, 0)");
    q.exec("INSERT INTO Request VALUES (2, '26.11.2014', 1, 1, 1, 0, 0)");
    q.exec("INSERT INTO Request VALUES (3, '15.09.2015, 1, 1, 1, 0, 0)");
    q.exec("INSERT INTO Request VALUES (4, '19.09.2015', 1, 1, 1, 0, 0)");
    q.exec("INSERT INTO Request VALUES (5, '21.03.2016', 0, 0, 0, 0, 0)");
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About"), tr("This software prduced specially for "
        "REL Ingeneering co. to store productions information. There are used only "
        "opensource code based on Qt's library, C++ and SQLite database. "));
}

void MainWindow::sendStatusMessage(const QString &message)
{
    emit statusMessage(message);
}

void MainWindow::closeTab()
{
    tabId = TabWidget->currentIndex();
    TabWidget->removeTab(tabId);
}

void MainWindow::closeTab(int index)
{
    if(index>0)
    {
        TabWidget->removeTab(index);
    }
    if(TabWidget->count()<2)
        TabWidget->setTabsClosable(false);
}

void MainWindow::openWaybillBrowser()
{
    WaybillBrowser *browser = new WaybillBrowser(activeDb, tables, this);
    tabId = TabWidget->addTab(browser, tr("Накладные"));
    if( TabWidget->currentIndex() != tabId)
        TabWidget->setCurrentIndex(tabId);
    TabWidget->setTabsClosable(true);
    connect(browser, SIGNAL(statusMessage(QString)), this, SLOT(sendStatusMessage(QString)));
    connect(browser, SIGNAL(openEditWidget(QSqlRecord)), this, SLOT(openWaybillEdit(QSqlRecord)));
}

void MainWindow::openDeviceBrowser()
{
    DeviceBrowser *browser = new DeviceBrowser(activeDb, tables, this);
    tabId = TabWidget->addTab(browser, tr("Устройства"));
    if( TabWidget->currentIndex() != tabId)
        TabWidget->setCurrentIndex(tabId);
    TabWidget->setTabsClosable(true);
    connect(browser, SIGNAL(statusMessage(QString)), this, SLOT(sendStatusMessage(QString)));
    connect(browser, SIGNAL(openEditWidget(QSqlRecord)), this, SLOT(openDeviceEdit(QSqlRecord)));
}

void MainWindow::openRepair()
{
    WayBillWidget *waybill = new WayBillWidget(activeDb, tables, this);
    tabId = TabWidget->addTab(waybill, tr("Прием в ремонт"));
    if( TabWidget->currentIndex() != tabId)
        TabWidget->setCurrentIndex(tabId);
    TabWidget->setTabsClosable(true);
    connect(waybill, SIGNAL(statusMessage(QString)), this, SLOT(sendStatusMessage(QString)));
    connect(waybill, SIGNAL(closeTab()), this, SLOT(closeTab()));
}

void MainWindow::openWaybillEdit(const QSqlRecord &record)
{
    WayBillWidget *waybill = new WayBillWidget(record, activeDb, tables, this);
    tabId = TabWidget->addTab(waybill, record.value(QString("Serial")).toString());
    if( TabWidget->currentIndex() != tabId)
        TabWidget->setCurrentIndex(tabId);
    TabWidget->setTabsClosable(true);
    connect(waybill, SIGNAL(statusMessage(QString)), this, SLOT(sendStatusMessage(QString)));
    connect(waybill, SIGNAL(closeTab()), this, SLOT(closeTab()));
}

void MainWindow::openDeviceEdit(const QSqlRecord &record)
{
    DeviceWidget *device = new DeviceWidget(record, activeDb, tables);
    tabId = TabWidget->addTab(device, record.value(QString("Serial")).toString());
    if( TabWidget->currentIndex() != tabId)
        TabWidget->setCurrentIndex(tabId);
    connect(device, SIGNAL(statusMessage(QString)), this, SLOT(sendStatusMessage(QString)));
    connect(device, SIGNAL(closeTab()), this, SLOT(closeTab()));

}

void MainWindow::openDialExcelBrowser()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Выберите файл для извлечения данных:"),QString(),
                                                        QString().fromLocal8Bit("Microsoft Excel (*.xlsx)"));
    //QXlsx::Document xlsx(fileName);

}
