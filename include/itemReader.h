#ifndef ITEMREADER_H
#define ITEMREADER_H

#include "includes.h"
#include "parser.h"

class itemReader : public QObject{
    Q_OBJECT
public:
    void getSteamInventory(int chatId, QString steamId);
    void getCountOfItemsJson();
    void startPackOfReadItems (int count);
    void cycleOfReadItems(int start, int count);
    void cycleOfReadPages(QVector<itemsOfPage> listOfItems);
    void cycleOfLoadingDataOfItem(QVector<itemsOfPage> listOfItems);
signals:
    void getCountOfItemsIsFinished(QJsonDocument jsonDoc);
    void readCatalogIsFinished(QJsonDocument jsonDoc);
    void readItemIsFinished();
    void readPageOfItemIsFinished(QString html, QString nameOfItem);
    void sendJsonOfData(QJsonDocument jsonDoc, int id);
    void sendResultOfSteamInventory(int chatId, QString steamId, QJsonDocument result);
public slots:
    void readItems(int start);
    void readPageOfItem(QString nameOfItem);
    void loadDataOfItem(int id);
private:
    void startProxy(QNetworkAccessManager *manager);
};


#endif