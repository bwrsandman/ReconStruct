#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class QComboBox;
class QStandardItemModel;

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
    QString currentFile;

    void setModel();
    void loadFile(const QString &fileName);
    QString formatBytes(const QByteArray& byteString) const;
    int getEntrySize(const int row) const;
    int getCoveredSize(const int end) const;
    int getSelectionSize() const;
    void setCurrentFile(const QString &fileName);

private slots:
    void open();
    void addRow();
};

#endif // MAINWINDOW_H
