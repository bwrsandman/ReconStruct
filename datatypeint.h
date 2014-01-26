#ifndef DATATYPEINT_H
#define DATATYPEINT_H

#include "datatypebase.h"

class DataTypeInt : public DataTypeBase
{
public:
    DataTypeInt();
    virtual ~DataTypeInt();
    virtual QString format(const QByteArray &byteString) const;
};

#endif // DATATYPEINT_H
