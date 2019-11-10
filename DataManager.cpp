//
// Created by Raymond_Lx on 2019/11/10.
//

#include "DataManager.h"

#include <QFile>
#include <QDebug>

DataManager::DataManager():dirty(false){

}

DataManager *DataManager::Instance() {
    static DataManager ins;
    return &ins;
}

void DataManager::loadFromFile(const QString &path) {
    currentJsonFileUrl = QUrl::fromLocalFile(path);
    QFile theFile(currentJsonFileUrl.toLocalFile());
    if(!theFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open file: "<<path<<endl;
        return;
    }
    QString jsonText(theFile.readAll());
    theFile.close();

    Rix::Json::Parser p(jsonText.toStdString());

    try {
        Rix::Json::Object j(p.Parse());

        currentJsonObject = j;
    }catch (Rix::Json::ParserException e)
    {
        currentJsonFileUrl.clear();
        qDebug()<<"Parsing json text fail!"<<endl;
        qDebug()<<e.what()<<endl;
    }



    dirty = false;
}

void DataManager::saveToFile(const QString &path) {
    currentJsonFileUrl = QUrl::fromLocalFile(path);
    saveToFile();
}
void DataManager::saveToFile() {
    if(currentJsonFileUrl.isEmpty())
        return;
    QFile theFile(currentJsonFileUrl.toLocalFile());
    if(!theFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug()<<"Can't open file "<<currentJsonFileUrl.toLocalFile()<<endl;
        return;
    }

    QString jsonText = QString::fromStdString(currentJsonObject.ToStr(true));

    theFile.write(jsonText.toStdString().c_str());

    theFile.close();

    dirty = false;
}

Rix::Json::Object &DataManager::getCurrentJsonObject() {
    return currentJsonObject;
}