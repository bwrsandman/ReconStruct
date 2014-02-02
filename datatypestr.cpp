#include "datatypestr.h"

DataTypeStr::DataTypeStr(int size)
    : DataTypeBase(size)
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
