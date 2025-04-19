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
    QVector<itemsOfPage> getListOfItems();
signals:
    void sendListOfItems(QVector<itemsOfPage> listOfItems);
    void sendCountOfPages(int count);
private:
    void parsPageOfMarketPlace(QString line);
    QString m_html;
    QFile m_file;
    QDateTime m_dateTime;
    itemsOfPage m_items;
    QVector<itemsOfPage> m_listOfItems;
};

#endif