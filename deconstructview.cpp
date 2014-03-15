#include <QInputDialog>
#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QTreeView>
#include <QHexEdit>

#include "comboboxdelegate.h"
#include "datatypebase.h"
#include "datatypecustom.h"
#include "mainwindow.h" /* For tr() */

#include "deconstructview.h"

DeconstructView::DeconstructView(QTreeView * treeView, QHexEdit* hexEdit)
    : mTreeView(treeView)
    , mModel(new QStandardItemModel())
    , mSelectionModel(new QItemSelectionModel(mModel.get(),
                                              mTreeView.get()))
    , mHexEdit(hexEdit)
{
    setModel();
}

void DeconstructView::setModel()
{
    QStandardItem *lbl_label = new QStandardItem(MainWindow::tr("Label"));
    mModel->setHorizontalHeaderItem(cols::LABEL, lbl_label);

    QStandardItem *lbl_size = new QStandardItem(MainWindow::tr("Size"));
    mModel->setHorizontalHeaderItem(cols::SIZE, lbl_size);

    QStandardItem *lbl_type = new QStandardItem(MainWindow::tr("Type"));
    mModel->setHorizontalHeaderItem(cols::TYPE, lbl_type);
    mTypesDelegate.reset(new ComboBoxDelegate(mTreeView.get(),
                                              defaultTypes));
    mTreeView->setItemDelegateForColumn(cols::TYPE, mTypesDelegate.get());

    QStandardItem *lbl_prev = new QStandardItem(MainWindow::tr("Preview"));
    mModel->setHorizontalHeaderItem(cols::PREVIEW, lbl_prev);

    mTreeView->setModel(mModel.get());
    mTreeView->setSelectionModel(mSelectionModel.get());
    mTreeView->setColumnWidth(cols::LABEL, 100);
    mTreeView->setColumnWidth(cols::SIZE, 50);
    mTreeView->setColumnWidth(cols::TYPE, 55);
    mTreeView->setColumnWidth(cols::PREVIEW, 20);

    DataTypeCustom::declaredTypes = QHash<QString, DataTypeCustom*>();
}

void DeconstructView::itemLabelChanged(QStandardItem * const)
{

}

void DeconstructView::itemSizeChanged(QStandardItem * const size)
{
    QStandardItem *type = mModel->item(size->row(), cols::TYPE);
    if (mTypesDelegate->getCustomItems().contains(type->text()))
    {
        int entrySize = getEntrySize(size->row(), size->parent(), false);
        if (entrySize != mModel->item(size->row())->rowCount())
        {
            balanceChildren(size);
        }
    }
}

void DeconstructView::itemTypeChanged(QStandardItem * const type)
{
    // User selected custom, create new type
    if (type->text() == MainWindow::tr("custom"))
    {
        createCustomType(type);
    }
    // Create subitems if item type is custom
    else if (mTypesDelegate->getCustomItems().contains(type->text()))
    {
        balanceChildren(type);
    }
    // Is not custom, clean up children
    else
    {
        QModelIndex index = mModel->indexFromItem(type);
        QStandardItem * item = mModel->item(index.row());
        item->removeRows(0, item->rowCount());
    }
}

void DeconstructView::itemPreviewChanged(QStandardItem * const)
{

}

void DeconstructView::itemChanged(QStandardItem *item, bool selectAfter)
{
    if (!item)
    {
        return;
    }
    QStandardItem* parent = item->parent();
    if (parent)
    {
        QStandardItem* grandParent = parent->parent();
        if (grandParent)
        {
            QModelIndex grandParentIndex = mModel->indexFromItem(grandParent);
            QString grandParentTypeName = mModel->item(grandParentIndex.row(), cols::TYPE)->text();
            DataTypeCustom* grandParentType = DataTypeCustom::declaredTypes[grandParentTypeName];
            grandParentType->update(parent);
            balanceChildren(grandParent);
            return;
        }
        else // Is index number ex: [1]
        {
            return;
        }
    }

    QModelIndex index = mModel->indexFromItem(item);

    switch(index.column()){
    case cols::LABEL:
        itemLabelChanged(item);
        break;
    case cols::SIZE:
        itemSizeChanged(item);
        setPreview(index);
        break;
    case cols::TYPE:
        itemTypeChanged(item);
        setPreview(index);
        break;
    case cols::PREVIEW:
        itemPreviewChanged(item);
        return;
        break;
    default:
        break;
    }

    QStandardItem* rowRoot = mModel->item(item->row());
    // Create and set preview
    if (rowRoot && index.row() + 1 < rowRoot->rowCount())
    {
        QStandardItem *nextRow = mModel->item(index.row() + 1,
                                              index.column());
        itemChanged(nextRow, false);
    }

    if (selectAfter)
    {
        mSelectionModel->select(index,
                                QItemSelectionModel::ClearAndSelect |
                                QItemSelectionModel::Rows);
        selectionChanged();
    }
}

/*
 * Get text from size, split by operators,
 * substitute non-numbers and calculate
 */
int DeconstructView::getEntrySize(const int row, QStandardItem *parent,
                                  bool fromChildren) const
{
    if (!parent)
        parent = mModel->invisibleRootItem();
    if (row < 0)
        return 0;
    // Get the int representation of the size for the row
    QString text = parent->child(row, 1)->text();
    if (text.isEmpty()) {
        return 0;
    }
    int size = parseSizeExpression(text, row);
    if (fromChildren && mTypesDelegate->getCustomItems()
                        .contains(parent->child(row, cols::TYPE)->text()))
    {
        size *= getCoveredSize(size, parent->child(0));
    }
    return size;
}

/*
 * Removes extra children and adds missing entries
 *
 */
void DeconstructView::balanceChildren(QStandardItem * const item)
{
    int desiredRowCount = getEntrySize(item->row(), item->parent(), false);
    QStandardItem *parent = mModel->item(item->row());
    int parentRowCount = parent->rowCount();
    // Remove extra rows
    parent->removeRows(0, parentRowCount);
    // Add additional desired amount of rows back
    for (int i = 0; i < desiredRowCount; ++i)
    {
        // Displays index in for form of "[1]"
        QList<QStandardItem*> indexRow;
        indexRow << new QStandardItem(QString("[%1]").arg(i));
        indexRow << new QStandardItem();
        indexRow << new QStandardItem();
        indexRow << new QStandardItem();

        QString typeName = mModel->item(item->row(), cols::TYPE)->text();
        DataTypeCustom* customType = DataTypeCustom::declaredTypes[typeName];
        if (customType) {
            foreach (DataTypeBase* dataType, customType->getContents()) {
                // Copy of custom data type description
                QList<QStandardItem*> dataRow;
                dataRow << new QStandardItem(dataType->getLabel());
                dataRow << new QStandardItem(dataType->getSize());
                dataRow << new QStandardItem(dataType->getTypeName());
                dataRow << new QStandardItem();
                indexRow.first()->appendRow(dataRow);
            }
        }
        parent->appendRow(indexRow);
    }
    if (desiredRowCount)
        mTreeView->expand(parent->child(0)->index());
    mTreeView->expand(mModel->indexFromItem(item));
}

void DeconstructView::createCustomType(QStandardItem * const type)
{
    QString customType = QInputDialog::getText(
                mTreeView.get(), MainWindow::tr("Enter custom datatype name"),
                MainWindow::tr("Enter custom datatype name"));
    // Create custom type entry before modifying model
    delete DataTypeCustom::declaredTypes[customType];
    DataTypeCustom::declaredTypes[customType] = new DataTypeCustom(customType);
    // Modify model
    mTypesDelegate->addItem(customType);
    type->setText(customType);
}

void DeconstructView::setPreview(const QModelIndex &index)
{
    QStandardItem *preview = mModel->item(index.row(), cols::PREVIEW);
    int row = preview->row();
    QString type = mModel->item(row, cols::TYPE)->text();
    QString byteString = formatPreview(getCoveredSize(row),
                                       getEntrySize(row, preview->parent()),
                                       type);
    preview->setText(byteString);
}

/*
 * Basic operations are allowed:
 * products (*), not-equals (!=) and equals (==)
 * they are calculated with equals first, not-equals, second
 * and products last
 */
int DeconstructView::parseSizeExpression(const QString text,
                                         const int row) const
{
    return parseProduct(text, row);
}

int DeconstructView::parseProduct(const QString text, const int row) const
{
    QStringList elementList = text.split("*");
    int res = parseNequals(elementList.first(), row);
    for (int k = 1; k < elementList.length(); ++k)
    {
        QString element = elementList.at(k);
        res *= parseNequals(element, row);
    }
    return res;
}

int DeconstructView::parseEquals(const QString text, const int row) const
{
    QStringList elementList = text.split("==");
    int res = parseSizeElement(elementList.first(), row);
    for (int k = 1; k < elementList.length(); ++k)
    {
        QString element = elementList.at(k);
        res = res == parseSizeElement(element, row);
    }
    return res;
}

int DeconstructView::parseNequals(const QString text, const int row) const
{
    QStringList elementList = text.split("!=");
    int res = parseEquals(elementList.first(), row);
    for (int k = 1; k < elementList.length(); ++k)
    {
        QString element = elementList.at(k);
        res = res != parseEquals(element, row);
    }
    return res;
}

int DeconstructView::getCoveredSize(const int end, QStandardItem *parent) const
{
    if (!parent)
        parent = mModel->invisibleRootItem();
    // Get the number of bytes covered by schema in tree
    int coveredSize = 0;
    for (int i = 0; i < end && i < parent->rowCount(); ++i)
        coveredSize += getEntrySize(i, parent);
    return coveredSize;
}

QString DeconstructView::formatPreview(const int start, const int end,
                                       const QString &type) const
{
    const QByteArray byteString = mHexEdit->data().mid(start, end);
    std::unique_ptr<DataTypeBase> interp = DataTypeBase::getInterpreter(type);
    return interp? interp->format(byteString): "";
}

int DeconstructView::parseSizeElement(const QString text, const int row) const
{
    bool ok;
    int res = text.toInt(&ok, 0);
    if (ok) {
        return res;
    } else {
        return getSizeFromText(text, row);
    }
}

int DeconstructView::getSizeFromText(const QString text, const int end) const
{
    bool ok;
    int res = 0;
    const QString trimmed = text.trimmed();
    for (int i = end - 1; i >= 0; --i)
    {
        if(mModel->item(i, cols::LABEL)->text() == trimmed)
        {
            res = mModel->item(i, cols::PREVIEW)->text().toInt(&ok, 0);
            break;
        }
    }
    return res;
}

void DeconstructView::selectionChanged()
{
    QModelIndexList selectedRows = mSelectionModel->selectedRows();
    if (selectedRows.isEmpty())
    {
        mHexEdit->setSelection(0, 0);
        return;
    }
    int start = getCoveredSize(selectedRows.first().row());
    int size = getEntrySize(
                selectedRows.last().row(),
                mModel->itemFromIndex(selectedRows.last().parent()));
    if (selectedRows.length() > 1)
        size += getCoveredSize(selectedRows.last().row()) - start;
    mHexEdit->setSelection(start, start + size);
}

void DeconstructView::addRow(QString labelText,
                             QString sizeText,
                             QString typeText,
                             QStandardItem *rootItem)
{
    if (!rootItem)
        rootItem = mModel->invisibleRootItem();
    // Create new row(label, size, type, preview)
    QList<QStandardItem*> newRow;
    QStandardItem *label = new QStandardItem(labelText);
    newRow.push_back(label);
    // Default size is position of cursor on hex view
    if (sizeText.isEmpty()) {
        int size = getSelectionSize();
        sizeText = "0x" + QString::number(size, 16).toUpper();
    }
    newRow.push_back(new QStandardItem(sizeText));
    // Default datatype is bytes
    newRow.push_back(new QStandardItem(typeText));
    // Preview column is non-editable
    QStandardItem *preview = new QStandardItem();
    preview->setEditable(false);
    newRow.push_back(preview);
    // Add item and put in edit mode
    // Put as child of selected item if there is only one
    rootItem->appendRow(newRow);
    mTreeView->edit(mModel->indexFromItem(label));
}

void DeconstructView::removeRows(int start, int count)
{
    mModel->removeRows(start, count);
}

int DeconstructView::getSelectionSize() const
{
    // Find a selection size for the next field based on qHexedit cursor
    // position, if position is already covered, default to 0
    int curPosition = mHexEdit->cursorPosition();
    int coveredSize = getCoveredSize(mModel->rowCount());
    return (curPosition > coveredSize)? curPosition - coveredSize: 0;
}

void DeconstructView::refreshPreview()
{
    if (mModel->rowCount() > 0)
        itemChanged(mModel->item(0), false);
}
