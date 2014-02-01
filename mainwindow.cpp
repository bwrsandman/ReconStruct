#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <memory>

#include "comboboxdelegate.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "deconstructview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
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
    deconstructView.reset(new DeconstructView(ui->treeView,
                                              ui->qHexEdit));
    connect(deconstructView->getModelPtr(),
            SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(itemChanged(QStandardItem*)));
    connect(deconstructView->getSelectionModelPtr(),
            SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(selectionChanged()));
}

MainWindow::~MainWindow()
{
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
    deconstructView->refreshPreview();
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
    deconstructView->setEnabled(true);
    deconstructView->clearRows();

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
    deconstructView->refreshPreview();
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
    for (int i = 0; i < deconstructView->rowCount(); ++i)
    {
        labelLen = std::max(labelLen,
                            deconstructView->labelAt(i).length());
        sizeLen = std::max(sizeLen,
                           deconstructView->sizeAt(i).length());
        typeLen = std::max(typeLen,
                           deconstructView->typeAt(i).length());
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

    for (int i = 0; i < deconstructView->rowCount(); ++i)
    {
        out << tableFormat.arg(deconstructView->labelAt(i), -labelLen)
                              .arg(deconstructView->sizeAt(i), -sizeLen)
                              .arg(deconstructView->typeAt(i), -typeLen)
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
    deconstructView->addRow(labelText, sizeText, typeText, rootItem);
}

void MainWindow::itemChanged(QStandardItem *item, bool selectAfter)
{
    deconstructView->itemChanged(item, selectAfter);
}

bool rowLessThan(const QModelIndex &lhs, const QModelIndex &rhs)
{
    return lhs.row() < rhs.row();
}

void MainWindow::selectionChanged()
{
    deconstructView->selectionChanged();
}
