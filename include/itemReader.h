#ifndef ITEMREADER_H
#define ITEMREADER_H

#include "includes.h"
#include "parser.h"

class itemReader : public QObject{
    Q_OBJECT
public:
    void getSteamInventory(int chatId, QString steamId);                // получает данные инвентаря (стандартная структура QNetworkAccessManager)
    void getCountOfItemsJson();                                         // получает данные ТП (стандартная структура QNetworkAccessManager)
    void startPackOfReadItems (int count);                              // метод который запускает цикл, который запускает readItems
    void cycleOfReadItems(int start, int count);                        // цикл который запускается startPackOfReadItems для того чтобы запустить readItems
    void cycleOfReadPages(QVector<itemsOfPage> listOfItems);            // цикл который запускает readItems
    void cycleOfLoadingDataOfItem(QVector<itemsOfPage> listOfItems);    // цикл который запускает loadDataOfItem
signals:
    void getCountOfItemsIsFinished(QJsonDocument jsonDoc);              // сигнал: успех getCountOfItemsJson
    void readCatalogIsFinished(QJsonDocument jsonDoc);                  // сигнал: успех readItems
    void readPageOfItemIsFinished(QString html, QString nameOfItem);    // сигнал: успех readPageOfItem
    void sendJsonOfData(QJsonDocument jsonDoc, int id);                 // сигнал: успех loadDataOfItem 
    void sendResultOfSteamInventory(int chatId, QString steamId, QJsonDocument result); // сигнал: успех getSteamInventory
public slots:
    void readItems(int start);                                          // получение списка предметов ТП (стандартная структура QNetworkAccessManager)
    void readPageOfItem(QString nameOfItem);                            // получает endPoint со страницы предмета (стандартная структура QNetworkAccessManager)
    void loadDataOfItem(int id);                                        // получает торговые данные предмета (стандартная структура QNetworkAccessManager)
private:
    void startProxy(QNetworkAccessManager *manager);                    // соединяет QNetworkAccessManager с прокси
};

class dataRecipient : public QObject{
    Q_OBJECT
public:
    void setData(QString url);
    QByteArray getData();
private:
    QByteArray m_responseData;
};

class textData : public QObject{
    Q_OBJECT
public:
    virtual void shapeData(QByteArray responseData) = 0;
};

class jsonData : textData{
public:
    void shapeData(QByteArray responseData) override;
};

class cycleStarter{
public:
    void setUrl(QString url);
    void setStep(int count);
    void startCycle();
private:
    int m_step = 0;
    QString m_url = "";
};

class proxyStarter{
public:
    proxyStarter(QNetworkAccessManager *manager);
};

#endif