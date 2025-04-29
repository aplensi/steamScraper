#ifndef ITEMREADER_H
#define ITEMREADER_H

#include "includes.h"
#include "../include/parser.h"

class itemReader : public QObject{
    Q_OBJECT
public:
    void getCountOfItemsJson();
    void cycleOfReadItems(int countOfItems);
    void cycleOfReadPages(QVector<itemsOfPage> listOfItems);
    void cycleOfLoadingDataOfItem(QVector<itemsOfPage> listOfItems);
signals:
    void getCountOfItemsIsFinished(QJsonDocument jsonDoc);
    void readCatalogIsFinished(QJsonDocument jsonDoc);
    void readItemIsFinished();
    void readPageOfItemIsFinished(QString html, QString nameOfItem);
    void sendJsonOfData(QJsonDocument jsonDoc, int id);
public slots:
    void readItems(int start);
    void readPageOfItem(QString nameOfItem);
    void loadDataOfItem(int id);
private:
    void startProxy(QNetworkAccessManager *manager);
};


#endif