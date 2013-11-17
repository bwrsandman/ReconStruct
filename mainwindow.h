#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>

namespace Ui {
class MainWindow;
}
class QComboBox;
class QStandardItem;
class QStandardItemModel;
class QItemSelectionModel;

const QStringList defaultTypes = QStringList() << "bytes" << "str"
                                               << "int" << "bool" ;
const QStringList columns = QStringList() << "Label" << "Size"
                                          << "Type" << "Preview";

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QItemSelectionModel* selectionModel;
    QString currentFile;

    void setModel();
    void loadFile(const QString &fileName);
    QString formatPreview(const int start, const int end,
                          const QString& type) const;
    QString formatBytes(const QByteArray& byteString) const;
    QString formatStr(const QByteArray& byteString) const;
    QString formatInt(const QByteArray& byteString) const;
    QString formatBool(const QByteArray& byteString) const;
    int getEntrySize(const int row) const;
    int getCoveredSize(const int end) const;
    int getSelectionSize() const;
    void setCurrentFile(const QString &fileName);

private slots:
    void open();
    void addRow();
    void itemChanged(QStandardItem*, bool selectAfter=true);
    void selectionChanged();
};

#endif // MAINWINDOW_H
