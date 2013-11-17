#include <QFileDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QStandardItemModel>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "comboboxdelegate.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new QStandardItemModel()),
    currentFile(QString())
{
    ui->setupUi(this);
    connect(ui->action_Open, SIGNAL(triggered()),
            this, SLOT(open()));
    connect(ui->action_Add, SIGNAL(triggered()),
            this, SLOT(addRow()));;
    setModel();
    ui->treeView->setEnabled(false);
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
    ui->treeView->setColumnWidth(0, 75);
    ui->treeView->setColumnWidth(1, 50);
    ui->treeView->setColumnWidth(2, 55);
    ui->treeView->setColumnWidth(3, 50);
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
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    // Clear treeView
    ui->treeView->setEnabled(true);
    model->removeRows(0, model->rowCount());
}

QString MainWindow::formatBytes(const QByteArray& byteString) const
{
    if (byteString.isEmpty())
            return QString();
    return "0x" + QString(byteString.toHex()).toUpper();

}

int MainWindow::getEntrySize(const int row) const
{
    // Get the int representation of the size for the row
    QString text = model->item(row, 1)->text();
    return text.toInt(0, 16);
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
    setWindowModified(false);
    setWindowFilePath(currentFile);
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

void MainWindow::addRow()
{
    if (currentFile.isEmpty())
        return;
    QModelIndexList selectedItems = ui->treeView->selectionModel()
                                                 ->selectedRows();
    // Create new row(label, size, type, preview)
    QList<QStandardItem*> newRow;
    QStandardItem *label = new QStandardItem();
    newRow.push_back(label);
    // Default size is position of cursor on hex view
    int size = getSelectionSize();
    QString hexNumber = "0x" + QString::number(size, 16).toUpper();
    newRow.push_back(new QStandardItem(hexNumber));
    // Default datatype is bytes
    newRow.push_back(new QStandardItem("bytes"));
    // Preview column is non-editable
    QString byteString = formatBytes(ui->qHexEdit->data().mid(
                                     getCoveredSize(model->rowCount()),
                                     size));
    QStandardItem *preview = new QStandardItem(byteString);
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
