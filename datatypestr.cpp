#include "datatypestr.h"

DataTypeStr::DataTypeStr(QString label, QString size)
    : DataTypeBase(label, size)
{
}

DataTypeStr::~DataTypeStr()
{
}

QString DataTypeStr::format(const QByteArray &byteString) const
{
    if (byteString.isEmpty())
            return QString();
    return QString(byteString);
}
