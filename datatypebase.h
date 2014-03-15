#ifndef DATATYPEBASE_H
#define DATATYPEBASE_H

#include <QString>
#include <memory>

class DataTypeBase
{
protected:
    const QString mTypeName = "ERROR: BaseType";
    QString mLabel = "";
    QString mSize = "";
    virtual QString formatByteString(const QByteArray& byteString) const;
public:
    DataTypeBase(QString label="", QString size="");
    virtual ~DataTypeBase();
    virtual QString format(const QByteArray& byteString) const;
    static std::unique_ptr<DataTypeBase> getInterpreter(const QString& type, const QString &label="", const QString &string="");
    virtual QString getLabel() { return mLabel; }
    virtual QString getSize() { return mSize.isEmpty()? "0x0" : mSize; }
    virtual QString getTypeName() { return mTypeName; }

};

#endif // DATATYPEBASE_H
