#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStack>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    theModel(new QStandardItemModel()),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    theModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("Key")));
    theModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Value")));
    theModel->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("Type")));

    ui->treeView->setModel(theModel);
    ui->treeView->setItemDelegate(&theItemDelegate);

    connect(theModel, &QStandardItemModel::itemChanged, this, &MainWindow::treeDataChanged);

    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveFile);
    connect(ui->saveAnotherButton, &QPushButton::clicked, this, &MainWindow::saveAnotherFile);

    connect(ui->menuEdit, &QMenu::triggered, this, &MainWindow::onMenuActionTrigger);
    connect(ui->menuFile, &QMenu::triggered, this, &MainWindow::onMenuActionTrigger);

    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::showTreeViewMenu);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->menuEdit, &QMenu::aboutToShow, this, &MainWindow::aboutToShowEditMenu);
}

void MainWindow::aboutToShowEditMenu() {
    auto index = ui->treeView->selectionModel()->currentIndex();
    ui->actionAddChild->setVisible(index.isValid() && (index.siblingAtColumn(2).data(Qt::DisplayRole) == "array" || index.siblingAtColumn(2).data(Qt::DisplayRole) == "object"));
}

void MainWindow::showTreeViewMenu(const QPoint &point) {
    ui->menuEdit->exec(QCursor::pos());
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

    if(item->column() == 0)// setting key
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
    }else if(item->column() == 1)// setting value
    {
        o->SetRawValue(item->text().toStdString());
    }else if(item->column() == 2)// setting type
    {
        auto t_index = theModel->indexFromItem(item);
        auto v_index = t_index.siblingAtColumn(1);
        auto k_index = t_index.siblingAtColumn(0);

        auto type = t_index.data();
        auto v_type = v_index.data(Rix::Json::Role::Type);
        if(type == "string" && v_type != Rix::Json::DisplayType::String)
        {
            theModel->setData(v_index, tr("\"\""));
            theModel->setData(v_index, Rix::Json::DisplayType::String, Rix::Json::Role::Type);
            theModel->itemFromIndex(v_index)->setEditable(true);
            o->SetType(Rix::Json::ValueType::STRING);
            theModel->removeRows(0, theModel->rowCount(k_index), k_index);
        }else if(type == "real" && v_type != Rix::Json::DisplayType::Real)
        {
            theModel->setData(v_index, tr("0"));
            theModel->setData(v_index, Rix::Json::DisplayType::Real, Rix::Json::Role::Type);
            theModel->itemFromIndex(v_index)->setEditable(true);
            o->SetType(Rix::Json::ValueType::REAL);
            theModel->removeRows(0, theModel->rowCount(k_index), k_index);
        }else if(type == "boolean" && v_type != Rix::Json::DisplayType::Bool)
        {
            theModel->setData(v_index, tr("false"));
            theModel->setData(v_index, Rix::Json::DisplayType::Bool, Rix::Json::Role::Type);
            theModel->itemFromIndex(v_index)->setEditable(true);
            o->SetType(Rix::Json::ValueType::BOOLEAN);
            theModel->removeRows(0, theModel->rowCount(k_index), k_index);
        }else if(type == "array" && o->GetType() != Rix::Json::ValueType::ARRAY)
        {
            o->SetType(Rix::Json::ValueType::ARRAY);
            updateTreeModel();
        }else if(type == "object" && o->GetType() != Rix::Json::ValueType::OBJECT)
        {
            o->SetType(Rix::Json::ValueType::OBJECT);
            updateTreeModel();
        }
    }

    DataManager::Instance()->setDirty(true);

    ui->textBrowser->setText(tr(DataManager::Instance()->getCurrentJsonObject().ToStr(true).c_str()));


    setWindowTitle(tr("RixJsonEditor | ") + DataManager::Instance()->getFileName() + tr("*"));
}

void MainWindow::openFile() {
    auto fileName = QFileDialog::getOpenFileName(
            this,
            tr("请选择一个JSON文件"),
            QString(),
            tr("JSON File(*.json);;Text File(*.txt);;All File(*.*)")
            );
    bool success = false;
    try {
        success = DataManager::Instance()->loadFromFile(fileName);
    }catch (Rix::Json::ParserException e)
    {
        QMessageBox theMsgBox(this);
        theMsgBox.setWindowTitle("You know what");
        theMsgBox.setText(tr("Loading JSON file fail.\n") + tr(e.what()));
        theMsgBox.exec();
        return;
    }
    if(!success)
        return;


    updateTreeModel();

    ui->textBrowser->setText(tr(DataManager::Instance()->getCurrentJsonObject().ToStr(true).c_str()));
    setWindowTitle(tr("RixJsonEditor") + (fileName.size() > 0 ? tr(" | ") + fileName.split('/').last() : QString()));
}
void MainWindow::saveFile() {
    if(!DataManager::Instance()->currentFileExists())
    {
        saveAnotherFile();
        return;
    }

    bool success = DataManager::Instance()->saveToFile();
    if(!success)
        return;
    DataManager::Instance()->setDirty(false);
    setWindowTitle(tr("RixJsonEditor | ") + DataManager::Instance()->getFileName());
}
void MainWindow::saveAnotherFile() {
    auto fileName = QFileDialog::getSaveFileName(
            this,
            tr("请选择需要保存的文件"),
            QString(),
            tr("JSON File(*.json);;Text File(*.txt);;All File(*.*)")
            );
    bool success = DataManager::Instance()->saveToFile(fileName);
    if(!success)
        return;
    DataManager::Instance()->setDirty(false);
    setWindowTitle(tr("RixJsonEditor | ") + DataManager::Instance()->getFileName());
}
void MainWindow::updateTreeModel() {
    theModel->clear();
    theModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("Key")));
    theModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Value")));
    theModel->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("Type")));

    auto &rO = DataManager::Instance()->getCurrentJsonObject();
    auto &os = rO.GetChildren();
    QRegExp rm("\"(.*)\"");
    for(auto &o : os)
    {
        if(o.GetType() == Rix::Json::ValueType::STRING || o.GetType() == Rix::Json::ValueType::REAL || o.GetType() == Rix::Json::ValueType::BOOLEAN)
        {
            auto i0 = new QStandardItem(o.GetKey().c_str());
            i0->setData(Rix::Json::DisplayType::String, Rix::Json::Role::Type);
            if(o.GetKey().size() <= 2) i0->setEditable(false);
            auto i1 = new QStandardItem(o.GetRawValue().c_str());
            auto i2 = new QStandardItem();
            i2->setData(Rix::Json::DisplayType::VType, Rix::Json::Role::Type);
            switch(o.GetType())
            {
                case Rix::Json::ValueType::REAL:
                    i2->setText("real");
                    i1->setData(Rix::Json::DisplayType::Real, Rix::Json::Role::Type);
                    break;
                case Rix::Json::ValueType::STRING:
                    i2->setText("string");
                    i1->setData(Rix::Json::DisplayType::String, Rix::Json::Role::Type);
                    break;
                case Rix::Json::ValueType::BOOLEAN:
                    i2->setText("boolean");
                    i1->setData(Rix::Json::DisplayType::Bool, Rix::Json::Role::Type);
                    break;
                default:
                    i2->setText("unknown");
                    break;
            }
            QList<QStandardItem*> i;
            i.append(i0);
            i.append(i1);
            i.append(i2);
            theModel->appendRow(i);
        }else
        {
            auto i = new QStandardItem(o.GetKey().c_str());
            i->setData(Rix::Json::DisplayType::String, Rix::Json::Role::Type);
            auto ci = new QStandardItem();
            ci->setEditable(false);
            auto i2 = new QStandardItem();
            i2->setData(Rix::Json::DisplayType::VType, Rix::Json::Role::Type);
            switch(o.GetType())
            {
                case Rix::Json::ValueType::ARRAY:
                    i2->setText("array");
                    break;
                case Rix::Json::ValueType::OBJECT:
                    i2->setText("object");
                    break;
                default:
                    i2->setText("unknown");
                    break;
            }
            theModel->appendRow({i, ci, i2});
            updateTreeModel(o, i);
        }
    }
}
void MainWindow::updateTreeModel(Rix::Json::Object &po, QStandardItem *pi) {
    auto &os = po.GetChildren();
    QRegExp rm("\"(.*)\"");
    for(auto &o : os)
    {
        if(o.GetType() == Rix::Json::ValueType::STRING || o.GetType() == Rix::Json::ValueType::REAL || o.GetType() == Rix::Json::ValueType::BOOLEAN)
        {
            auto i0 = new QStandardItem(o.GetKey().c_str());
            i0->setData(Rix::Json::DisplayType::String, Rix::Json::Role::Type);
            if(po.GetType() == Rix::Json::ValueType::ARRAY)
            {
                i0->setEditable(false);
                i0->setText(QString());
            }
            auto i1 = new QStandardItem(o.GetRawValue().c_str());
            auto i2 = new QStandardItem();
            i2->setData(Rix::Json::DisplayType::VType, Rix::Json::Role::Type);
            switch(o.GetType())
            {
                case Rix::Json::ValueType::REAL:
                    i2->setText("real");
                    i1->setData(Rix::Json::DisplayType::Real, Rix::Json::Role::Type);
                    break;
                case Rix::Json::ValueType::STRING:
                    i2->setText("string");
                    i1->setData(Rix::Json::DisplayType::String, Rix::Json::Role::Type);
                    break;
                case Rix::Json::ValueType::BOOLEAN:
                    i2->setText("boolean");
                    i1->setData(Rix::Json::DisplayType::Bool, Rix::Json::Role::Type);
                    break;
                default:
                    i2->setText("unknown");
                    break;
            }
            QList<QStandardItem*> i;
            i.append(i0);
            i.append(i1);
            i.append(i2);
            pi->appendRow(i);
        }else
        {
            auto i = new QStandardItem(o.GetKey().c_str());
            i->setData(Rix::Json::DisplayType::String, Rix::Json::Role::Type);
            if(po.GetType() == Rix::Json::ValueType::ARRAY)
            {
                i->setEditable(false);
                i->setText(QString());
            }
            auto ci = new QStandardItem();
            ci->setEditable(false);
            auto i2 = new QStandardItem();
            i2->setData(Rix::Json::DisplayType::VType, Rix::Json::Role::Type);
            switch(o.GetType())
            {
                case Rix::Json::ValueType::ARRAY:
                    i2->setText("array");
                    break;
                case Rix::Json::ValueType::OBJECT:
                    i2->setText("object");
                    break;
                default:
                    i2->setText("unknown");
                    break;
            }
            pi->appendRow({i, ci, i2});
            updateTreeModel(o, i);
        }
    }
}

void MainWindow::onMenuActionTrigger(QAction *action) {
    if(action == ui->actionAdd_item)
    {
        addRixJsonItem(false);
    }else if(action == ui->actionAddChild)
    {
        addRixJsonItem(true);
    }else if(action == ui->actionExpand_All)
    {
        expandAll();
    }else if(action == ui->actionCollaps_All)
    {
        collapseAll();
    }else if(action == ui->actionDelete)
    {
        deleteRixJsonItem();
    } else if(action == ui->actionQuit)
    {
        exit(0);
    }else if(action == ui->actionOpen_File)
    {
        openFile();
    }else if(action == ui->actionSave_File)
    {
        saveFile();
    }else if(action == ui->actionSave_as)
    {
        saveAnotherFile();
    }
}
void MainWindow::deleteRixJsonItem() {
    auto index = ui->treeView->selectionModel()->currentIndex();
    if(!index.isValid()) return;
    auto item = theModel->itemFromIndex(index);

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

    if(p != nullptr)
    {
        auto &os = p->GetChildren();
        os.erase(os.begin() + index.row());

        updateTreeModel();
        ui->textBrowser->setText(tr(DataManager::Instance()->getCurrentJsonObject().ToStr(true).c_str()));

        DataManager::Instance()->setDirty(true);
        setWindowTitle(tr("RixJsonEditor | ") + DataManager::Instance()->getFileName() + tr("*"));
    }
}
void MainWindow::addRixJsonItem(bool asChild, QString key, QString value, Rix::Json::ValueType type) {
    auto index = ui->treeView->selectionModel()->currentIndex();

    Rix::Json::Object new_o;
    new_o.SetKey(key.toStdString());
    new_o.SetRawValue(value.toStdString());
    new_o.SetType(type);

    if(!index.isValid())
    {
        auto &o = DataManager::Instance()->getCurrentJsonObject();
        o.SetType(Rix::Json::ValueType::OBJECT);
        o.AddChild(new_o);

        updateTreeModel();
        ui->textBrowser->setText(tr(DataManager::Instance()->getCurrentJsonObject().ToStr(true).c_str()));

        DataManager::Instance()->setDirty(true);
        setWindowTitle(tr("RixJsonEditor | ") + DataManager::Instance()->getFileName() + tr("*"));
        return;
    }
    auto item = theModel->itemFromIndex(index);

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



    if(asChild)
    {
        o->AddChild(new_o);
    } else{
        if(p != nullptr)
        {
            auto &os = p->GetChildren();
            auto pos = index.row();
            os.insert(os.begin() + pos + 1, new_o);
        }
    }


    updateTreeModel();
    ui->textBrowser->setText(tr(DataManager::Instance()->getCurrentJsonObject().ToStr(true).c_str()));

    DataManager::Instance()->setDirty(true);
    setWindowTitle(tr("RixJsonEditor | ") + DataManager::Instance()->getFileName() + tr("*"));
}
void MainWindow::expandAll() {
    ui->treeView->expandAll();
}
void MainWindow::collapseAll() {
    ui->treeView->collapseAll();
}

MainWindow::~MainWindow()
{
    delete ui;
}

