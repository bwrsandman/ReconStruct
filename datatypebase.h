#ifndef DATATYPEBASE_H
#define DATATYPEBASE_H

#include <QString>
#include <memory>

class DataTypeBase
{
protected:
    QString mLabel;
    uint mSize;
    QString mSizeReference;
    std::shared_ptr<const DataTypeBase> mParent;
public:
    DataTypeBase(const QString& label, const uint& size, const std::shared_ptr<DataTypeBase>& parent=nullptr);
    DataTypeBase(const QString& label, const QString& size, const std::shared_ptr<DataTypeBase>& parent=nullptr);
    virtual ~DataTypeBase();
    virtual QString format(const QByteArray& byteString, uint& start) const=0;
    virtual const uint getSize() const;
    virtual const uint getActualSizeOf(const QString& label) const=0;
    virtual void setParent(const std::shared_ptr<const DataTypeBase> parent) { mParent = parent; }
};

#endif // DATATYPEBASE_H
