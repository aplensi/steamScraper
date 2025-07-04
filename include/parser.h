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

struct userItems{
    QString m_name;
    int m_count;
    float m_price;
    int m_countOfOffers;
};

struct userInventory{
    int m_steamId;
    QVector<userItems> m_listOfItems; 
    float m_commonPrice;
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
    void parsInventory(int chatId, QString steamId, QJsonDocument jsonDoc);

signals:
    void emptyRequest();                                        // tgBot
    void updateIdIsSet(int id);
    void sendIdAndSteamId(int chatId, QString steamId);
    void brockenDataOfInventory(int chatId, QString steamId);
    void nullCountOfItemsInventory(int chatId, QString steamId);
    void commandCommand(int chatId);
    void commandStart(int chatId);
    void commandSetId(int tgId, QString steamId);
    void commandGetPrice(int tgId);
    void commandShowInvetory(int tgId, QString steamId);
    void sendUserInventory(int tgId, userInventory usInv);
    void dontHaveItems(int tgId, QString steamId);

    void sendCountOfPages(int count);                           // itemReader
    void startNewPack(int start, int count);
    void countOfItemsIsNull();
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