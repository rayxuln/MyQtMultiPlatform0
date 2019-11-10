#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QStack>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    theModel(new QStandardItemModel()),
    theDelegate(nullptr),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    theModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("Key")));
    theModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Value")));

    ui->treeView->setModel(theModel);
    theDelegate = ui->treeView->itemDelegate();



    connect(theModel, &QStandardItemModel::itemChanged, this, &MainWindow::treeDataChanged);

    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveFile);
    connect(ui->saveAnotherButton, &QPushButton::clicked, this, &MainWindow::saveAnotherFile);
}

void MainWindow::treeDataChanged(QStandardItem * item)
{
    QStack<QModelIndex> s;
    QModelIndex i;
    if(item->column() == 0)
    {
        i = theModel->indexFromItem(item);
    }
    else{
        i = theModel->indexFromItem(item).siblingAtColumn(0);
    }
    do
    {
        s.push(i);
        i = i.parent();
    }while (i.isValid());

    auto o = &DataManager::Instance()->getCurrentJsonObject();
    Rix::Json::Object *p = nullptr;
    while(!s.isEmpty())
    {
        auto index = s.pop();
        p = o;
        o = &o->GetChild(index.row());
    }

    if(item->column() == 0)
    {
        o->SetKey(item->text().toStdString());

        if(p != nullptr)
        {
            auto &cs = p->GetChildren();
            bool isArray = true;
            for(auto &c : cs)
            {
                if(c.GetKey().size() > 0) isArray = false;
            }
            if(isArray) p->SetType(Rix::Json::ValueType::ARRAY);
            else p->SetType(Rix::Json::ValueType::OBJECT);
        }
    }else
    {
        o->SetRawValue(item->text().toStdString());
    }

    ui->textBrowser->setText(tr(DataManager::Instance()->getCurrentJsonObject().ToStr(true).c_str()));
}

void MainWindow::openFile() {
    auto fileName = QFileDialog::getOpenFileName(
            this,
            tr("请选择一个JSON文件"),
            QString(),
            tr("JSON File(*.json);;Text File(*.txt);;All File(*.*)")
            );
    DataManager::Instance()->loadFromFile(fileName);

    auto &rO = DataManager::Instance()->getCurrentJsonObject();
    auto &os = rO.GetChildren();
    for(auto &o : os)
    {
        if(o.GetType() == Rix::Json::ValueType::STRING || o.GetType() == Rix::Json::ValueType::REAL)
        {
            auto i0 = new QStandardItem(o.GetKey().c_str());
            auto i1 = new QStandardItem(o.GetRawValue().c_str());
            QList<QStandardItem*> i;
            i.append(i0);
            i.append(i1);
            theModel->appendRow(i);
        }else
        {
            auto i = new QStandardItem(o.GetKey().c_str());
            auto ci = new QStandardItem();
            ci->setEditable(false);
            theModel->appendRow({i, ci});
            updateTreeModel(o, i);
        }
    }

    ui->textBrowser->setText(tr(rO.ToStr(true).c_str()));


    setWindowTitle(fileName);
}
void MainWindow::saveFile() {
    DataManager::Instance()->saveToFile();
}
void MainWindow::saveAnotherFile() {
    auto fileName = QFileDialog::getSaveFileName(
            this,
            tr("请选择需要保存的文件"),
            QString(),
            tr("JSON File(*.json);;Text File(*.txt);;All File(*.*)")
            );
    DataManager::Instance()->saveToFile(fileName);
}

void MainWindow::updateTreeModel(Rix::Json::Object &po, QStandardItem *pi) {
    auto os = po.GetChildren();
    for(auto &o : os)
    {
        if(o.GetType() == Rix::Json::ValueType::STRING || o.GetType() == Rix::Json::ValueType::REAL)
        {
            auto i0 = new QStandardItem(o.GetKey().c_str());
            auto i1 = new QStandardItem(o.GetRawValue().c_str());
            QList<QStandardItem*> i;
            i.append(i0);
            i.append(i1);
            pi->appendRow(i);
        }else
        {
            auto i = new QStandardItem(o.GetKey().c_str());
            auto ci = new QStandardItem();
            ci->setEditable(false);
            pi->appendRow({i, ci});
            updateTreeModel(o, i);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

