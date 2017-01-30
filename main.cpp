#include "logindialog.h"
#include "mainwindow.h"

#include <QtCore>
#include <QtWidgets>
#include <QtSql>

void addConnectionsFromCommandline(const QStringList &args, MainWindow *mainWin)
{
    for (int i = 1; i < args.count(); ++i) {
        QUrl url(args.at(i), QUrl::TolerantMode);
        if (!url.isValid()) {
            qWarning("Invalid URL: %s", qPrintable(args.at(i)));
            continue;
        }
        QSqlError err = mainWin->addConnection(url.scheme(), url.path().mid(1), url.host(),
                                               url.userName(), url.password(), url.port(-1));
        if (err.type() != QSqlError::NoError)
            qDebug() << "Unable to open connection:" << err;
    }
}

void addConnectionsFromLoginDialog(const QStringList &args, MainWindow *mainWin)
{
    for (int i = 1; i < args.count(); ++i) {
        QUrl url(args.at(i), QUrl::TolerantMode);
        if (!url.isValid()) {
            qWarning("Invalid URL: %s", qPrintable(args.at(i)));
            continue;
        }
        QSqlError err = mainWin->addConnection(url.scheme(), url.path().mid(1), url.host(),
                                               url.userName(), url.password(), url.port(-1));
        if (err.type() != QSqlError::NoError)
            qDebug() << "Unable to open connection:" << err;
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    LoginDialog logDial;
    logDial.setWindowTitle(QObject::tr("Верификация пользователя"));
    if (logDial.exec() != QDialog::Accepted) {
        return 0;

    }

    MainWindow mainwindow;
    mainwindow.setWindowTitle(QObject::tr("DataBase"));
    //mainwindow.setWindowState(Qt::WindowFullScreen);

    QObject::connect(&mainwindow, SIGNAL(statusMessage(QString)),
                         mainwindow.statusBar(), SLOT(showMessage(QString)));

    //if(argc > 1)
    //    addConnectionsFromCommandline(app.arguments(), &mainwindow);
    //else
    //    addConnectionsFromLoginDialog(logDial.arguments(), &mainwindow);
    mainwindow.show();
    if (QSqlDatabase::connectionNames().isEmpty())
        QMetaObject::invokeMethod(&mainwindow, "addConnection", Qt::QueuedConnection);



    return app.exec();
}
