#ifndef DATATYPEBYTES_H
#define DATATYPEBYTES_H

#include "datatypebase.h"

class DataTypeBytes : public DataTypeBase
{
protected:
    const QString mTypeName = "bytes";
public:
    DataTypeBytes(int size=0);
    virtual ~DataTypeBytes();

    virtual QString format(const QByteArray& byteString) const;
    virtual QString getTypeName() { return mTypeName; }
};

#endif // DATATYPEBYTES_H
