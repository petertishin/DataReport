#ifndef DEVICECONTROLDIALOG_H
#define DEVICECONTROLDIALOG_H

#include <QDialog>
#include "ui_devicecontroldialog.h"

class DeviceControlDialog : public QDialog, private Ui::DeviceControlDialog
{
    Q_OBJECT
public:
    explicit DeviceControlDialog(QWidget *parent = 0);

signals:

public slots:

};

#endif // DEVICECONTROLDIALOG_H
