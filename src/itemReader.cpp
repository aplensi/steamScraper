#include "../include/itemReader.h"

void itemReader::getCountOfItemsJson()
{
    QUrl url("https://steamcommunity.com/market/search/render/?query=&start=0&count=0&search_descriptions=0&sort_column=name&sort_dir=asc&norender=1&appid=252490");
    QNetworkRequest request(url);
    QNetworkAccessManager* networkManager = new QNetworkAccessManager();
    startProxy(networkManager);
    networkManager->get(request);
    connect(networkManager, &QNetworkAccessManager::finished, [this, networkManager](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        networkManager->deleteLater();
        reply->deleteLater();
        reply = nullptr;

        if(jsonDoc.isNull()) {
            getCountOfItemsJson();
        }else{
            emit getCountOfItemsIsFinished(jsonDoc);
        }
    });
}

void itemReader::cycleOfReadItems(int countOfItems)
{
    int i = 0;
    while(i < countOfItems){
        readItems(i);
        i += 100;
    }
}

void itemReader::readItems(int start)
{
    QUrl url("https://steamcommunity.com/market/search/render/?query=&start=" + QString::number(start) + "&count=100&search_descriptions=0&sort_column=name&sort_dir=asc&norender=1&appid=252490");
    QNetworkRequest request(url);
    QNetworkAccessManager* networkManager = new QNetworkAccessManager();
    startProxy(networkManager);
    networkManager->get(request);
    connect(networkManager, &QNetworkAccessManager::finished, [this, start, networkManager](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        networkManager->deleteLater();
        reply->deleteLater();
        reply = nullptr;
        if(jsonDoc.isNull()) {
            readItems(start);
        }else{
            emit readCatalogIsFinished(jsonDoc);
        }
    });
}

void itemReader::cycleOfReadPages(QVector<itemsOfPage> listOfItems)
{
    for(int i = 0; i < 100 && i < listOfItems.length(); i++){
        readPageOfItem(listOfItems[i].m_name);
    }
}

void itemReader::readPageOfItem(QString nameOfItem)
{
    QString encodedName = QUrl::toPercentEncoding(nameOfItem);
    QUrl url("https://steamcommunity.com/market/listings/252490/" + encodedName);
    QNetworkRequest request(url);
    QNetworkAccessManager* networkManager = new QNetworkAccessManager();
    startProxy(networkManager);
    networkManager->get(request);
    connect(networkManager, &QNetworkAccessManager::finished, [this, nameOfItem, networkManager, request](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QString responseString = QString::fromUtf8(responseData);
        networkManager->deleteLater();
        reply->deleteLater();
        reply = nullptr;
        if(responseString.isEmpty()) {
            readPageOfItem(nameOfItem);
        }else{
            emit readPageOfItemIsFinished(responseString, nameOfItem);
        }
    });
}

void itemReader::cycleOfLoadingDataOfItem(QVector<itemsOfPage> listOfItems)
{
    for(int i = 0; i < 200 && i < listOfItems.length(); i++){
        loadDataOfItem(listOfItems[i].m_id);
    }
}

void itemReader::loadDataOfItem(int id)
{
    QUrl url("https://steamcommunity.com/market/itemordershistogram?country=EU&language=english&currency=1&item_nameid=" + QString::number(id) + "&norender=1");
    QNetworkRequest request(url);
    QNetworkAccessManager* networkManager = new QNetworkAccessManager();
    QTimer* timer = new QTimer(this);
    startProxy(networkManager);
    networkManager->get(request);
    connect(timer, &QTimer::timeout, [this, id, networkManager, timer]() {
        disconnect(networkManager, &QNetworkAccessManager::finished, nullptr, nullptr);
        networkManager->deleteLater();
        timer->stop();
        timer->deleteLater();
        disconnect(networkManager, nullptr, nullptr, nullptr);
        loadDataOfItem(id);
    });
    connect(networkManager, &QNetworkAccessManager::finished, [this, networkManager, id, timer](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        disconnect(timer, nullptr, nullptr, nullptr);
        networkManager->deleteLater();
        reply->deleteLater();
        timer->deleteLater();
        reply = nullptr;

        if(responseData == "" || jsonDoc.isNull()) { 
            loadDataOfItem(id);
        }else{
            emit sendJsonOfData(jsonDoc, id);
        }
    });
    timer->start(10000);
}

void itemReader::getSteamInventory(int chatId, QString steamId){
    QUrl url("https://steamcommunity.com/inventory/" + steamId + "/252490/2?l=english&norender=1");
    QNetworkRequest request(url);
    QNetworkAccessManager* networkManager = new QNetworkAccessManager();
    startProxy(networkManager);
    networkManager->get(request);
    connect(networkManager, &QNetworkAccessManager::finished, [this, networkManager, chatId, steamId](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        networkManager->deleteLater();
        reply->deleteLater();
        reply = nullptr;
        emit sendResultOfSteamInventory(chatId, steamId, jsonDoc);
    });
}

void itemReader::startProxy(QNetworkAccessManager *manager) 
{
    QNetworkProxy proxy(QNetworkProxy::Socks5Proxy, "127.0.0.1", 9050);
    manager->setProxy(proxy);
}