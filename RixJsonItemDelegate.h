//
// Created by Raymond_Lx on 2019/11/13.
//

#ifndef MYQTAOO_RIXJSONITEMDELEGATE_H
#define MYQTAOO_RIXJSONITEMDELEGATE_H

#include <QStyledItemDelegate>

namespace Rix{
    namespace Json{
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
