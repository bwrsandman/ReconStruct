#ifndef DATATYPEBASE_H
#define DATATYPEBASE_H

#include <QString>
#include <memory>

class DataTypeBase
{
protected:
    const QString mTypeName = "ERROR: BaseType";
    int mSize = 0;
    virtual QString formatByteString(const QByteArray& byteString) const;
public:
    DataTypeBase(int size=0);
    virtual ~DataTypeBase();
    virtual QString format(const QByteArray& byteString) const;
    static std::unique_ptr<DataTypeBase> getInterpreter(const QString& type);
    virtual int getSize() { return mSize; }
    virtual QString getTypeName() { return mTypeName; }

};

#endif // DATATYPEBASE_H
