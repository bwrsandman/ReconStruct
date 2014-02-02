#ifndef DATATYPEINT_H
#define DATATYPEINT_H

#include "datatypebase.h"

class DataTypeInt : public DataTypeBase
{
protected:
    const QString mTypeName = "int";
public:
    DataTypeInt(int size=0);
    virtual ~DataTypeInt();

    virtual QString format(const QByteArray &byteString) const;
    virtual QString getTypeName() { return mTypeName; }
};

#endif // DATATYPEINT_H
