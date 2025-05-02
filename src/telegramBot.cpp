#include "../include/telegramBot.h"

void telegramBot::cycleOfGetUpdates(){
    m_parser = new parser();
    m_reader = new itemReader();
    m_controll = new controller();
    setConnections();
}

void telegramBot::setConnections(){
    connect(this, &telegramBot::updateIsObtained, m_parser, &parser::parsBotUpdate);

    connect(m_parser, &parser::updateIdIsSet, this, &telegramBot::setUpdateIdToFile);
    connect(m_parser, &parser::emptyRequest, this, &telegramBot::getUpdates);

    connect(m_parser, &parser::commandStart, this, &telegramBot::answerStartCommand);
    connect(m_parser, &parser::commandCommand, this, &telegramBot::answerCommandCommand);

    connect(m_parser, &parser::commandSetId, [this](int chatId, QString steamId){
        connect(m_reader, &itemReader::sendResultOfSteamInventory, m_parser, &parser::parsAndCheckSteamId);
        connect(m_parser, &parser::sendIdAndSteamId, m_controll, &controller::pushUserToDB);
        connect(m_parser, &parser::nullCountOfItemsInventory, this, &telegramBot::answerNullCountOfItemInventory);
        connect(m_parser, &parser::brockenDataOfInventory, this, &telegramBot::answerBrockenId);
        connect(m_controll, &controller::userAdded, this, &telegramBot::answerSetIdCommand);
        m_reader->getSteamInventory(chatId, steamId);
    });

    connect(m_parser, &parser::commandGetPrice, [this](int tgId){
        connect(m_controll, &controller::setSteamIdOfUser, m_reader, &itemReader::getSteamInventory);
        connect(m_reader, &itemReader::sendResultOfSteamInventory, m_parser, &parser::parsInventory);
        connect(m_parser, &parser::brockenDataOfInventory, m_reader, &itemReader::getSteamInventory);
        connect(m_parser, &parser::dontHaveItems, this, &telegramBot::answerDontHaveItems);
        connect(m_parser, &parser::sendUserInventory, m_controll, &controller::fillUserInventory);
        connect(m_controll, &controller::invOfUserIsFilled, this, &telegramBot::answerGetInventoryCommad);
        m_controll->getSteamIdOfUser(tgId);
    });

    connect(m_parser, &parser::commandShowInvetory, [this](int tgId, QString steamId){
        connect(m_reader, &itemReader::sendResultOfSteamInventory, m_parser, &parser::parsInventory);
        connect(m_parser, &parser::nullCountOfItemsInventory, this, &telegramBot::answerNullCountOfItemInventory);
        connect(m_parser, &parser::brockenDataOfInventory, this, &telegramBot::answerBrockenId);
        connect(m_parser, &parser::dontHaveItems, this, &telegramBot::answerDontHaveItems);
        connect(m_parser, &parser::sendUserInventory, m_controll, &controller::fillUserInventory);
        connect(m_controll, &controller::invOfUserIsFilled, this, &telegramBot::answerGetInventoryCommad);
        m_reader->getSteamInventory(tgId, steamId);
    });

    connect(this, &telegramBot::idIsSet, this, &telegramBot::getUpdates);
}

void telegramBot::deleteConnections(){
    disconnect(this, nullptr, nullptr, nullptr);
    disconnect(m_parser, nullptr, nullptr, nullptr);
    disconnect(m_reader, nullptr, nullptr, nullptr);
    disconnect(m_controll, nullptr, nullptr, nullptr);
}

void telegramBot::connectToDb(QString userName, QString passWord, QString address, int port, QString nameDatabase){
    m_controll->connectToPgSQL(userName, passWord, address, port, nameDatabase);
}

void telegramBot::getUpdates(){
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.telegram.org/bot" + m_token + "/getUpdates?offset=" + m_updateId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this, manager, timer]() {
        disconnect(manager, &QNetworkAccessManager::finished, nullptr, nullptr);
        manager->deleteLater();
        timer->stop();
        timer->deleteLater();
        disconnect(manager, nullptr, nullptr, nullptr);
        getUpdates();
    });
    connect(manager, &QNetworkAccessManager::finished, [this, timer, manager](QNetworkReply *reply) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        disconnect(timer, nullptr, nullptr, nullptr);
        reply->deleteLater();
        manager->deleteLater();
        timer->deleteLater();
        emit updateIsObtained(jsonDoc);
    });
    manager->get(request);
    timer->start(4000);
}

void telegramBot::answerBrockenId(int tgId, QString steamId){
    if(m_countOfBrockenCheckOfInventory < 10){
        qDebug() << "count brocken tests: " << m_countOfBrockenCheckOfInventory;
        QTimer::singleShot(500, [this, tgId, steamId](){
            m_reader->getSteamInventory(tgId, steamId);
            m_countOfBrockenCheckOfInventory++;
        });
    }else{
        sendMessage(tgId, "🤡 Вы ввели не верный id.\n\nВозможные причины:\n- Закрытый инвентарь\n- Неверный id\n\nВ случае, "
                            "если вы уверены в правильности id, пожалуйста попробуйте снова через несколько секунд.\n\n"
                            "(В случае если вы повторно ввели id и все заработало - сообщите нам и Ашир из Нью-Дели будет наказан)");
        m_countOfBrockenCheckOfInventory = 0;                    
    }
    
}
void telegramBot::answerNullCountOfItemInventory(int tgId, QString steamId){
     sendMessage(tgId, "🤡 Инвентарь с нулевым количеством скинов.\n\nВозможные причины:\n- Отсутствие скинов в инвентаре\n- Отсутствие игры на аккаунте\n"
                        "Пожалуйста введите другой id.");
}

void telegramBot::answerDontHaveItems(int chatId){
    sendMessage(chatId, "🤡 Что ты решил проверить? \nУ тебя нет скинов на продажу!");
}

void telegramBot::answerGetInventoryCommad(int chatId, userInventory inventory){
    QString message = "🎒 Инвентарь: \n\n";
    float commonPrice = 0;
    for(auto i : inventory.m_listOfItems){
        message += "🔥 Название: " + i.m_name + " | Цена: " + QString::number(i.m_price) +
        " | Количество: " + QString::number(i.m_count) + " | Количество предложений на ТП: " + QString::number(i.m_countOfOffers) + "\n";
        commonPrice += i.m_price * i.m_count;
    }
    message += "\n💰 Общая стоимость: " + QString::number(commonPrice) + " баксов нахуй";
    sendMessage(chatId, message);
}

void telegramBot::answerStartCommand(int chatId){
    sendMessage(chatId, "Бот с помощью которого ты сможешь проверить цену инвентаря и связать профиль тг со стимом, чтобы постоянно не вводить id.\n"
                        "Данные о ценах обновляются каждые 1-3 минуты "
                        "(зависит от того, какая скорость будет у хостинга).\n Работа над ботом все ещё ведется, код грязный и в какой то степени не оптимизированный "
                        "так что, пожалуйста, не спамьте командами, а просто дождитесь ответа.\n"
                        "Проект opensource, если есть желание то можете развернуть бота на своем, более мощном сервере, который может и сможет быстрее обновлять базу."
                        "\n\ngitHub проекта: \nhttps://github.com/aplensi/steamScraper");
}
void telegramBot::answerCommandCommand(int chatId){
    sendMessage(chatId, "Список команд: \n\n"
                        "- /setid:id - связать профиль tg с профилем steam.\n"
                        "- /getprice - получить стоимость инвентаря (только с привязанным профилем steam)\n"
                        "- /showinventory:id - цена любого открытого инвентаря\n\n"
                        "‼️ \"id\" заменять на steamID64. Пример: /setid:123456789 \n(его можно получить тут https://steamid.io )");
}

void telegramBot::sendMessage(int chatId, QString text){
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.telegram.org/bot" + m_token + "/sendMessage?chat_id=" + QString::number(chatId)+"&text=" + text));
    manager->get(request);
    deleteConnections();
    setConnections();
}

void telegramBot::setToken(QString token){
    m_token = token;
}

void telegramBot::answerSetIdCommand(int tgId, QString steamId){
    m_countOfBrockenCheckOfInventory = 0;
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