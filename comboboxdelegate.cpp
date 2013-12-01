#include <QItemDelegate>
#include <QComboBox>
#include "comboboxdelegate.h"

ComboBoxDelegate::ComboBoxDelegate(QObject *parent, const QStringList &items):
    QItemDelegate(parent),
    items(items),
    customItems(QStringList())
{
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem&,
                                        const QModelIndex&) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->addItems(items);
    editor->insertSeparator(items.length());
    editor->addItems(customItems);
    if (!customItems.isEmpty())
        editor->insertSeparator(items.length() + customItems.length() + 1);
    editor->addItem(tr("custom"));
    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget*, const QModelIndex&) const
{
    // QString value = index.model()->data(index, Qt::EditRole).toString();
    //QComboBox *comboBox = static_cast<QComboBox*>(editor);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

void ComboBoxDelegate::addItem(const QString item)
{
    if (!items.contains(item) && !customItems.contains(item))
        customItems.append(item);
}
