#ifndef DATATYPEBOOL_H
#define DATATYPEBOOL_H

#include "datatypebase.h"

class DataTypeBool : public DataTypeBase
{
public:
    DataTypeBool();
    virtual ~DataTypeBool();
    virtual QString format(const QByteArray &byteString) const;
};

#endif // DATATYPEBOOL_H
