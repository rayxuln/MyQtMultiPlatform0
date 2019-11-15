//
// Created by Raymond_Lx on 2019/11/13.
//

#ifndef MYQTAOO_RIXJSONITEMDELEGATE_H
#define MYQTAOO_RIXJSONITEMDELEGATE_H

#include <QStyledItemDelegate>

namespace Rix{
    namespace Json{
        // 用来指定MODEL储存的数据类型，这里用来储存显示的类型，以便分配不同的编辑器
        enum Role{
            Type = Qt::UserRole + 1
        };
        enum DisplayType{
            String,
            Real,
            Bool,
            VType
        };
    }
}

class RixJsonItemDelegate : public QStyledItemDelegate {
public:

    //Override func
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    //constructor/destructor
    RixJsonItemDelegate(QObject *parent = nullptr);
    virtual ~RixJsonItemDelegate();
};


#endif //MYQTAOO_RIXJSONITEMDELEGATE_H
