#ifndef ITEMREADER_H
#define ITEMREADER_H

#include "includes.h"
#include "../include/parser.h"

class itemReader : public QObject{
    Q_OBJECT
public:
    void getCountOfItemsJson();
    void cycleOfReadItems(int countOfItems);
    void readItem();
signals:
    void getCountOfItemsIsFinished(QJsonDocument jsonDoc);
    void readCatalogIsFinished(QJsonDocument jsonDoc);
    void readItemIsFinished();
private:
void readItems(int start, int countOfItems);
void startProxy();
parser* m_parser;
QNetworkAccessManager* m_networkManager;
QNetworkRequest* m_request;
QNetworkReply* m_reply;
};


#endif