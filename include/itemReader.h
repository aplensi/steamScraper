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
signals:
    void dataAreReceived(QByteArray data);
private:
    QByteArray m_responseData;
};

class textData : public QObject{
    Q_OBJECT
public:
    virtual void shapeData(QByteArray responseData);
    virtual void shapeData(QVector<QByteArray> responseData);
signals:
    void dataAreShaped(auto data);
};

class jsonData : textData{
public:
    void shapeData(QByteArray responseData) override;
    virtual void shapeData(QVector<QByteArray> responseData) override;
};

struct receiverData{
    int m_threads = 0;
    QVector<QString> m_listOfUrls;
    QVector<QByteArray> m_listOfReceivedData;
};

class setParametersReceiverCycle{
public:
    setParametersReceiverCycle(receiverData* cycleData) : m_cycleData(cycleData){};
    void setStep(int count);
    void setListOfUrls(QVector<QString> urlList);
private:
    receiverData* m_cycleData;
};

class cycleStarter{
public:
    cycleStarter(receiverData* cycleData) : m_cycleData(cycleData){};
    void start();
signals:
    void dataAreReceived();
private:
    receiverData* m_cycleData;
};

class executor : public QObject{
    Q_OBJECT
public:
    executor(QVector<QString> listOfUrls, receiverData* cycleData) : m_listOfUrls(listOfUrls), m_cycleData(cycleData){};
    void start();
    void start(int position);
signals: 
    void dataAreReceived();
private:
    void startNewIteration(QByteArray data);
    int m_currentPosition = 0;
    receiverData* m_cycleData;
    QVector<QString> m_listOfUrls;
    dataRecipient* m_recipData;
};

class proxyStarter{
public:
    static void start(QNetworkAccessManager *manager);
};

#endif