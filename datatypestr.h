#ifndef DATATYPESTR_H
#define DATATYPESTR_H

#include "datatypebase.h"

class DataTypeStr : public DataTypeBase
{
protected:
    const QString mTypeName = "str";
public:
    DataTypeStr(int size=0);
    virtual ~DataTypeStr();

    virtual QString format(const QByteArray &byteString) const;
    virtual QString getTypeName() { return mTypeName; }
};

#endif // DATATYPESTR_H
