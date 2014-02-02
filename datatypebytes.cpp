#include "datatypebytes.h"

DataTypeBytes::DataTypeBytes(int size)
    : DataTypeBase(size)
{
}

DataTypeBytes::~DataTypeBytes()
{
}

QString DataTypeBytes::format(const QByteArray &byteString) const
{
    return formatByteString(byteString);
}
