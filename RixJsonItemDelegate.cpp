//
// Created by Raymond_Lx on 2019/11/13.
//

#include "RixJsonItemDelegate.h"

#include <QLineEdit>
#include <QComboBox>
#include <QDebug>


QWidget *RixJsonItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QWidget *editor = nullptr;
    auto type = (Rix::Json::DisplayType) index.data(Rix::Json::Role::Type).toInt();
    switch (type)
    {
        case Rix::Json::DisplayType::Real:
        case Rix::Json::DisplayType::String:
            editor = new QLineEdit(parent);
            break;
        case Rix::Json::DisplayType::Bool:
        {
            auto b = new QComboBox(parent);
            b->addItems({"true", "false"});

            editor = b;
            break;
        }
        case Rix::Json::DisplayType::VType:
        {
            auto b = new QComboBox(parent);
            b->addItems({"string", "real", "boolean", "array", "object"});

            editor = b;
            break;
        }
    }

    return  editor;
}

void RixJsonItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    auto type = (Rix::Json::DisplayType) index.data(Rix::Json::Role::Type).toInt();

    switch(type)
    {
        case Rix::Json::DisplayType::String:
        {
            auto e = (QLineEdit*) editor;
            auto text = index.data().toString();
            QRegExp rm("\"(.*)\"");
            e->setText(text.replace(rm, "\\1"));
            break;
        }
        case Rix::Json::DisplayType::Real:
        {
            auto e = (QLineEdit*) editor;
            e->setText(index.data().toString());
            break;
        }
        case Rix::Json::DisplayType::VType:
        case Rix::Json::DisplayType::Bool:
        {
            auto b = (QComboBox*) editor;
            b->setCurrentText(index.data().toString());
            break;
        }
    }
}

void RixJsonItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    auto type = (Rix::Json::DisplayType) index.data(Rix::Json::Role::Type).toInt();

    switch(type)
    {
        case Rix::Json::DisplayType::String:
        {
            auto e = (QLineEdit*) editor;
            model->setData(index, tr("\"%1\"").arg(e->text()));
            break;
        }
        case Rix::Json::DisplayType::Real:
        {
            auto e = (QLineEdit*) editor;
            model->setData(index, e->text().toDouble());
            break;
        }
        case Rix::Json::DisplayType::VType:
        case Rix::Json::DisplayType::Bool:
        {
            auto b = (QComboBox*) editor;
            model->setData(index, b->currentText());
            break;
        }
    }
}

void RixJsonItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    editor->setGeometry(option.rect);
}


RixJsonItemDelegate::RixJsonItemDelegate(QObject *p): QStyledItemDelegate(p) {

}

RixJsonItemDelegate::~RixJsonItemDelegate() {

}