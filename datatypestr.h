#ifndef DATATYPESTR_H
#define DATATYPESTR_H

#include "datatypebase.h"

class DataTypeStr : public DataTypeBase
{
protected:
    const QString mTypeName = "str";
public:
    DataTypeStr(QString label="", QString size="");
    virtual ~DataTypeStr();

    virtual QString format(const QByteArray &byteString) const;
    virtual QString getTypeName() { return mTypeName; }
};

#endif // DATATYPESTR_H
