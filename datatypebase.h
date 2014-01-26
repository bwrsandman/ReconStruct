#ifndef DATATYPEBASE_H
#define DATATYPEBASE_H

#include <QString>
#include <memory>

class DataTypeBase
{
public:
    DataTypeBase();
    virtual ~DataTypeBase();
    virtual QString format(const QByteArray& byteString) const;
    static std::unique_ptr<DataTypeBase> getInterpreter(const QString& type);

protected:
    virtual QString formatByteString(const QByteArray& byteString) const;
};

#endif // DATATYPEBASE_H
