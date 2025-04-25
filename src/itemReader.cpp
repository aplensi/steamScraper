#include "../include/itemReader.h"

void itemReader::getCountOfItemsJson()
{
    QUrl url("https://steamcommunity.com/market/search/render/?query=&norender=1&appid=252490");
    QNetworkRequest request(url);
    QNetworkAccessManager* m_networkManager = new QNetworkAccessManager();
    m_networkManager->get(request);
    connect(m_networkManager, &QNetworkAccessManager::finished, [this, m_networkManager](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if(jsonDoc.isNull()) {
            qDebug() << "Error: Failed to parse JSON.";
            startProxy();
            getCountOfItemsJson();
        }else{
            emit getCountOfItemsIsFinished(jsonDoc);
        }
        m_networkManager->deleteLater();
        reply->deleteLater();
        reply = nullptr;
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
    networkManager->get(request);
    connect(networkManager, &QNetworkAccessManager::finished, [this, start, networkManager](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if(jsonDoc.isNull()) {
            startProxy();
            readItems(start);
        }else{
            emit readCatalogIsFinished(jsonDoc);
        }
        networkManager->deleteLater();
        reply->deleteLater();
        reply = nullptr;
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
    networkManager->get(request);
    connect(networkManager, &QNetworkAccessManager::finished, [this, nameOfItem, networkManager, request](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QString responseString = QString::fromUtf8(responseData);
        if(responseString.isEmpty()) {
            startProxy();
            readPageOfItem(nameOfItem);
        }else{
            emit readPageOfItemIsFinished(responseString, nameOfItem);
        }
        networkManager->deleteLater();
    });
}

void itemReader::cycleOfLoadingDataOfItem(QVector<itemsOfPage> listOfItems)
{
    for(int i = 0; i < 250 && i < listOfItems.length(); i++){
        loadDataOfItem(listOfItems[i].m_id);
    }
}

void itemReader::loadDataOfItem(int id)
{
    QUrl url("https://steamcommunity.com/market/itemordershistogram?country=EU&language=english&currency=1&item_nameid=" + QString::number(id) + "&norender=1");
    QNetworkRequest request(url);
    QNetworkAccessManager* networkManager = new QNetworkAccessManager();
    networkManager->get(request);
    connect(networkManager, &QNetworkAccessManager::finished, [this, networkManager, id](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if(responseData == "" || jsonDoc.isNull()) { 
            startProxy();
            loadDataOfItem(id);
        }else{
            emit sendJsonOfData(jsonDoc, id);
            networkManager->deleteLater();
            reply->deleteLater();
            reply = nullptr;
        }
    });
}

void itemReader::pageWithTooManyRequests(QString name)
{
    startProxy();
    readPageOfItem(name);
}

void itemReader::startProxy() 
{
    QString proxyHost = "127.0.0.1";
    int proxyPort = 9050;

    QNetworkProxy networkProxy(QNetworkProxy::Socks5Proxy, proxyHost, proxyPort);
    QNetworkProxy::setApplicationProxy(networkProxy);
}