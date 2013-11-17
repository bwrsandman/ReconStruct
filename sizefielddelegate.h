#ifndef SIZEFIELDDELEGATE_H
#define SIZEFIELDDELEGATE_H

#include <QItemDelegate>

class SizeFieldDelegate : public QItemDelegate
{
public:
    explicit SizeFieldDelegate(QObject *parent);
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem & option,
                          const QModelIndex & index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index ) const;
};

#endif // SIZEFIELDDELEGATE_H
