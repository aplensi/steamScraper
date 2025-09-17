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

void dataRecipient::setData(QString UrlAddress){
    QUrl url(UrlAddress);
    QNetworkRequest request(url);
    QNetworkAccessManager* networkManager = new QNetworkAccessManager();
    QTimer* timer = new QTimer();
    proxyStarter::start(networkManager);

    networkManager->get(request); 
    connect(timer, &QTimer::timeout, [this, UrlAddress, networkManager, timer]() {
        disconnect(networkManager, &QNetworkAccessManager::finished, nullptr, nullptr);
        networkManager->deleteLater();
        timer->stop();
        timer->deleteLater();
        setData(UrlAddress);
    });
    connect(networkManager, &QNetworkAccessManager::finished, [this, networkManager, UrlAddress, timer](QNetworkReply* reply) {
        QByteArray responseData = reply->readAll();
        disconnect(timer, &QTimer::timeout, nullptr, nullptr);
        networkManager->deleteLater();
        reply->deleteLater();
        timer->deleteLater();
        reply = nullptr;

        if(responseData == "") { 
            setData(UrlAddress);
        }else{
            emit dataAreReceived(responseData);
        }
    });
    timer->start(2000);
}

void setParametersReceiverCycle::setStep(int step){
    if(step <= 1){
        qDebug() << "The step is specified incorrectly!";
        return;
    }else{
        m_cycleData->m_threads = step;
    }
}

void setParametersReceiverCycle::setListOfUrls(QVector<QString> listOfUrls){
    if(listOfUrls.isEmpty()){
        qDebug() << "List is empty!";
        return;
    }else{
        m_cycleData->m_listOfUrls = listOfUrls;
    }
}

void cycleStarter::checkData(){
    qDebug() << "data reception completed";
    if(m_cycleData->m_listOfReceivedData.length() == m_cycleData->m_listOfUrls.length()){
        emit dataAreReceived(m_cycleData->m_listOfReceivedData);
    }
}

void cycleStarter::start(){
    if(!inProgress){
        inProgress = true;
        if(m_cycleData->m_listOfUrls.isEmpty() || m_cycleData->m_threads < 1){
            qDebug() << "Data not specified!";
            qDebug() << "Length of url list: " << m_cycleData->m_listOfUrls.length();
            qDebug() << "Count of threads: " << m_cycleData->m_threads;
        }else{
            int step = (m_cycleData->m_listOfUrls.length() - 1 + m_cycleData->m_threads) / m_cycleData->m_threads;
            QVector<QString> listOfUrls;
            for(int i = 0; i < m_cycleData->m_listOfUrls.length(); i++){
                if((i + 1) % step == 0 || i == m_cycleData->m_listOfUrls.length() - 1){
                    listOfUrls.append(m_cycleData->m_listOfUrls[i]);
                    m_exe = new executor(listOfUrls, m_cycleData);
                    QThread *th = new QThread;
                    m_exe->moveToThread(th);
                    connect(m_exe, &executor::lastThreadIsFinished, this, &cycleStarter::checkData);
                    connect(m_exe, &executor::dataAreReceived, th, &QThread::quit);
                    connect(this, &cycleStarter::dataAreReceived, m_exe, &executor::deleteLater);
                    connect(this, &cycleStarter::dataAreReceived, th, &QThread::deleteLater);
                    connect(th, &QThread::started, m_exe, &executor::start);
                    listOfUrls.clear();
                    th->start();
                }else{
                    listOfUrls.append(m_cycleData->m_listOfUrls[i]);
                }
            }
        }
    }else{
        qDebug() << "Program in progress";
    }
}

void executor::start(){
    m_recipData = new dataRecipient();
    connect(m_recipData, &dataRecipient::dataAreReceived, this, &executor::startNewIteration);
    m_recipData->setData(m_listOfUrls[m_currentPosition]);
}

void executor::startNewIteration(QByteArray data){
    QMutexLocker locker(&m_cycleData->listMutex);
    m_cycleData->m_listOfReceivedData.append(data);
    m_currentPosition++;
    if(m_currentPosition < m_listOfUrls.length()){
        m_recipData->setData(m_listOfUrls[m_currentPosition]);
    }else{
        if(m_cycleData->m_listOfUrls.length() == m_cycleData->m_listOfReceivedData.length()){
            emit lastThreadIsFinished();
        }
        emit dataAreReceived();
        this->deleteLater();
    }
}

void proxyStarter::start(QNetworkAccessManager *manager){
    QNetworkProxy proxy(QNetworkProxy::Socks5Proxy, "127.0.0.1", 9050);
    manager->setProxy(proxy);
}