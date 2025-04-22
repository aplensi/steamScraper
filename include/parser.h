#ifndef PARSER_H
#define PARSER_H

#include "includes.h"

struct itemsOfPage{
    QString m_name;
    int m_id;
};

struct item{
    QString m_name;
    int m_id;
    float m_purchasePrice;
    float m_countOfPurchase;
    float m_salePrice;
    float m_countOfSale;
};

class parser: public QObject{
    Q_OBJECT
public slots:
    void readFile(QString fileName);
    void readBuffer(QString html);
    void getCountOfPagesFromBuffer(QString html);
    void getCountOfItemsFromJson(QJsonDocument jsonDoc);
    void readItemsFromJson(QJsonDocument jsonDoc);
    void parsPageOfItem(QString html, QString nameOfItem);
    QVector<itemsOfPage> getListOfItems();
signals:
    void sendListOfItems(QVector<itemsOfPage> listOfItems);
    void sendCountOfPages(int count);
    void brockenRequest(int start);
    void brockenPageOfItem(QString name);
    void namesIsFilled(QVector<itemsOfPage> listOfItems);
    void heapIsFinished(QVector<itemsOfPage> listOfItems);
    void namesAndIdsIsReceived(QVector<itemsOfPage> listOfItems);
private:
    void parsPageOfMarketPlace(QString line);
    QString m_html;
    QFile m_file;
    int m_countOfItems = 0;
    int m_countOfReadedItems = 0;
    int m_finishedThreads = 0;
    QDateTime m_dateTime;
    itemsOfPage m_items;
    QVector<itemsOfPage> m_listOfItems;
};

#endif