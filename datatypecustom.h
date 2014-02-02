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
public:
    DataTypeCustom(QString typeName);
    virtual ~DataTypeCustom();
    static QHash<QString, DataTypeCustom*> declaredTypes;
    inline virtual QString getTypeName() { return mTypeName; }
    inline virtual QList<DataTypeBase*> getContents() { return mContents; }
};

#endif // DATATYPECUSTOM_H
