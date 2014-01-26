#include "datatypestr.h"

DataTypeStr::DataTypeStr()
    : DataTypeBase()
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
