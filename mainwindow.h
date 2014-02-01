#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include <memory>

namespace Ui {
class MainWindow;
}
class QStandardItem;
class DeconstructView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<DeconstructView> deconstructView;
    QString currentFile;
    QString currentSchemaFile;

    void loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void setCurrentSchemaFile(const QString &fileName);

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
