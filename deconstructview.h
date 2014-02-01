#ifndef DECONSTRUCTVIEW_H
#define DECONSTRUCTVIEW_H

#include <memory>

class ComboBoxDelegate;
class QItemSelectionModel;
class QHexEdit;
class QString;
class QStandardItem;
class QStandardItemModel;
class QTreeView;

const QStringList defaultTypes =
        QStringList() << "bytes" << "str" << "int" << "bool";
const QStringList columns = QStringList()
        << "Label" << "Size" << "Type" << "Preview";

namespace cols {
enum {
    LABEL, SIZE, TYPE, PREVIEW
};
}

class DeconstructView
{
private:
    std::unique_ptr<QTreeView> mTreeView;
    std::unique_ptr<QStandardItemModel> mModel;
    std::unique_ptr<QItemSelectionModel> mSelectionModel;
    std::unique_ptr<ComboBoxDelegate> mTypesDelegate;
    QHexEdit* mHexEdit;
    void setModel();
    void itemLabelChanged(QStandardItem * const);
    void itemSizeChanged(QStandardItem * const);
    void itemTypeChanged(QStandardItem * const);
    void itemPreviewChanged(QStandardItem * const);
    int getEntrySize(const int row, QStandardItem *parent,
                     bool fromChildren=true) const;
    void balanceChildren(QStandardItem* const item);
    void createCustomType(QStandardItem* const type);
    void setPreview(const QModelIndex& index);
    int parseSizeExpression(const QString text, const int row) const;
    int parseProduct(const QString text, const int row) const;
    int parseEquals(const QString text, const int row) const;
    int parseNequals(const QString text, const int row) const;
    int getCoveredSize(const int end, QStandardItem *parent=0) const;
    QString formatPreview(const int start, const int end,
                          const QString& type) const;
    int parseSizeElement(const QString text, const int row) const;
    int getSizeFromText(const QString text, const int end) const;
    int getSelectionSize() const;

public:
    DeconstructView(QTreeView * treeView, QHexEdit* hexEdit);
    void itemChanged(QStandardItem*, bool selectAfter=true);
    void refreshPreview();
    void selectionChanged();
    void addRow(QString labelText=QString(),
                QString sizeText=QString(),
                QString typeText=QString("bytes"),
                QStandardItem *rootItem=0);
    void removeRows(int start, int count);
    inline void clearRows() { removeRows(0, rowCount()); }
    inline int rowCount() { return mModel->rowCount(); }
    inline QString labelAt(const int i)
        { return mModel->item(i, cols::LABEL)->text(); }
    inline QString sizeAt(const int i)
        { return mModel->item(i, cols::SIZE)->text(); }
    inline QString typeAt(const int i)
        { return mModel->item(i, cols::TYPE)->text(); }
    inline QString previewAt(const int i)
        { return mModel->item(i, cols::PREVIEW)->text(); }
    inline void setEnabled(const bool enabled)
        { mTreeView->setEnabled(enabled); }
    const QStandardItemModel* getModelPtr() const
        { return mModel.get(); }
    const QItemSelectionModel* getSelectionModelPtr() const
        { return mSelectionModel.get(); }
};

#endif // DECONSTRUCTVIEW_H
