#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include <memory>

namespace Ui {
class MainWindow;
}
class QComboBox;
class QStandardItem;
class QStandardItemModel;
class QItemSelectionModel;
class ComboBoxDelegate;

const QStringList defaultTypes = QStringList() << "bytes" << "str"
                                               << "int" << "bool";
const QStringList columns = QStringList() << "Label" << "Size"
                                          << "Type" << "Preview";
namespace cols {
enum {
    LABEL, SIZE, TYPE, PREVIEW
};
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<QStandardItemModel> model;
    std::unique_ptr<QItemSelectionModel> selectionModel;
    std::unique_ptr<ComboBoxDelegate> typesDelegate;
    QString currentFile;
    QString currentSchemaFile;

    void setModel();
    void loadFile(const QString &fileName);
    QString formatPreview(const int start, const int end,
                          const QString& type) const;
    void itemLabelChanged(QStandardItem * const);
    void itemSizeChanged(QStandardItem * const);
    void itemTypeChanged(QStandardItem * const);
    void itemPreviewChanged(QStandardItem * const);
    void setPreview(const QModelIndex& index);
    void createCustomType(QStandardItem* const type);
    void balanceChildren(QStandardItem* const item);
    int getSizeFromText(const QString text, const int end) const;
    int getEntrySize(const int row, QStandardItem *parent, bool fromChildren=true) const;
    int parseSizeElement(const QString text, const int row) const;
    int parseSizeExpression(const QString text, const int row) const;
    int parseProduct(const QString text, const int row) const;
    int parseNequals(const QString text, const int row) const;
    int parseEquals(const QString text, const int row) const;
    int getCoveredSize(const int end, QStandardItem *parent=0) const;
    int getSelectionSize() const;
    void setCurrentFile(const QString &fileName);
    void setCurrentSchemaFile(const QString &fileName);
    void refreshPreview();

private slots:
    void open();
    void loadSchema();
    void saveSchema();
    void addRow(QString labelText=QString(),
                QString sizeText=QString(),
                QString typeText=QString("bytes"),
                QStandardItem *rootItem=0);
    void itemChanged(QStandardItem*, bool selectAfter=true);
    void selectionChanged();
};

#endif // MAINWINDOW_H
