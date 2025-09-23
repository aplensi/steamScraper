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
        
    void parsBotUpdate(QJsonDocument jsonDoc);                  // tgBot
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

//================================================================================

class parsJson{
public:
    static QVector<QJsonObject> convertData(QVector<QByteArray> data);
};

class parsText{
public:
    QVector<QString> convertData(QVector<QByteArray> data);
};

class extractPageData{
public:
    QVector<itemsOfPage> extract(QJsonObject jsonDoc);
protected:
    int m_countItems;
    QJsonObject m_itemObj;
    QJsonArray m_itemsArray;
    itemsOfPage m_items;
    QVector<itemsOfPage> m_itemsVec;
};

class extractCountOfItemsFromPage : public extractPageData{
public:
    int getCountOfItems(QJsonObject jsonDoc);
};

class extractALotOfDataPages : public extractPageData{
public:
    static QVector<itemsOfPage> extract(QVector<QJsonObject> jsonDoc);
};

class extractItemData{
public:
    static QVector<item> extract(QVector<QJsonObject> jsonDoc);
};

class urlCreator{
public:
    static QString fromIdToMarketPriceUrl(int id); 
    static QString fromSteamIdToUserInventoryUrl(QString steamId);
    static QString fromNameOfItemToPageOfItemsUrl(QString nameOfItem);
    static QString fromNumberOfPageToPageOfTPUrl(int number);
};

#endif