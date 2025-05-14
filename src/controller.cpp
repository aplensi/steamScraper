#include "../include/controller.h"

void controller::connectToPgSQL(QString userName, QString passWord, QString address, int port, QString nameDatabase)
{
    QString connInfo = QString("host=%1 port=%2 dbname=%3 user=%4 password=%5")
                           .arg(address)
                           .arg(port)
                           .arg(nameDatabase)
                           .arg(userName)
                           .arg(passWord);

    conn = PQconnectdb(connInfo.toUtf8().constData());

    if (PQstatus(conn) == CONNECTION_OK) {
        m_pgConnected = true;
        qDebug() << "db is connected.";
    } else {
        m_pgConnected = false;
        qDebug() << "db isn't connected:" << PQerrorMessage(conn);
        PQfinish(conn);
        conn = nullptr;
    }
}

void controller::pushToPgSQL(QVector<itemsOfPage> listOfItems)
{
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    PGresult* res = PQexec(conn, "COPY items (name, id) FROM STDIN");
    if (PQresultStatus(res) != PGRES_COPY_IN) {
        qDebug() << "COPY command failed:" << PQerrorMessage(conn);
        PQclear(res);
        return;
    }
    PQclear(res);

    for (const auto& item : listOfItems) {
        QString line = QString("%1\t%2\n")
                           .arg(item.m_name)
                           .arg(item.m_id);
        QByteArray utf8Line = line.toUtf8();
        if (PQputCopyData(conn, utf8Line.constData(), utf8Line.size()) != 1) {
            qDebug() << "Error sending data:" << PQerrorMessage(conn);
            PQputCopyEnd(conn, "Error during COPY");
            return;
        }
    }

    if (PQputCopyEnd(conn, nullptr) != 1) {
        qDebug() << "Error ending COPY:" << PQerrorMessage(conn);
        return;
    }

    PGresult* copyRes;
    while ((copyRes = PQgetResult(conn)) != nullptr) {
        if (PQresultStatus(copyRes) != PGRES_COMMAND_OK) {
            qDebug() << "COPY failed:" << PQerrorMessage(conn);
            PQclear(copyRes);
            return;
        }
        PQclear(copyRes);
    }

    qDebug() << "Data pushed to PostgreSQL.";
    emit dataIsPushedToPgSQL();
}

void controller::pushData(QVector<item> listOfItems, QString tableName){
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    PGresult* res;

    if(tableName == "stablelist"){
        res = PQexec(conn, "TRUNCATE TABLE stablelist");
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            qDebug() << "TRUNCATE stablelist failed:" << PQerrorMessage(conn);
            PQclear(res);
            return;
        }
        PQclear(res);
    }

    QString queryStr = QString("COPY %1 (id, sellordercount, sellorderprice, buyordercount, buyorderprice) FROM STDIN")
                           .arg(tableName);

    res = PQexec(conn, queryStr.toUtf8().constData());
    if (PQresultStatus(res) != PGRES_COPY_IN) {
        qDebug() << "COPY command failed:" << PQerrorMessage(conn);
        PQclear(res);
        return;
    }
    PQclear(res);

    for (const auto& item : listOfItems) {
        QString line = QString("%1\t%2\t%3\t%4\t%5\n")
                           .arg(item.m_id)
                           .arg(item.m_countOfSale)
                           .arg(item.m_salePrice)
                           .arg(item.m_countOfPurchase)
                           .arg(item.m_purchasePrice);
        QByteArray utf8Line = line.toUtf8();
        if (PQputCopyData(conn, utf8Line.constData(), utf8Line.size()) != 1) {
            qDebug() << "Error sending data:" << PQerrorMessage(conn);
            PQputCopyEnd(conn, "Error during COPY");
            return;
        }
    }

    if (PQputCopyEnd(conn, nullptr) != 1) {
        qDebug() << "Error ending COPY:" << PQerrorMessage(conn);
        return;
    }

    PGresult* copyRes;
    while ((copyRes = PQgetResult(conn)) != nullptr) {
        if (PQresultStatus(copyRes) != PGRES_COMMAND_OK) {
            qDebug() << "COPY failed:" << PQerrorMessage(conn);
            PQclear(copyRes);
            return;
        }
        PQclear(copyRes);
    }
}

void controller::pushDataOfItemsToPgSQL(QVector<item> listOfItems)
{
    pushData(listOfItems, "stablelist");
    pushData(listOfItems, "pricesOfItems");
    qDebug() << "Data pushed to PostgreSQL.";
    qDebug() << "Duration: " << m_timer.elapsed()/1000 << "s | Time: " << QDateTime::currentDateTime();
    emit dataOfItemIsPushedToPgSQL();
}

void controller::pushUserToDB(int chatId, QString steamId) {
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    QString query = QString("INSERT INTO users (tgid, steamid) VALUES (%1, '%2') "
                            "ON CONFLICT (tgid) DO UPDATE SET steamid = '%2';")
                            .arg(chatId)
                            .arg(steamId);

    PGresult* res = PQexec(conn, query.toUtf8().constData());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        qDebug() << "INSERT/UPDATE command failed:" << PQerrorMessage(conn);
        PQclear(res);
        return;
    }

    PQclear(res);
    qDebug() << "Data pushed to users.";
    emit userAdded(chatId, steamId);
}

void controller::addIdsToNewItems(QVector<itemsOfPage> listOfItems)
{
    for(auto i : listOfItems){
        for(auto& j : m_listOfNewItems){
            if(i.m_name == j.m_name){
                j.m_id = i.m_id;
                break;
            }
        }
    }
    emit pushNewDataToPgSQL(m_listOfNewItems);
    for(auto i : m_listOfNewItems){
        qDebug() << "New item: " << i.m_name << " ID: " << i.m_id;
    }
}

void controller::createTable()
{
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    const char* createTableSQL =
        "CREATE TABLE IF NOT EXISTS items ("
        "name VARCHAR(255) PRIMARY KEY, "
        "id INTEGER);";

    PGresult* res = PQexec(conn, createTableSQL);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        qDebug() << "Error creating table:" << PQerrorMessage(conn);
    } else {
        qDebug() << "Table created successfully.";
    }

    PQclear(res);
}

void controller::createTableOfItems()
{
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    const char* createTableSQL =
        "CREATE TABLE IF NOT EXISTS pricesOfItems ("
        "id INTEGER, "
        "sellOrderCount INTEGER,"
        "sellOrderPrice FLOAT,"
        "buyOrderCount INTEGER,"
        "buyOrderPrice FLOAT,"
        "lastUpdate TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

    PGresult* res = PQexec(conn, createTableSQL);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        qDebug() << "Error creating table:" << PQerrorMessage(conn);
    } else {
        qDebug() << "Table created successfully.";
    }

    PQclear(res);
}

void controller::createStableListOfData(){
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    const char* createTableSQL =
        "CREATE TABLE IF NOT EXISTS stablelist ("
        "id INTEGER, "
        "sellOrderCount INTEGER,"
        "sellOrderPrice FLOAT,"
        "buyOrderCount INTEGER,"
        "buyOrderPrice FLOAT,"
        "lastUpdate TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

    PGresult* res = PQexec(conn, createTableSQL);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        qDebug() << "Error creating table:" << PQerrorMessage(conn);
    } else {
        qDebug() << "Table created successfully.";
    }

    PQclear(res);
}

void controller::createTableAveragePrice(){
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    const char* createTableSQL =
        "CREATE TABLE IF NOT EXISTS tableaverageprice ("
        "id INTEGER, "
        "sellOrderCount INTEGER,"
        "sellOrderPrice FLOAT,"
        "buyOrderCount INTEGER,"
        "buyOrderPrice FLOAT,"
        "lastUpdate DATE);";

    PGresult* res = PQexec(conn, createTableSQL);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        qDebug() << "Error creating table:" << PQerrorMessage(conn);
    } else {
        qDebug() << "Table created successfully.";
    }

    PQclear(res);
}

void controller::createTableListOfBotUsers(){
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    const char* createTableSQL =
        "CREATE TABLE IF NOT EXISTS users ("
        "tgId INTEGER PRIMARY KEY, "
        "steamId VARCHAR(255));";

    PGresult* res = PQexec(conn, createTableSQL);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        qDebug() << "Error creating table:" << PQerrorMessage(conn);
    } else {
        qDebug() << "Table created successfully.";
    }

    PQclear(res);
}

void controller::createTableOfUsersItems(){
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }
    const char* createTableSQL =
        "CREATE TABLE IF NOT EXISTS usersItems ("
        "tgId INTEGER, "
        "name VARCHAR(255),"
        "count INTEGER);";
    
    PGresult* res = PQexec(conn, createTableSQL);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        qDebug() << "Error creating table:" << PQerrorMessage(conn);
    } else {
        qDebug() << "Users inventories table created successfully.";
    }

    PQclear(res);
}

void controller::pushUserInventoryToDb(int tgId, userInventory inventory){
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    std::string tgIdStr = std::to_string(tgId);
    const char* paramValues[1];
    paramValues[0] = tgIdStr.c_str();
    PGresult* res = PQexecParams(conn,
                                  "DELETE FROM usersItems WHERE tgId = $1",
                                  1,
                                  NULL,
                                  paramValues,
                                  NULL,
                                  NULL,
                                  0); 
    PQclear(res);

    res = PQexec(conn, "COPY usersItems (tgId, name, count) FROM STDIN");
    if (PQresultStatus(res) != PGRES_COPY_IN) {
        qDebug() << "COPY command failed:" << PQerrorMessage(conn);
        PQclear(res);
        return;
    }
    PQclear(res);

    for (const auto& item : inventory.m_listOfItems) {
        QString line = QString("%1\t%2\t%3\n")
                            .arg(tgId)
                           .arg(item.m_name)
                           .arg(item.m_count);
        QByteArray utf8Line = line.toUtf8();
        if (PQputCopyData(conn, utf8Line.constData(), utf8Line.size()) != 1) {
            qDebug() << "Error sending data:" << PQerrorMessage(conn);
            PQputCopyEnd(conn, "Error during COPY");
            return;
        }
    }

    if (PQputCopyEnd(conn, nullptr) != 1) {
        qDebug() << "Error ending COPY:" << PQerrorMessage(conn);
        return;
    }

    PGresult* copyRes;
    while ((copyRes = PQgetResult(conn)) != nullptr) {
        if (PQresultStatus(copyRes) != PGRES_COMMAND_OK) {
            qDebug() << "COPY failed:" << PQerrorMessage(conn);
            PQclear(copyRes);
            return;
        }
        PQclear(copyRes);
    }

    qDebug() << "Inventory of user " << QString::number(tgId) << " pushed to PostgreSQL.";
}

void controller::getCountOfItemsInDB()
{
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    const char* countSQL = "SELECT COUNT(*) FROM items;";
    PGresult* res = PQexec(conn, countSQL);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        qDebug() << "Error getting count of items:" << PQerrorMessage(conn);
    } else {
        int count = atoi(PQgetvalue(res, 0, 0));
        m_countOfItemsInDB = count;
        emit countOfItemsFromDB(count);
    }

    PQclear(res);

}

void controller::getSteamIdOfUser(int tgId){
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    const char* countSQL = "SELECT steamid FROM users WHERE tgid = $1;;";
    const char* paramValues[1];
    paramValues[0] = std::to_string(tgId).c_str();

    PGresult* res = PQexecParams(conn, countSQL, 1, nullptr, paramValues, nullptr, nullptr, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        qDebug() << "Error getting count of items:" << PQerrorMessage(conn);
    } else {
        if(PQgetvalue(res, 0, 0) != nullptr){
            QString steamId = QString::fromStdString(PQgetvalue(res, 0, 0));
            qDebug() << "Steam ID: " << steamId;
            emit setSteamIdOfUser(tgId, steamId);
        }else{
            emit userIsNotRegistered(tgId);
        }
    }
}

void controller::fillUserInventory(int chatId, userInventory usInv) {
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    if (usInv.m_listOfItems.isEmpty()) {
        qDebug() << "Item list is empty.";
        return;
    }

    QString query = "SELECT items.name, stablelist.id, stablelist.sellorderprice, stablelist.sellordercount "
                    "FROM stablelist "
                    "JOIN items ON items.id = stablelist.id "
                    "WHERE items.name IN (";

    for (size_t i = 0; i < usInv.m_listOfItems.size(); ++i) {
        query += "$" + QString::number(i + 1);
        if (i < usInv.m_listOfItems.size() - 1) {
            query += ", ";
        }
    }
    query += ")";

    const char* queryCStr = query.toUtf8().constData();

    QVector<QByteArray> paramValues(usInv.m_listOfItems.size());
    for (size_t i = 0; i < usInv.m_listOfItems.size(); ++i) {
        paramValues[i] = usInv.m_listOfItems[i].m_name.toUtf8();
    }

    const char* paramPointers[usInv.m_listOfItems.size()];
    for (size_t i = 0; i < usInv.m_listOfItems.size(); ++i) {
        paramPointers[i] = paramValues[i].constData();
    }

    PGresult* res = PQexecParams(conn, queryCStr, usInv.m_listOfItems.size(), nullptr, paramPointers, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        qDebug() << "Error getting count of items:" << PQerrorMessage(conn);
    } else {
        int numRows = PQntuples(res);
        for (int i = 0; i < numRows; ++i) {
            QString itemName = QString::fromUtf8(PQgetvalue(res, i, 0));
            for (auto& j : usInv.m_listOfItems) {
                if (j.m_name == itemName) {
                    j.m_countOfOffers = std::stoi(PQgetvalue(res, i, 3));
                    j.m_price = std::stod(PQgetvalue(res, i, 2));
                }
            }
        }
    }

    emit invOfUserIsFilled(chatId, usInv);

    PQclear(res);
}

void controller::getInventoryOfUserFromDb(int chatId, QString steamId){
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    const char* countSQL = "SELECT usersitems.name, usersitems.count, sl.sellorderprice, sl.sellordercount"
                            "FROM usersitems"
                            "JOIN items itm ON itm.name = usersitems.name"
                            "JOIN stablelist sl ON itm.id = sl.id"
                            "WHERE tgid = $1;;";
    const char* paramValues[1];
    paramValues[0] = std::to_string(chatId).c_str();

    userInventory usInv;
    userItems usItm;
    PGresult* res = PQexecParams(conn, countSQL, 1, nullptr, paramValues, nullptr, nullptr, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        qDebug() << "Error getting count of items:" << PQerrorMessage(conn);
    } else {
        if(PQgetvalue(res, 0, 0) != nullptr){
            int numRows = PQntuples(res);
            for (int i = 0; i < numRows; ++i) {
                usItm.m_name = QString::fromUtf8(PQgetvalue(res, i, 0));
                usItm.m_count = std::stoi(PQgetvalue(res, i, 1));
                usItm.m_price = std::stod(PQgetvalue(res, i, 2));
                usItm.m_countOfOffers = std::stoi(PQgetvalue(res, i, 3));
                usInv.m_listOfItems.append(usItm);
            }
            emit sendUserInventory(chatId, usInv);
        }else{
            emit dontHaveSuchUser(chatId);
        }
    }
}

void controller::checkTime(){
    QTime currentTime = QTime::currentTime();
    if (currentTime >= QTime(0, 0) && currentTime < QTime(1, 0) && m_checkInMidNight == false) {
        m_checkInMidNight = true;
        emit timeInRange();
    }else if(currentTime >= QTime(1, 0)){
        m_checkInMidNight = false;
        emit dayDataInDbIsCollected();
    }else{
        emit dayDataInDbIsCollected();
    }
}

void controller::collectDayData(){
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    const char* countSQL =  "BEGIN; "
                            "INSERT INTO tableaverageprice ("
                                "id, "
                                "sellOrderCount, "
                                "sellOrderPrice, "
                                "buyOrderCount, "
                                "buyOrderPrice, "
                                "lastUpdate) "
                            "SELECT "
                                "id, "
                                "AVG(sellOrderCount)::INTEGER, "
                                "ROUND(AVG(sellOrderPrice)::numeric, 2), "
                                "AVG(buyOrderCount)::INTEGER, "
                                "ROUND(AVG(buyOrderPrice)::numeric, 2), "
                                "NOW() "
                            "FROM "
                                "pricesofitems "
                            "WHERE "
                                "lastUpdate >= NOW() - INTERVAL '1 day' "
                            "GROUP BY "
                                "id; "
                            "TRUNCATE TABLE pricesofitems; "
                            "COMMIT;";

    PGresult* res = PQexec(conn, countSQL);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        qDebug() << "Error collect data:" << PQerrorMessage(conn);
    } else {
        emit dayDataInDbIsCollected();
    }

    PQclear(res);

}

void controller::setCountOfItems(int count)
{
    m_countOfItems = count;
    emit countOfItemsIsObtained();
}

void controller::getListOfItemsFromDB()
{
    m_listOfItemsFromDB.clear();
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;  // Everything will be fine - your own will stab you with a knife. You are sick in the head - that means you are armed.
    }

    const char* selectSQL = "SELECT name, id FROM items;";
    PGresult* res = PQexec(conn, selectSQL);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        qDebug() << "Error getting list of items:" << PQerrorMessage(conn);
    } else {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; ++i) {
            m_items.m_name = PQgetvalue(res, i, 0);
            m_items.m_id = atoi(PQgetvalue(res, i, 1));
            m_listOfItemsFromDB.append(m_items);
        }
    }

    PQclear(res);
    emit listOfItemsFromDB(m_listOfItemsFromDB);
}

void controller::compareCountOfItems()
{
    m_countOfCompares++;
    if(m_countOfCompares == 2){
        if(m_countOfItems == m_countOfItemsInDB){
            qDebug() << "Count of items in DB is equal to count of items in Steam. \nCount: " << m_countOfItems;
            emit continueReadItems();
            m_countOfCompares = 0;
        }else{
            qDebug() << "Appeared new items";
            qDebug() << "Count of items in DB: " << m_countOfItemsInDB;
            qDebug() << "Count of items in Steam: " << m_countOfItems;
            emit getMissingItems();
            m_countOfCompares = 0;
        }
    }
}

void controller::compareData()
{
    bool found;
    for(auto& i : m_listOfItems){
        found = false;
        for(auto& j : m_listOfItemsFromDB){
            if(i.m_name == j.m_name){
                found = true;
                break;
            }
        }
        if(!found){
            m_listOfNewItems.append(i);
        }
    }
    qDebug() << "Count of new items: " << m_listOfNewItems.length();
    emit dataIsCompared(m_listOfNewItems);
}

void controller::setListOfItems(QVector<itemsOfPage> listOfItems)
{
    m_listOfItems = listOfItems;
    emit listOfItemsIsObtained();
}

void controller::getDataFromDB()
{
    getCountOfItemsInDB();
    getListOfItemsFromDB();
}

void controller::cycleOfProgram(){
    m_reader = new itemReader();
    m_parser = new parser();
    startCycleOfProgram();
}

void controller::startCycleOfProgram()
{
    disconnect(this, nullptr, nullptr, nullptr);
    disconnect(m_parser, nullptr, nullptr, nullptr);
    disconnect(m_reader, nullptr, nullptr, nullptr);
    setConnectionsOfMethods();
    m_timer.start();
    m_listOfNewItems.clear();
    getDataFromDB();
    m_reader->getCountOfItemsJson();
}

void controller::setConnectionsOfMethods(){

    // проверка соответствия данных в дб и в steam
    connect(this, &controller::listOfItemsFromDB, m_parser, &parser::setListOfItemsDB); // получаем количество предметов в БД
    connect(this, &controller::countOfItemsFromDB, m_parser, &parser::setCountOfDBItems); // получаем количество предметов в БД
    connect(m_reader, &itemReader::getCountOfItemsIsFinished, m_parser, &parser::getCountOfItemsFromJson); // получаем данные со стима и отправляем парситься
    connect(m_parser, &parser::countOfItemsIsNull, m_reader, &itemReader::getCountOfItemsJson); // проверяем наличие ошибки в запросе на получение кол-ва предметов
    connect(m_parser, &parser::sendCountOfPages, this, &controller::setCountOfItems); // отправляем пропарсенные данные в контроллер
    connect(this, &controller::countOfItemsIsObtained, this, &controller::compareCountOfItems); // сравниваем данные стима с БД
    connect(this, &controller::countOfItemsFromDB, this, &controller::compareCountOfItems); // сравниваем данные БД с данными стима

    // ежедневная сборка данных
    connect(this, &controller::dataIsPushedToPgSQL, this, &controller::checkTime); // проверяем время после каждого цикла
    connect(this, &controller::dataOfItemIsPushedToPgSQL, this, &controller::checkTime); // проверяем время после каждого цикла
    connect(this, &controller::timeInRange, this, &controller::collectDayData); // сборка и очистка данных после наступления времени
    connect(this, &controller::dayDataInDbIsCollected, this, &controller::startCycleOfProgram); // продолжение цикла

     // получаем недостающие данные
    connect(this, &controller::getMissingItems, [this](){
        disconnect(m_parser, &parser::sendCountOfPages, nullptr, nullptr);
        connect(m_parser, &parser::sendCountOfPages, m_reader, &itemReader::cycleOfReadItems); // отправляем количество данных в цикл чтения
        connect(m_reader, &itemReader::readCatalogIsFinished, m_parser, &parser::readItemsFromJson); // отправляем пачку данных в парсер
        connect(m_parser, &parser::brockenRequest, m_reader, &itemReader::readItems); // в случае ошибки повторяем запрос
        connect(m_parser, &parser::namesIsFilled, this, &controller::setListOfItems); // устанавливаем список названий предметов в контроллер
        connect(this, &controller::listOfItemsIsObtained, this, &controller::compareData); // сравниваем данные стима с БД
        connect(this, &controller::dataIsCompared, m_reader, &itemReader::cycleOfReadPages); // отправляем заполненный список названий в цикл получения id предмета и читаем по пачкам
        connect(m_reader, &itemReader::readPageOfItemIsFinished, m_parser, &parser::parsPageOfItem); // отправляем html код страницы скина в парсер
        connect(m_parser, &parser::heapIsFinished, m_reader, &itemReader::cycleOfReadPages); // отправляем оставшиеся предметы в цикл получения id предмета
        connect(m_parser, &parser::brockenPageOfItem, m_reader, &itemReader::readPageOfItem); // в случае ошибки повторяем запрос
        connect(m_parser, &parser::namesAndIdsIsReceived, this, &controller::addIdsToNewItems); // получаем id предметов
        connect(this, &controller::pushNewDataToPgSQL, this, &controller::pushToPgSQL); // отправляем данные в БД
        m_reader->getCountOfItemsJson();
    });

    // получаем данные скина
    connect(this, &controller::continueReadItems, [this](){
        connect(this, &controller::listOfItemsFromDB, m_reader, &itemReader::cycleOfLoadingDataOfItem); // устанавливаем список названий предметов в контроллер
        connect(m_reader, &itemReader::sendJsonOfData, m_parser, &parser::parsDataOfItem); // отправляем json с данными в парсер
        connect(m_parser, &parser::dataOfItemIsReceived, m_reader, &itemReader::cycleOfLoadingDataOfItem); // получаем данные из БД
        connect(m_parser, &parser::gettingDataIsOvered, this, &controller::pushDataOfItemsToPgSQL); // отправляем данные в БД
        getListOfItemsFromDB();
    });
}