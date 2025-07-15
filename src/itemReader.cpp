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

void itemReader::startPackOfReadItems(int count){
    cycleOfReadItems(0, 50);
}

void itemReader::cycleOfReadItems(int start, int count)
{
    int step = 10;
    int i = start;
    while(i < start + count * step){
        readItems(i);
        i += step;
    }
}

void itemReader::readItems(int start)
{
    QUrl url("https://steamcommunity.com/market/search/render/?query=&start=" + QString::number(start) + "&count=10&search_descriptions=0&sort_column=name&sort_dir=asc&norender=1&appid=252490");
    QNetworkRequest request(url);
    QNetworkAccessManager* networkManager = new QNetworkAccessManager();
    QTimer* timer = new QTimer(this);
    startProxy(networkManager);
    networkManager->get(request);
    connect(timer, &QTimer::timeout, [this, start, networkManager, timer]() {
        disconnect(networkManager, &QNetworkAccessManager::finished, nullptr, nullptr);
        networkManager->deleteLater();
        timer->stop();
        timer->deleteLater();
        disconnect(networkManager, nullptr, nullptr, nullptr);
        readItems(start);
    });
    connect(networkManager, &QNetworkAccessManager::finished, [this, networkManager, start, timer](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        disconnect(timer, nullptr, nullptr, nullptr);
        networkManager->deleteLater();
        reply->deleteLater();
        timer->deleteLater();
        reply = nullptr;

        if(jsonDoc.isNull()) {
            readItems(start);
        }else{
            emit readCatalogIsFinished(jsonDoc);
        }
    });
    timer->start(2000);

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
    timer->start(2000);
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

//==============================================================================================================================

void dataRecipient::setData(QString UrlAdderess){
    QUrl url(UrlAdderess);
    QNetworkRequest request(url);
    QNetworkAccessManager* networkManager = new QNetworkAccessManager();
    QTimer* timer = new QTimer(this);
    proxyStarter::start(networkManager);
    networkManager->get(request);
    connect(timer, &QTimer::timeout, [this, UrlAdderess, networkManager, timer]() {
        disconnect(networkManager, &QNetworkAccessManager::finished, nullptr, nullptr);
        networkManager->deleteLater();
        timer->stop();
        timer->deleteLater();
        disconnect(networkManager, nullptr, nullptr, nullptr);
        setData(UrlAdderess);
    });
    connect(networkManager, &QNetworkAccessManager::finished, [this, networkManager, UrlAdderess, timer](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        disconnect(timer, nullptr, nullptr, nullptr);
        networkManager->deleteLater();
        reply->deleteLater();
        timer->deleteLater();
        reply = nullptr;

        if(responseData == "") { 
            setData(UrlAdderess);
        }else{
            emit dataAreReceived(responseData);
        }
    });
    timer->start(2000);
}

void textData::shapeData(QByteArray responseData){
    emit dataAreShaped(QString::fromUtf8(responseData));
}

void textData::shapeData(QVector<QByteArray> responseList){
    QVector<QString> newData;
    for(const auto &i : responseList){
        newData.append(QString::fromUtf8(i));
    }
    emit dataAreShaped(newData);
}

void jsonData::shapeData(QByteArray responseData){
    emit dataAreShaped(QJsonDocument::fromJson(responseData));
}

void jsonData::shapeData(QVector<QByteArray> responseList){
    QVector<QJsonDocument> newData;
    for(const auto &i : responseList){
        newData.append(QJsonDocument::fromJson(i));
    }
    emit dataAreShaped(newData);
}

void setParametersReceiverCycle::setStep(int step){
    if(step <= 1){
        std::cout << "\nThe step is specified incorrectly!" << std::endl;
        return;
    }else{
        m_cycleData->m_threads = step;
    }
}

void setParametersReceiverCycle::setListOfUrls(QVector<QString> listOfUrls){
    if(listOfUrls.isEmpty()){
        std::cout << "\nList is empty!" << std::endl;
        return;
    }else{
        m_cycleData->m_listOfUrls = listOfUrls;
    }
}

cycleStarter::~cycleStarter(){
    delete m_cycleData;
    delete m_exe;
}

void cycleStarter::checkData(){
    if(m_cycleData->m_listOfReceivedData.length() == m_cycleData->m_listOfUrls.length()){
        emit dataAreReceived();
    }
}

void cycleStarter::start(){
    connect(m_exe, &executor::dataAreReceived, this, &cycleStarter::checkData);
    if(m_cycleData->m_listOfUrls.isEmpty() && m_cycleData->m_threads <= 1){
        std::cout << "\nData not specified!" << std::endl;
    }else{
        int step = (m_cycleData->m_listOfUrls.length() - 1 + m_cycleData->m_threads) / m_cycleData->m_threads;
        QVector<QString> listOfUrls;
        for(int i = 0; i < m_cycleData->m_listOfUrls.length(); i++){
            if(i + 1 % step == 0){
                m_exe = new executor(listOfUrls, m_cycleData);
                listOfUrls.clear();
                m_exe->start();
            }else{
                listOfUrls.append(m_cycleData->m_listOfUrls[i]);
            }
        }
    }
}

void executor::start(){
    connect(m_recipData, &dataRecipient::dataAreReceived, this, &executor::startNewIteration);
    m_recipData->setData(m_listOfUrls[m_currentPosition]);
}

void executor::startNewIteration(QByteArray data){
    m_cycleData->m_listOfReceivedData.append(data);
    m_currentPosition++;
    if(m_currentPosition < m_listOfUrls.length()){
        m_recipData->setData(m_listOfUrls[m_currentPosition]);
    }else{
        emit dataAreReceived();
    }
}

void proxyStarter::start(QNetworkAccessManager *manager){
    QNetworkProxy proxy(QNetworkProxy::Socks5Proxy, "127.0.0.1", 9050);
    manager->setProxy(proxy);
}