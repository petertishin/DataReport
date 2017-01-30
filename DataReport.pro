#-------------------------------------------------
#
# Project created by QtCreator 2016-02-15T17:05:11
#
#-------------------------------------------------

#include(C:\Qt\qt_full\5.2.1\QtXlsxWriter-master\src\xlsx)
QT       += core gui xlsx

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql xml

TARGET = DataReport
TEMPLATE = app


SOURCES += main.cpp\
    qsqlconnectiondialog.cpp \
    devicewidget.cpp \
    logindialog.cpp \
    waybillwidget.cpp \
    departmentdialog.cpp \
    devicebrowser.cpp \
    clientdialog.cpp \
    mainwindow.cpp \
    startpage.cpp \
    waybillbrowser.cpp \
    repairdialog.cpp \
    importer.cpp \
    accessdialog.cpp


HEADERS  += \
    waybillbrowser.h \
    qsqlconnectiondialog.h \
    devicewidget.h \
    logindialog.h \
    waybillwidget.h \
    departmentdialog.h \
    devicebrowser.h \
    clientdialog.h \
    mainwindow.h \
    startpage.h \
    repairdialog.h \
    importer.h \
    accessdialog.h

FORMS    += \
    waybillbrowser.ui \
    qsqlconnectiondialog.ui \
    logindialog.ui \
    waybillwidget.ui \
    departmentdialog.ui \
    devicebrowser.ui \
    clientdialog.ui \
    mainwindow.ui \
    startpage.ui \
    devicewidget.ui \
    repairdialog.ui \
    accessdialog.ui
