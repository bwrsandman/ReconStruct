#include "datatypecustom.h"

DataTypeCustom::DataTypeCustom(const QString& label, const uint& size, const std::shared_ptr<DataTypeBase>& parent)
    : DataTypeBase(label, size, parent)
{
}

DataTypeCustom::~DataTypeCustom()
{
}

void DataTypeCustom::add(DataTypeBase * const datatype)
{
    datatype->setParent(std::shared_ptr<const DataTypeCustom>(this));
    mContents.append(datatype);
}

QString DataTypeCustom::format(const QByteArray &byteString, uint& start) const
{
    QString ret = "";
    for (int i = 0; i < mSize; ++i)
    {
        foreach (DataTypeBase * dataType, mContents) {
            ret += dataType->format(byteString, start);
        }
    }
    return ret;
}
