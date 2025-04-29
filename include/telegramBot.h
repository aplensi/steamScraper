#ifndef TELEGRAMBOT_H
#define TELEGRAMBOT_H

#include "includes.h"
#include "parser.h"

class telegramBot : public QObject{
    Q_OBJECT
public:
    void cycleOfGetUpdates();
    void setToken(QString token);
    void getUpdates();
    void setUpdateFile(QString fileName);
signals:
    void idIsSet(); 
    void updateIsObtained(QJsonDocument jsonDoc);
private:
    void setUpdateIdToFile(int id);
    QString m_file;
    QString m_token;
    QString m_updateId = "-1";
    parser* m_parser;
};


#endif