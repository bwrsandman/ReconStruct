#include "datatypebase.h"

#include "mainwindow.h" /* For tr() */

#include "datatypebytes.h"
#include "datatypebool.h"
#include "datatypeint.h"
#include "datatypestr.h"

DataTypeBase::DataTypeBase(int size)
    :mSize(size)
{
}

DataTypeBase::~DataTypeBase()
{
}

QString DataTypeBase::formatByteString(const QByteArray &byteString) const
{
    QString formattedBytes;
    if (!byteString.isEmpty())
        formattedBytes = "0x" + QString(byteString.toHex()).toUpper();
    return formattedBytes;
}

QString DataTypeBase::format(const QByteArray &byteString) const
{
    return MainWindow::tr("TODO: Preview unimplemented (%1)")
            .arg(formatByteString(byteString));
}

std::unique_ptr<DataTypeBase> DataTypeBase::getInterpreter(const QString &type)
{
    std::unique_ptr<DataTypeBase> typeInterpreter;
    if (type == "bytes") {
        typeInterpreter.reset(new DataTypeBytes());
    } else if (type == "str"){
        typeInterpreter.reset(new DataTypeStr());
    } else if (type == "int"){
        typeInterpreter.reset(new DataTypeInt());
    } else if (type == "bool"){
        typeInterpreter.reset(new DataTypeBool());
    } else {
        typeInterpreter.reset();
    }
    return typeInterpreter;
}
