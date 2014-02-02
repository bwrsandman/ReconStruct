#ifndef DATATYPEBOOL_H
#define DATATYPEBOOL_H

#include "datatypebase.h"

class DataTypeBool : public DataTypeBase
{
protected:
    const QString mTypeName = "bool";
public:
    DataTypeBool(int size=0);
    virtual ~DataTypeBool();

    virtual QString format(const QByteArray &byteString) const;
    virtual QString getTypeName() { return mTypeName; }
};

#endif // DATATYPEBOOL_H
