#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QItemDelegate>

class ComboBoxDelegate : public QItemDelegate
{
public:
    explicit ComboBoxDelegate(QObject *parent, const QStringList &items);
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem & option,
                          const QModelIndex & index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex & index) const;
    void addItem(const QString);
private:
    const QStringList items;
    QStringList customItems;
};

#endif // COMBOBOXDELEGATE_H
