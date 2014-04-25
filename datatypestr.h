#ifndef DATATYPESTR_H
#define DATATYPESTR_H

#include "datatypebase.h"

class DataTypeStr : public DataTypeBase
{
public:
    DataTypeStr(const QString& label, const uint& size, const std::shared_ptr<DataTypeBase>& parent=nullptr);
    virtual ~DataTypeStr();

    virtual QString format(const QByteArray& byteString, uint& start) const;
    virtual const uint getActualSizeOf(const QString& label) const { return 0; }
};

#endif // DATATYPESTR_H
