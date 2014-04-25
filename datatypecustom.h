#ifndef DATATYPECUSTOM_H
#define DATATYPECUSTOM_H

#include <QHash>
#include <QList>
#include <memory>

#include "datatypebase.h"

class DataTypeCustom : public DataTypeBase
{
private:
    QString mTypeName;
    QList<DataTypeBase*> mContents;
    QHash<QString, uint> mCachedSizes;
public:
    DataTypeCustom(const QString& label, const uint& size, const std::shared_ptr<DataTypeBase>& parent=nullptr);
    virtual ~DataTypeCustom();
    virtual void add(DataTypeBase * const datatype);
    virtual QString format(const QByteArray& byteString, uint& start) const;
    virtual const uint getActualSizeOf(const QString& label) const { return 0; }
    inline virtual QString getTypeName() { return mTypeName; }
};

#endif // DATATYPECUSTOM_H
