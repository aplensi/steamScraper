#ifndef TELEGRAMBOT_H
#define TELEGRAMBOT_H

#include "includes.h"
#include "controller.h"

class telegramBot : public QObject{
    Q_OBJECT
public:
    void cycleOfGetUpdates();
    void setToken(QString token);
    void getUpdates();
    void setUpdateFile(QString fileName);
    void connectToDb(QString userName, QString passWord, QString address, int port, QString nameDatabase);
signals:
    void idIsSet(); 
    void updateIsObtained(QJsonDocument jsonDoc);
private:
    void answerUserIsNotRegistered(int tgId);
    void answerDontHaveItems(int tgId);
    void answerNullCountOfItemInventory(int tgId, QString steamId);
    void answerBrockenId(int tgId, QString steamId);
    void answerStartCommand(int chatId);
    void answerCommandCommand(int chatId);
    void answerGetInventoryCommad(int chatId, userInventory inventory);
    void answerSetIdCommand(int tgId, QString steamId);
    void setUpdateIdToFile(int id);
    void setConnections();
    void deleteConnections();
    void sendMessage(int chatId, QString text);
    int m_countOfBrockenCheckOfInventory = 0;
    QString m_file;
    QString m_token;
    QString m_updateId = "-1";
    parser* m_parser;
    itemReader* m_reader;
    controller* m_controll;
};


#endif