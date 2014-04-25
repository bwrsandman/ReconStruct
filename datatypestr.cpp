#include "datatypestr.h"

DataTypeStr::DataTypeStr(const QString& label, const uint& size, const std::shared_ptr<DataTypeBase>& parent)
    : DataTypeBase(label, size, parent)
{
}

DataTypeStr::~DataTypeStr()
{
}

QString DataTypeStr::format(const QByteArray &byteString, uint& start) const
{
    if (byteString.isEmpty())
            return QString();
    uint size = getSize();
    QString ret = QString(byteString).mid(start, size);
    start += size;
    return ret;
}
