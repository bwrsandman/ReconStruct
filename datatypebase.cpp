#include "datatypebase.h"

#include "mainwindow.h" /* For tr() */

DataTypeBase::DataTypeBase()
{
}

DataTypeBase::~DataTypeBase()
{
}

QString DataTypeBase::formatByteString(const QByteArray &byteString) const
{
    QString formattedBytes;
    if (!byteString.isEmpty())
        formattedBytes = "0x" + QString(byteString.toHex()).toUpper();
    return formattedBytes;
}

QString DataTypeBase::format(const QByteArray &byteString) const
{
    return MainWindow::tr("TODO: Preview unimplemented (%1)")
                       .arg(formatByteString(byteString));
}
