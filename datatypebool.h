#ifndef DATATYPEBOOL_H
#define DATATYPEBOOL_H

#include "datatypebase.h"

class DataTypeBool : public DataTypeBase
{
protected:
    const QString mTypeName = "bool";
public:
    DataTypeBool(QString label="", QString size="");
    virtual ~DataTypeBool();

    virtual QString format(const QByteArray &byteString) const;
    virtual QString getTypeName() { return mTypeName; }
};

#endif // DATATYPEBOOL_H
