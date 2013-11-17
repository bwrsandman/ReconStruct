#include <QComboBox>
#include "comboboxdelegate.h"

ComboBoxDelegate::ComboBoxDelegate(QObject *parent, const QStringList &items):
    QItemDelegate(parent),
    items(items)
{
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem&,
                                        const QModelIndex&) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->addItems(items);
    return editor;
}

void ComboBoxDelegate::setModelData(QWidget *editor,
                                    QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex&) const
{
    editor->setGeometry(option.rect);
}
