#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <memory>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "comboboxdelegate.h"

#include "datatypebase.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new QStandardItemModel()),
    selectionModel(new QItemSelectionModel(model, this)),
    currentFile(QString())
{
    ui->setupUi(this);
    connect(ui->action_Open, SIGNAL(triggered()),
            this, SLOT(open()));
    connect(ui->action_Import_Schema, SIGNAL(triggered()),
            this, SLOT(loadSchema()));
    connect(ui->action_Export_Schema, SIGNAL(triggered()),
            this, SLOT(saveSchema()));
    connect(ui->action_Add, SIGNAL(triggered()),
            this, SLOT(addRow()));
    connect(model, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(itemChanged(QStandardItem*)));
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(selectionChanged()));
    setModel();
}

MainWindow::~MainWindow()
{
    delete model;
    delete ui;
}

void MainWindow::setModel()
{
    QStandardItem *lbl_label = new QStandardItem(tr("Label"));
    model->setHorizontalHeaderItem(cols::LABEL, lbl_label);

    QStandardItem *lbl_size = new QStandardItem(tr("Size"));
    model->setHorizontalHeaderItem(cols::SIZE, lbl_size);

    QStandardItem *lbl_type = new QStandardItem(tr("Type"));
    model->setHorizontalHeaderItem(cols::TYPE, lbl_type);
    typesDelegate = new ComboBoxDelegate(ui->treeView, defaultTypes);
    ui->treeView->setItemDelegateForColumn(cols::TYPE, typesDelegate);

    QStandardItem *lbl_prev = new QStandardItem(tr("Preview"));
    model->setHorizontalHeaderItem(cols::PREVIEW, lbl_prev);

    ui->treeView->setModel(model);
    ui->treeView->setSelectionModel(selectionModel);
    ui->treeView->setColumnWidth(cols::LABEL, 100);
    ui->treeView->setColumnWidth(cols::SIZE, 50);
    ui->treeView->setColumnWidth(cols::TYPE, 55);
    ui->treeView->setColumnWidth(cols::PREVIEW, 20);
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QString msg = tr("Cannot read file %1").arg(fileName);
        QMessageBox::warning(this, msg, msg + ":\n"
                             + file.errorString());
        return;
    }
    // TODO: OPTIMIZE: large files will consume a lot of memory
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->qHexEdit->setData(file.readAll());
    file.close();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    refreshPreview();
}

QString MainWindow::formatPreview(const int start, const int end, const QString &type) const
{
    const QByteArray byteString = ui->qHexEdit->data().mid(start, end);
    return DataTypeBase::getInterpreter(type)->format(byteString);
}

int MainWindow::getSizeFromText(const QString text, const int end) const
{
    bool ok;
    int res = 0;
    const QString trimmed = text.trimmed();
    for (int i = end - 1; i >= 0; --i)
    {
        if(model->item(i, cols::LABEL)->text() == trimmed)
        {
            res = model->item(i, cols::PREVIEW)->text().toInt(&ok, 0);
            break;
        }
    }
    return res;
}

int MainWindow::parseSizeElement(const QString text, const int row) const
{
    bool ok;
    int res = text.toInt(&ok, 0);
    if (ok) {
        return res;
    } else {
        return getSizeFromText(text, row);
    }
}

int MainWindow::parseEquals(const QString text, const int row) const
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

int MainWindow::parseNequals(const QString text, const int row) const
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

int MainWindow::parseProduct(const QString text, const int row) const
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

/*
 * Basic operations are allowed:
 * products (*), not-equals (!=) and equals (==)
 * they are calculated with equals first, not-equals, second
 * and products last
 */
int MainWindow::parseSizeExpression(const QString text, const int row) const
{
    return parseProduct(text, row);
}

/*
 * Get text from size, split by operators,
 * substitute non-numbers and calculate
 */
int MainWindow::getEntrySize(const int row, QStandardItem *parent, bool fromChildren) const
{
    if (!parent)
        parent = model->invisibleRootItem();
    if (row < 0)
        return 0;
    // Get the int representation of the size for the row
    QString text = parent->child(row, 1)->text();
    if (text.isEmpty()) {
        return 0;
    }
    int size = parseSizeExpression(text, row);
    if (fromChildren && typesDelegate->getCustomItems().contains(parent->child(row, cols::TYPE)->text()))
    {
        size *= getCoveredSize(size, parent->child(0));
    }
    return size;
}

int MainWindow::getCoveredSize(const int end, QStandardItem *parent) const
{
    if (!parent)
        parent = model->invisibleRootItem();
    // Get the number of bytes covered by schema in tree
    int coveredSize = 0;
    for (int i = 0; i < end && i < parent->rowCount(); ++i)
        coveredSize += getEntrySize(i, parent);
    return coveredSize;
}

int MainWindow::getSelectionSize() const
{
    // Find a selection size for the next field based on qHexedit cursor
    // position, if position is already covered, default to 0
    int curPosition = ui->qHexEdit->cursorPosition();
    int coveredSize = getCoveredSize(model->rowCount());
    return (curPosition > coveredSize)? curPosition - coveredSize: 0;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    currentFile = QFileInfo(fileName).canonicalFilePath();
//    setWindowModified(false);
//    setWindowFilePath(currentFile);
}

void MainWindow::setCurrentSchemaFile(const QString &fileName)
{
    currentSchemaFile = QFileInfo(fileName).canonicalFilePath();
    setWindowModified(false);
    setWindowFilePath(currentSchemaFile);
}

void MainWindow::refreshPreview()
{
    if (model->rowCount() > 0)
        itemChanged(model->item(0), false);
}

/*
 * Slots
 */

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(
                this, tr("Open binary file"));
    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

/*
 * Schema is in markdown table format
 */

void MainWindow::loadSchema()
{
    QString fileName = QFileDialog::getOpenFileName(
                this, tr("Open schema of binary file"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QString msg = tr("Cannot read file %1").arg(fileName);
        QMessageBox::warning(this, msg, msg + ":\n"
                             + file.errorString());
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // Clear treeView
    ui->treeView->setEnabled(true);
    model->removeRows(0, model->rowCount());

    // Read first two lines
    if (!file.atEnd())
        file.readLine(0);
    if (!file.atEnd())
        file.readLine(0);

    while (!file.atEnd())
    {
        QString line = QString::fromUtf8(file.readLine(0));
        QStringList lineElements = line.split('|');
        if (lineElements.length() < 3)
            continue;
        QString label = lineElements.at(cols::LABEL + 1).trimmed();
        QString size = lineElements.at(cols::SIZE + 1).trimmed();
        QString type = lineElements.at(cols::TYPE + 1).trimmed();
        addRow(label, size, type);
    }

    file.close();
    QApplication::restoreOverrideCursor();

    setCurrentSchemaFile(fileName);
    refreshPreview();
}

void MainWindow::saveSchema()
{
    QString fileName = QFileDialog::getSaveFileName(
                this, tr("Save schema of binary file"));
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QString msg = tr("Cannot write to file %1").arg(fileName);
        QMessageBox::warning(this, msg, msg + ":\n"
                             + file.errorString());
        return;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QString labelTitle = tr("Contents"),
            sizeTitle = tr("Size"),
            typeTitle = tr("Type"),
            descTitle = tr("Description (optional)");

    int labelLen = labelTitle.length(),
        sizeLen = sizeTitle.length(),
        typeLen = typeTitle.length(),
        descLen = descTitle.length();

    // Get max size for padding
    for (int i = 0; i < model->rowCount(); ++i)
    {
        labelLen = std::max(labelLen, model->item(i, cols::LABEL)->text().length());
        sizeLen = std::max(sizeLen, model->item(i, cols::SIZE)->text().length());
        typeLen = std::max(typeLen, model->item(i, cols::TYPE)->text().length());
    }
    const QString tableFormat = QString("| %1 | %2 | %3 | %4 |\n");
    const QString seperFormat = QString("|-%1-|-%2-|-%3-|-%4-|\n");

    out << tableFormat.arg(labelTitle, -labelLen)
                      .arg(sizeTitle, -sizeLen)
                      .arg(typeTitle, -typeLen)
                      .arg(descTitle, -descLen)
        << seperFormat.arg("", -labelLen, '-')
                      .arg("", -sizeLen, '-')
                      .arg("", -typeLen, '-')
                      .arg("", -descLen, '-');

    for (int i = 0; i < model->rowCount(); ++i)
    {
        out << tableFormat.arg(model->item(i, cols::LABEL)->text(), -labelLen)
                              .arg(model->item(i, cols::SIZE)->text(), -sizeLen)
                              .arg(model->item(i, cols::TYPE)->text(), -typeLen)
                              .arg("", -descLen);
    }

    file.close();
    QApplication::restoreOverrideCursor();

    setCurrentSchemaFile(fileName);
}

void MainWindow::addRow(QString labelText,
                        QString sizeText,
                        QString typeText,
                        QStandardItem *rootItem)
{
    if (currentFile.isEmpty())
        return;
    if (!rootItem)
        rootItem = model->invisibleRootItem();
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
    ui->treeView->edit(model->indexFromItem(label));
}

void MainWindow::itemChanged(QStandardItem *item, bool selectAfter)
{
    if (!item)
    {
        return;
    }
    QModelIndex index = model->indexFromItem(item);

    bool isOrphan = !item->parent();

    // If preview changed, ignore.
    if (index.column() == 3)
        return;
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
        break;
    default:
        break;
    }
    // Create and set preview
    QStandardItem *parent = model->item(item->row());
    if (index.row() + 1 < parent->rowCount())
    {
        QStandardItem *nextRow = model->item(index.row() + 1, index.column());
        itemChanged(nextRow, false);
    }

    if (selectAfter)
    {
        selectionModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        selectionChanged();
    }
}

void MainWindow::itemLabelChanged(QStandardItem * const label)
{

}

void MainWindow::itemSizeChanged(QStandardItem * const size)
{
    QStandardItem *type = model->item(size->row(), cols::TYPE);
    if (typesDelegate->getCustomItems().contains(type->text()))
    {
        int entrySize = getEntrySize(size->row(), size->parent(), false);
        if (entrySize != model->item(size->row())->rowCount())
            balanceChildren(size);
    }
}

void MainWindow::itemTypeChanged(QStandardItem * const type)
{
    // User selected custom, create new type
    if (type->text() == tr("custom"))
    {
        createCustomType(type);
    }
    // Create subitems if item type is custom
    if (typesDelegate->getCustomItems().contains(type->text()))
    {
        balanceChildren(type);
    }
    // Is not custom, clean up children
    else
    {
        QModelIndex index = model->indexFromItem(type);
        QStandardItem * item = model->item(index.row());
        item->removeRows(0, item->rowCount());
    }
}

void MainWindow::itemPreviewChanged(QStandardItem * const preview)
{
}

void MainWindow::setPreview(const QModelIndex& index)
{
    QStandardItem *preview = model->item(index.row(), cols::PREVIEW);
    int row = preview->row();
    QString type = model->item(row, cols::TYPE)->text();
    QString byteString = formatPreview(getCoveredSize(row),
                                       getEntrySize(row, preview->parent()),
                                       type);
    preview->setText(byteString);
}

void MainWindow::createCustomType(QStandardItem* const type)
{
    QString customType = QInputDialog::getText(
                this, tr("Enter custom datatype name"),
                tr("Enter custom datatype name"));
    typesDelegate->addItem(customType);
    type->setText(customType);
}

/*
 * Removes extra children and adds missing entries
 *
 */
void MainWindow::balanceChildren(QStandardItem* const item)
{
    QStandardItem *parent = model->item(item->row());
    parent->removeRows(0, parent->rowCount());
    // Add additional objects to list
    for (int i = parent->rowCount(); i < getEntrySize(item->row(), item->parent(), false); ++i)
    {
        QList<QStandardItem*> newItem = QList<QStandardItem*>();
        newItem.append(new QStandardItem(QString("[%1]").arg(i)));
        newItem.append(new QStandardItem());
        newItem.append(new QStandardItem());
        newItem.append(new QStandardItem());
        parent->appendRow(newItem);
        addRow("", "0x0", "bytes", newItem.at(0));
    }
    if (parent->rowCount())
        ui->treeView->expand(parent->child(0)->index());
    ui->treeView->expand(model->indexFromItem(item));
}

bool rowLessThan(const QModelIndex &lhs, const QModelIndex &rhs)
{
    return lhs.row() < rhs.row();
}

void MainWindow::selectionChanged()
{
    QModelIndexList selectedRows = selectionModel->selectedRows();
    if (selectedRows.isEmpty())
    {
        ui->qHexEdit->setSelection(0, 0);
        return;
    }
    int start = getCoveredSize(selectedRows.first().row());
    int size = getEntrySize(selectedRows.last().row(),
                            model->itemFromIndex(selectedRows.last().parent()));
    if (selectedRows.length() > 1)
        size += getCoveredSize(selectedRows.last().row()) - start;
    ui->qHexEdit->setSelection(start, start + size);
}
