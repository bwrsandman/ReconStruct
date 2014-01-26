#ifndef DATATYPEBYTES_H
#define DATATYPEBYTES_H

#include "datatypebase.h"

class DataTypeBytes : public DataTypeBase
{
public:
    DataTypeBytes();
    virtual ~DataTypeBytes();

    virtual QString format(const QByteArray& byteString) const;
};

#endif // DATATYPEBYTES_H
