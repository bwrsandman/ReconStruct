#include "datatypebytes.h"

DataTypeBytes::DataTypeBytes(QString label, QString size)
    : DataTypeBase(label, size)
{
}

DataTypeBytes::~DataTypeBytes()
{
}

QString DataTypeBytes::format(const QByteArray &byteString) const
{
    return formatByteString(byteString);
}
