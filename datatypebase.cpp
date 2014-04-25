#include "datatypebase.h"

DataTypeBase::DataTypeBase(const QString& label, const uint& size, const std::shared_ptr<DataTypeBase>& parent)
    : mLabel(label)
    , mSize(size)
    , mSizeReference(QString::null)
    , mParent(parent)
{
}

DataTypeBase::DataTypeBase(const QString& label, const QString& size, const std::shared_ptr<DataTypeBase>& parent)
    : mLabel(label)
    , mSize(0)
    , mSizeReference(size)
    , mParent(parent)
{
}

DataTypeBase::~DataTypeBase()
{
}

const uint DataTypeBase::getSize() const
{
    return mSizeReference.isNull() ?
                mSize :
                mParent->getActualSizeOf(mSizeReference);

}
