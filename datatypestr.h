#ifndef DATATYPESTR_H
#define DATATYPESTR_H

#include "datatypebase.h"

class DataTypeStr : public DataTypeBase
{
public:
    DataTypeStr();
    virtual ~DataTypeStr();

    virtual QString format(const QByteArray &byteString) const;
};

#endif // DATATYPESTR_H
