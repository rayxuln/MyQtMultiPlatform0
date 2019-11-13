//
// Created by Raymond_Lx on 2019/11/10.
//

#include "DataManager.h"

#include <QFile>
#include <QDebug>

DataManager::DataManager():dirty(false){
    currentJsonFileUrl = QUrl::fromLocalFile("Untitled.json");
}

DataManager *DataManager::Instance() {
    static DataManager ins;
    return &ins;
}

void DataManager::setDirty(bool _d) {
    dirty = _d;
}
bool DataManager::isDirty() {
    return dirty;
}

bool DataManager::currentFileExists() {
    return QFile::exists(currentJsonFileUrl.toLocalFile());
}

QString DataManager::getFileName() {
    return currentJsonFileUrl.fileName();
}

bool DataManager::loadFromFile(const QString &path) {
    if(!QFile::exists(path)){
        qDebug()<<"File not found: "<<path<<endl;
        return false;
    }
    currentJsonFileUrl = QUrl::fromLocalFile(path);
    QFile theFile(currentJsonFileUrl.toLocalFile());
    if(!theFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open file: "<<path<<endl;
        return false;
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
        throw e;
        return false;
    }



    dirty = false;
    return true;
}

bool DataManager::saveToFile(const QString &path) {
    currentJsonFileUrl = QUrl::fromLocalFile(path);
    return saveToFile();
}
bool DataManager::saveToFile() {
    if(currentJsonFileUrl.isEmpty())
        return false;
    QFile theFile(currentJsonFileUrl.toLocalFile());
    if(!theFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug()<<"Can't open file "<<currentJsonFileUrl.toLocalFile()<<endl;
        return false;
    }

    QString jsonText = QString::fromStdString(currentJsonObject.ToStr(true));

    theFile.write(jsonText.toStdString().c_str());

    theFile.close();

    dirty = false;
    return true;
}

Rix::Json::Object &DataManager::getCurrentJsonObject() {
    return currentJsonObject;
}