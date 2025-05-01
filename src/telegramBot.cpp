#include "../include/telegramBot.h"

void telegramBot::cycleOfGetUpdates(){
    m_parser = new parser();
    m_reader = new itemReader();
    m_controll = new controller();
    connect(this, &telegramBot::updateIsObtained, m_parser, &parser::parsBotUpdate);

    connect(m_parser, &parser::updateIdIsSet, this, &telegramBot::setUpdateIdToFile);
    connect(m_parser, &parser::emptyRequest, this, &telegramBot::getUpdates);

    connect(m_parser, &parser::commandStart, this, &telegramBot::answerStartCommand);
    connect(m_parser, &parser::commandCommand, this, &telegramBot::answerCommandCommand);

    connect(m_parser, &parser::commandSetId, m_reader, &itemReader::getSteamInventory);
    connect(m_reader, &itemReader::sendResultOfSteamInventory, m_parser, &parser::parsAndCheckSteamId);
    connect(m_parser, &parser::sendIdAndSteamId, m_controll, &controller::pushUserToDB);
    connect(m_parser, &parser::brockenDataOfInventory, this, &telegramBot::answerBrockenId);
    connect(m_controll, &controller::userAdded, this, &telegramBot::answerSetIdCommand);

    connect(this, &telegramBot::idIsSet, this, &telegramBot::getUpdates);
}

void telegramBot::connectToDb(QString userName, QString passWord, QString address, int port, QString nameDatabase){
    m_controll->connectToPgSQL(userName, passWord, address, port, nameDatabase);
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

void telegramBot::answerBrockenId(int tgId, QString steamId){
    sendMessage(tgId, "🤡 Вы ввели не верный id.\n\nВозможные проблемы:\n- Отсутствие скинов\n- Закрытый инвентарь\n- Неверный id\n\nВ случае, "
                    "если вы уверены в правильности id, пожалуйста попробуйте снова через несколько секунд.");
}

void telegramBot::answerGetInventoryCommad(int chatId, userInventory inventory){
    QString message = "Инвентарь: \n";
    for(auto i : inventory.m_listOfItems){
        message += "Название: " + i.m_name + " | Цена: " + QString::number(i.m_price) +
        " | Количество: " + QString::number(i.m_count) + " | Количество предложений на ТП: " + QString::number(i.m_countOfOffers) + "\n";
    }
    sendMessage(chatId, message);
}

void telegramBot::answerStartCommand(int chatId){
    sendMessage(chatId, "Пока бот нихуя не может, но в течении дня сможет проверить текущую стоимость инвентаря и привязать свой профиль к аккаунту tg \n\ngitHub проект: \nhttps://github.com/aplensi/steamScraper");
}
void telegramBot::answerCommandCommand(int chatId){
    sendMessage(chatId, "Список команд: \n\n"
                        "- /setId:id - связать профиль tg с профилем steam.\n"
                        "- /getPrice - получить стоимость инвентаря (только с привязанным профилем steam)\n"
                        "- /showInventory:id - цена любого открытого инвентаря\n\n"
                        "‼️ \"id\" заменять на steamID64 (его можно получить тут https://steamid.io )");
}

void telegramBot::sendMessage(int chatId, QString text){
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.telegram.org/bot" + m_token + "/sendMessage?chat_id=" + QString::number(chatId)+"&text=" + text));
    manager->get(request);
}

void telegramBot::setToken(QString token){
    m_token = token;
}

void telegramBot::answerSetIdCommand(int tgId, QString steamId){
    sendMessage(tgId, "Id " + steamId + " успешно связан с вашим аккаунтом");
}

void telegramBot::setUpdateFile(QString fileName){
    QFile file(fileName);
    m_file = fileName;
    if(file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        m_updateId = in.readLine();
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