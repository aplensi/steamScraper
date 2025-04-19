#include "../include/itemReader.h"

void itemReader::getCountOfItemsJson()
{
    QUrl url("https://steamcommunity.com/market/search/render/?query=&norender=1&appid=252490");
    QNetworkRequest request(url);
    m_networkManager = new QNetworkAccessManager();
    m_networkManager->get(request);
    connect(m_networkManager, &QNetworkAccessManager::finished, [this](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if(jsonDoc.isNull()) {
            qDebug() << "Error: Failed to parse JSON.";
            startProxy();
            getCountOfItemsJson();
        }else{
            emit getCountOfItemsIsFinished(jsonDoc);
        }
    });
}
void itemReader::cycleOfReadItems(int countOfItems)
{
    int i = -100;
    while(i < countOfItems + 100){
        i += 100;
        readItems(i, 100);
    }
}

void itemReader::readItems(int start, int count)
{
    QUrl url("https://steamcommunity.com/market/search/render/?query=&start=" + QString::number(start) + "&count=" + QString::number(count) + "&search_descriptions=0&sort_column=name&sort_dir=asc&norender=1&appid=252490");
    m_request = new QNetworkRequest(url);
    m_networkManager = new QNetworkAccessManager();
    m_networkManager->get(*m_request);
    connect(m_networkManager, &QNetworkAccessManager::finished, [this, start, count](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if(jsonDoc.isNull()) {
            //qDebug() << "Error: Failed to parse JSON. From: " << start;
            startProxy();
            readItems(start, count);
        }else{
            //qDebug() << "Data: " << responseData;
            emit readCatalogIsFinished(jsonDoc);
        }
    });
}

void itemReader::startProxy()
{
    QString proxyHost = "127.0.0.1";
    int proxyPort = 9050;

    QNetworkProxy networkProxy(QNetworkProxy::Socks5Proxy, proxyHost, proxyPort);
    QNetworkProxy::setApplicationProxy(networkProxy);
}