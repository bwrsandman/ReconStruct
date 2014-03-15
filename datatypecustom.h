#ifndef DATATYPECUSTOM_H
#define DATATYPECUSTOM_H

#include <QHash>
#include <QList>
#include <memory>

#include "datatypebase.h"

class QStandardItem;

class DataTypeCustom : public DataTypeBase
{
private:
    QString mTypeName;
    QList<DataTypeBase*> mContents;
    void clearContents();
public:
    DataTypeCustom(QString typeName);
    virtual ~DataTypeCustom();
    static QHash<QString, DataTypeCustom*> declaredTypes;
    inline virtual QString getTypeName() { return mTypeName; }
    inline virtual QList<DataTypeBase*> getContents() { return mContents; }
    void update(const QStandardItem* source);
};

#endif // DATATYPECUSTOM_H
