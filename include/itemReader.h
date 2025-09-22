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
signals:
    void getCountOfItemsIsFinished(QJsonDocument jsonDoc);              // сигнал: успех getCountOfItemsJson
    void readCatalogIsFinished(QJsonDocument jsonDoc);                  // сигнал: успех readItems
    void readPageOfItemIsFinished(QString html, QString nameOfItem);    // сигнал: успех readPageOfItem
    void sendResultOfSteamInventory(int chatId, QString steamId, QJsonDocument result); // сигнал: успех getSteamInventory
public slots:
    void readItems(int start);                                          // получение списка предметов ТП (стандартная структура QNetworkAccessManager)
    void readPageOfItem(QString nameOfItem);                            // получает endPoint со страницы предмета (стандартная структура QNetworkAccessManager)
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

struct receiverData{
    int m_threads = 0;
    QVector<QString> m_listOfUrls;
    QVector<QByteArray> m_listOfReceivedData;
    QMutex listMutex;
};

class setParametersReceiverCycle{
public:
    setParametersReceiverCycle(receiverData* cycleData) : m_cycleData(cycleData){};
    void setStep(int count);
    void setListOfUrls(QVector<QString> urlList);
private:
    receiverData* m_cycleData;
};

class executor : public QObject{
    Q_OBJECT
public:
    executor(QVector<QString> listOfUrls, receiverData* cycleData, int position) 
            : m_listOfUrls(listOfUrls), m_cycleData(cycleData), m_positionInGeneralVector(position){};
    void start();
signals: 
    void dataAreReceived();
    void lastThreadIsFinished();
private:
    void startNewIteration(QByteArray data);
    int m_currentPosition = 0;
    int m_positionInGeneralVector;
    receiverData* m_cycleData;
    QVector<QString> m_listOfUrls;
    dataRecipient* m_recipData;
};

class cycleStarter : public QObject{
    Q_OBJECT
public:
    cycleStarter(receiverData* cycleData) : m_cycleData(cycleData){};
    void start();
signals:
    void dataAreReceived(QVector<QByteArray> data);
public slots:
    void checkData();
private:
    bool inProgress;
    int m_countOfCompleted = 0;
    receiverData* m_cycleData;
    executor* m_exe;
};

class separationOfThreads{
public:
    static QVector<int> divide(int countOfItems, int countOfThreads);
};

class proxyStarter{
public:
    static void start(QNetworkAccessManager *manager);
};

#endif