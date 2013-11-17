#include <QTreeView>
#include <QStandardItemModel>

#include "sizefielddelegate.h"
#include "sizefield.h"

SizeFieldDelegate::SizeFieldDelegate(QObject *parent):
    QItemDelegate(parent)
{
}

QWidget *SizeFieldDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem&,
                                        const QModelIndex &index) const
{
    QTreeView *treeView = static_cast<QTreeView*>(this->parent());
    QStandardItemModel *model = static_cast<QStandardItemModel*>(treeView->model());
    QString text = model->itemFromIndex(index)->text();
    SizeField *editor = new SizeField(parent, text);
    return editor;
}

void SizeFieldDelegate::setModelData(QWidget *editor,
                                     QAbstractItemModel *model,
                                     const QModelIndex &index) const
{
    SizeField *sizeField = static_cast<SizeField*>(editor);
    model->setData(index, sizeField->currentText(), Qt::EditRole);
}


void SizeFieldDelegate::updateEditorGeometry(QWidget *editor,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex&) const
{
    SizeField *sizeField = static_cast<SizeField*>(editor);
    sizeField->setGeometry(option.rect);
}
