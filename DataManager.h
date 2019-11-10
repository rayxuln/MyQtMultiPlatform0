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
    void loadFromFile(const QString &path);
    void saveToFile();
    void saveToFile(const QString &path);

    Rix::Json::Object &getCurrentJsonObject();

    static DataManager *Instance();
};


#endif //MYQTAOO_DATAMANAGER_H
