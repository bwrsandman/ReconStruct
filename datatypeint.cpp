#include "datatypeint.h"

#include "mainwindow.h" /* For tr() */

DataTypeInt::DataTypeInt()
    : DataTypeBase()
{
}

DataTypeInt::~DataTypeInt ()
{
}

QString DataTypeInt::format(const QByteArray &byteString) const
{
    if (byteString.isEmpty())
            return QString();
    bool ok = true;
    // Reverse byte order
    QByteArray reverse = QByteArray(byteString.length(), '\0');
    std::reverse_copy(byteString.constBegin(), byteString.constEnd(),
                      reverse.begin());
    QString ret = QString::number(reverse.toHex().toInt(&ok, 16));
    return ok? ret: MainWindow::tr("Error");
}
