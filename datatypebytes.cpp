#include "datatypebytes.h"

DataTypeBytes::DataTypeBytes()
    : DataTypeBase()
{
}

DataTypeBytes::~DataTypeBytes()
{
}

QString DataTypeBytes::format(const QByteArray &byteString) const
{
    return formatByteString(byteString);
}

