#include "datatypebool.h"

#include "mainwindow.h" /* For tr() */

DataTypeBool::DataTypeBool(int size)
    : DataTypeBase(size)
{
}

DataTypeBool::~DataTypeBool()
{
}

QString DataTypeBool::format(const QByteArray &byteString) const
{
    if (byteString.isEmpty())
            return QString("false");
    bool ok = true;
    QString ret = (byteString.toHex().toInt(&ok, 16))? "true" : "false";
    return ok? ret: MainWindow::tr("Error");
}

