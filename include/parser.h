#ifndef PARSER_H
#define PARSER_H

#include "includes.h"

struct itemsOfPage{
    QString m_name;
    int m_id;
};

struct item{
    int m_id;
    float m_purchasePrice;
    int m_countOfPurchase;
    float m_salePrice;
    int m_countOfSale;
};

class parser: public QObject{
    Q_OBJECT
public slots:
    void getCountOfItemsFromJson(QJsonDocument jsonDoc);        // itemReader
    void readItemsFromJson(QJsonDocument jsonDoc);
    void parsPageOfItem(QString html, QString nameOfItem);
    QVector<itemsOfPage> getListOfItems();
    void setCountOfDBItems(int count);
    void setListOfItemsDB(QVector<itemsOfPage> listOfItems);

    void parsDataOfItem(QJsonDocument jsonDoc, int id);         // tgBot
    void parsBotUpdate(QJsonDocument jsonDoc);

signals:
    void emptyRequest();                                        // tgBot
    void updateIdIsSet(int id);
    void commandStart(int chatId);

    void sendCountOfPages(int count);                           // itemReader
    void brockenRequest(int start);
    void brockenPageOfItem(QString name);
    void namesIsFilled(QVector<itemsOfPage> listOfItems);
    void heapIsFinished(QVector<itemsOfPage> listOfItems);
    void namesAndIdsIsReceived(QVector<itemsOfPage> listOfItems);
    void dataOfItemIsReceived(QVector<itemsOfPage> listOfItems);
    void gettingDataIsOvered(QVector<item> listOfDataOfItem);
private:
    void parsPageOfMarketPlace(QString line);
    QString m_html;
    QFile m_file;
    int m_countOfItemsDB = 0;
    int m_countOfItems = 0;
    int m_countOfReadedItems = 0;
    int m_finishedThreads = 0;
    QDateTime m_dateTime;
    itemsOfPage m_items;
    item m_item;
    QVector<item> m_listOfDataOfItem;
    QVector<itemsOfPage> m_listOfItems;
    QVector<itemsOfPage> m_listOfItemsDB;
};

#endif