#ifndef DATATYPEBASE_H
#define DATATYPEBASE_H

#include <QString>

class DataTypeBase
{
public:
    DataTypeBase();
    virtual ~DataTypeBase();
    virtual QString format(const QByteArray& byteString) const;

protected:
    virtual QString formatByteString(const QByteArray& byteString) const;
};

#endif // DATATYPEBASE_H
