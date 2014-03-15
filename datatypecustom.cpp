#include <QStandardItem>

#include "datatypebytes.h"
#include "misc.h"

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
    declaredTypes[mTypeName] = NULL;
}

void DataTypeCustom::clearContents()
{
    foreach (DataTypeBase* dataType, mContents) {
        delete dataType;
    }
    mContents.clear();
}

void DataTypeCustom::update(const QStandardItem *source)
{
    clearContents();
    for (int i = 0; i < source->rowCount(); ++i)
    {
        QString label = source->child(i, cols::LABEL)->text();
        QString size = source->child(i, cols::SIZE)->text();
        QString type = source->child(i, cols::TYPE)->text();
        mContents << DataTypeBase::getInterpreter(type, label, size).release();
    }
    return;
}
