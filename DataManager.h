//
// Created by Raymond_Lx on 2019/11/10.
//

#ifndef MYQTAOO_DATAMANAGER_H
#define MYQTAOO_DATAMANAGER_H

#include <QString>
#include <QUrl>

#include "RixJson/RixJson.h"


class DataManager {
private:
    DataManager();

protected:
    Rix::Json::Object currentJsonObject;
    QUrl currentJsonFileUrl;
    bool dirty;
public:
    bool loadFromFile(const QString &path);
    bool saveToFile();
    bool saveToFile(const QString &path);
    bool currentFileExists();

    void setDirty(bool _d);
    bool isDirty();

    QString getFileName();

    Rix::Json::Object &getCurrentJsonObject();

    static DataManager *Instance();
};


#endif //MYQTAOO_DATAMANAGER_H
