#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

#include "DataManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void treeDataChanged(QStandardItem *);
    void openFile();
    void saveFile();
    void saveAnotherFile();
private:
    Ui::MainWindow *ui;
    QStandardItemModel *theModel;
private:
    void updateTreeModel(Rix::Json::Object &o, QStandardItem *i);
};
#endif // MAINWINDOW_H
