#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QItemDelegate>

#include "RixJsonItemDelegate.h"
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
    void onMenuActionTrigger(QAction *action);
    void showTreeViewMenu(const QPoint &point);
    void aboutToShowEditMenu();
private:
    Ui::MainWindow *ui;
    QStandardItemModel *theModel;
    RixJsonItemDelegate theItemDelegate;
private:
    void updateTreeModel(Rix::Json::Object &o, QStandardItem *i);
    void updateTreeModel();

    void addRixJsonItem(bool asChild = false, QString key = "\"Key\"", QString value = "\"Value\"", Rix::Json::ValueType type = Rix::Json::ValueType::STRING);
    void deleteRixJsonItem();
    void expandAll();
    void collapseAll();
};
#endif // MAINWINDOW_H
