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
signals:
    void getCountOfItemsIsFinished(QJsonDocument jsonDoc);
    void readCatalogIsFinished(QJsonDocument jsonDoc);
    void readItemIsFinished();
    void readPageOfItemIsFinished(QString html, QString nameOfItem);
public slots:
    void readItems(int start);
    void readPageOfItem(QString nameOfItem);
    void pageWithTooManyRequests(QString name);
private:
void startProxy();
};


#endif