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
    int i = 0;
    while(i < countOfItems){
        readItems(i);
        i += 100;
    }
}

void itemReader::readItems(int start)
{
    QUrl url("https://steamcommunity.com/market/search/render/?query=&start=" + QString::number(start) + "&count=100&search_descriptions=0&sort_column=name&sort_dir=asc&norender=1&appid=252490");
    m_request = new QNetworkRequest(url);
    m_networkManager = new QNetworkAccessManager();
    m_networkManager->get(*m_request);
    connect(m_networkManager, &QNetworkAccessManager::finished, [this, start](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if(jsonDoc.isNull()) {
            startProxy();
            readItems(start);
        }else{
            emit readCatalogIsFinished(jsonDoc);
        }
    });
}

void itemReader::cycleOfReadPages(QVector<itemsOfPage> listOfItems)
{
    m_networkManager->deleteLater();
    delete m_request;
    for(int i = 0; i < 100 && i < listOfItems.length(); i++){
        readPageOfItem(listOfItems[i].m_name);
    }
}

void itemReader::readPageOfItem(QString nameOfItem)
{
    QString encodedName = QUrl::toPercentEncoding(nameOfItem);
    QUrl url("https://steamcommunity.com/market/listings/252490/" + encodedName);
    m_request = new QNetworkRequest(url);
    m_networkManager = new QNetworkAccessManager(this);
    m_networkManager->get(*m_request);
    connect(m_networkManager, &QNetworkAccessManager::finished, [this, nameOfItem](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QString responseString = QString::fromUtf8(responseData);
        reply->deleteLater();
        if(responseString.isEmpty()) {
            startProxy();
            readPageOfItem(nameOfItem);
        }else{
            emit readPageOfItemIsFinished(responseString, nameOfItem);
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