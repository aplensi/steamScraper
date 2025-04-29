#include "../include/telegramBot.h"

void telegramBot::cycleOfGetUpdates(){
    m_parser = new parser();
    connect(this, &telegramBot::updateIsObtained, m_parser, &parser::parsBotUpdate);
    connect(m_parser, &parser::updateIdIsSet, this, &telegramBot::setUpdateIdToFile);
    connect(m_parser, &parser::emptyRequest, this, &telegramBot::getUpdates);
    connect(this, &telegramBot::idIsSet, this, &telegramBot::getUpdates);
}

void telegramBot::getUpdates(){
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.telegram.org/bot" + m_token + "/getUpdates?offset=" + m_updateId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        emit updateIsObtained(jsonDoc);
        reply->deleteLater();
        manager->deleteLater();
    });
    manager->get(request);
}

void telegramBot::setToken(QString token){
    m_token = token;
}

void telegramBot::setUpdateFile(QString fileName){
    QFile file(fileName);
    m_file = fileName;
    if(file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        QString id = in.readLine();
        file.close();
    }
}

void telegramBot::setUpdateIdToFile(int id){
    QFile file(m_file);
    m_updateId = QString::number(id+1);
    if(file.open(QIODevice::WriteOnly)){
        QTextStream out(&file);
        out << m_updateId;
        file.close();
    }
    emit idIsSet();
}