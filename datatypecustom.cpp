#include "datatypebytes.h"

#include "datatypecustom.h"

QHash<QString, DataTypeCustom* > DataTypeCustom::declaredTypes =
        QHash<QString, DataTypeCustom* >();

DataTypeCustom::DataTypeCustom(QString typeName)
    : mTypeName(typeName)
{
    mContents << new DataTypeBytes();
}

DataTypeCustom::~DataTypeCustom()
{
    foreach (DataTypeBase* dataType, mContents) {
        delete dataType;
    }
    declaredTypes[mTypeName] = NULL;
}
