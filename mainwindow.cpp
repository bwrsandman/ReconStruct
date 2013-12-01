#include <QFileDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QStandardItemModel>
#include <QItemSelectionModel>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "comboboxdelegate.h"

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
    model->setHorizontalHeaderItem(0, lbl_label);

    QStandardItem *lbl_size = new QStandardItem(tr("Size"));
    model->setHorizontalHeaderItem(1, lbl_size);

    QStandardItem *lbl_type = new QStandardItem(tr("Type"));
    model->setHorizontalHeaderItem(2, lbl_type);
    ui->treeView->setItemDelegateForColumn(2, new ComboBoxDelegate(ui->treeView, defaultTypes));

    QStandardItem *lbl_prev = new QStandardItem(tr("Preview"));
    model->setHorizontalHeaderItem(3, lbl_prev);

    ui->treeView->setModel(model);
    ui->treeView->setSelectionModel(selectionModel);
    ui->treeView->setColumnWidth(0, 100);
    ui->treeView->setColumnWidth(1, 50);
    ui->treeView->setColumnWidth(2, 55);
    ui->treeView->setColumnWidth(3, 20);
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
    QByteArray byteString = ui->qHexEdit->data().mid(start, end);
    QString previewText;
    if (type == "bytes") {
        previewText = formatBytes(byteString);
    } else if (type == "str"){
        previewText = formatStr(byteString);
    } else if (type == "int"){
        previewText = formatInt(byteString);
    } else if (type == "bool"){
        previewText = formatBool(byteString);
    } else {
        previewText = tr("TODO: Preview unimplemented (%1)")
                        .arg(formatBytes(byteString));
    }
    return previewText;
}

QString MainWindow::formatBytes(const QByteArray& byteString) const
{
    if (byteString.isEmpty())
            return QString();
    return "0x" + QString(byteString.toHex()).toUpper();

}

QString MainWindow::formatStr(const QByteArray& byteString) const
{
    if (byteString.isEmpty())
            return QString();
    return QString(byteString);

}

QString MainWindow::formatInt(const QByteArray& byteString) const
{
    if (byteString.isEmpty())
            return QString();
    bool ok = true;
    // Reverse byte order
    QByteArray reverse = QByteArray(byteString.length(), '\0');
    std::reverse_copy(byteString.constBegin(), byteString.constEnd(),
                      reverse.begin());
    QString ret = QString::number(reverse.toHex().toInt(&ok, 16));
    return ok? ret: tr("Error");

}

QString MainWindow::formatBool(const QByteArray& byteString) const
{
    if (byteString.isEmpty())
            return QString("false");
    bool ok = true;
    QString ret = (byteString.toHex().toInt(&ok, 16))? "true" : "false";
    return ok? ret: tr("Error");

}

int MainWindow::getSizeFromText(const QString text, const int end) const
{
    bool ok;
    int res = 0;
    const QString trimmed = text.trimmed();
    for (int i = end - 1; i >= 0; --i)
    {
        if(model->item(i, 0)->text() == trimmed)
        {
            res = model->item(i, 3)->text().toInt(&ok, 0);
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
int MainWindow::getEntrySize(const int row) const
{
    // Get the int representation of the size for the row
    QString text = model->item(row, 1)->text();
    if (text.isEmpty()) {
        return 0;
    }
    return parseSizeExpression(text, row);
}

int MainWindow::getCoveredSize(const int end) const
{
    // Get the number of bytes covered by schema in tree
    int coveredSize = 0;
    for (int i = 0; i < end; ++i)
        coveredSize += getEntrySize(i);
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
        QString label = lineElements.at(1).trimmed();
        QString size = lineElements.at(2).trimmed();
        QString type = lineElements.at(3).trimmed();
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
        labelLen = std::max(labelLen, model->item(i, 0)->text().length());
        sizeLen = std::max(sizeLen, model->item(i, 1)->text().length());
        typeLen = std::max(typeLen, model->item(i, 2)->text().length());
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
        out << tableFormat.arg(model->item(i, 0)->text(), -labelLen)
                              .arg(model->item(i, 1)->text(), -sizeLen)
                              .arg(model->item(i, 2)->text(), -typeLen)
                              .arg("", -descLen);
    }

    file.close();
    QApplication::restoreOverrideCursor();

    setCurrentSchemaFile(fileName);
}

void MainWindow::addRow(QString labelText,
                        QString sizeText,
                        QString typeText)
{
    if (currentFile.isEmpty())
        return;
    QModelIndexList selectedItems = ui->treeView->selectionModel()
                                                 ->selectedRows();
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
//    if (selectedItems.length() == 1) {
//        ui->treeView->expand(selectedItems[0]);
//        model->itemFromIndex(selectedItems[0])->appendRow(newRow);
//    } else {
        model->appendRow(newRow);
//    }
    ui->treeView->edit(model->indexFromItem(label));
}

void MainWindow::itemChanged(QStandardItem *item, bool selectAfter)
{
    QModelIndex index = model->indexFromItem(item);
    if (index.column() == 3)
        return;
    QStandardItem *size = model->item(index.row(), 1);
    QStandardItem *type = model->item(index.row(), 2);
    QStandardItem *preview = model->item(index.row(), 3);
    QString byteString = formatPreview(getCoveredSize(index.row()),
                                       getEntrySize(index.row()),
                                       type->text());
    preview->setText(byteString);
    // Cascade changes to following rows
    if (index.row() + 1 < model->rowCount())
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
    int size = getEntrySize(selectedRows.last().row());
    if (selectedRows.length() > 1)
        size += getCoveredSize(selectedRows.last().row()) - start;
    ui->qHexEdit->setSelection(start, start + size);
}
