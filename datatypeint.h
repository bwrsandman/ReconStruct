#ifndef DATATYPEINT_H
#define DATATYPEINT_H

#include "datatypebase.h"

class DataTypeInt : public DataTypeBase
{
protected:
    const QString mTypeName = "int";
public:
    DataTypeInt(QString label="", QString size="");
    virtual ~DataTypeInt();

    virtual QString format(const QByteArray &byteString) const;
    virtual QString getTypeName() { return mTypeName; }
};

#endif // DATATYPEINT_H
