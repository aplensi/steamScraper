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
    QElapsedTimer timerPush;
    timerPush.start();

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

void controller::pushDataOfItemsToPgSQL(QVector<item> listOfItems)
{

    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    PGresult* res = PQexec(conn, "COPY pricesofitems (id, sellordercount, sellorderprice, buyordercount, buyorderprice) FROM STDIN");
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

    qDebug() << "Data pushed to PostgreSQL.";
    emit dataOfItemIsPushedToPgSQL();
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

void controller::getDataFromDB()
{
    getCountOfItemsInDB();
    getListOfItemsFromDB();
}

void controller::setCountOfItems(int count)
{
    m_countOfItems = count;
    emit countOfItemsIsObtained();
}

void controller::getListOfItemsFromDB()
{
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

void controller::startCycleOfProgram()
{
    setConnectionsOfMethods();
    if(!m_inCycle){
        connect(this, &controller::dataIsPushedToPgSQL, [this](){
            QTimer::singleShot(2000, [this](){
                startCycleOfProgram();
            });
        });
        connect(this, &controller::dataOfItemIsPushedToPgSQL, [this](){
            QTimer::singleShot(2000, [this](){
                startCycleOfProgram();
            });
        });
        m_inCycle = true;
    }
    m_listOfItems.clear();
    m_listOfNewItems.clear();
    m_listOfItemsFromDB.clear();
    getDataFromDB();
    m_reader->getCountOfItemsJson();
}

void controller::setConnectionsOfMethods(){
    QObject::disconnect(this, nullptr, nullptr, nullptr);
    QObject::disconnect(m_reader, nullptr, nullptr, nullptr);
    QObject::disconnect(m_parser, nullptr, nullptr, nullptr);
    delete m_reader;
    delete m_parser;
    m_reader = new itemReader();
    m_parser = new parser();
    QObject::connect(this, &controller::listOfItemsFromDB, m_parser, &parser::setListOfItemsDB); // получаем количество предметов в БД
    QObject::connect(this, &controller::countOfItemsFromDB, m_parser, &parser::setCountOfDBItems); // получаем количество предметов в БД
    QObject::connect(m_reader, &itemReader::getCountOfItemsIsFinished, m_parser, &parser::getCountOfItemsFromJson); // получаем данные со стима и отправляем парситься
    QObject::connect(m_parser, &parser::sendCountOfPages, this, &controller::setCountOfItems); // отправляем пропарсенные данные в контроллер
    QObject::connect(this, &controller::countOfItemsIsObtained, this, &controller::compareCountOfItems); // сравниваем данные стима с БД
    QObject::connect(this, &controller::countOfItemsFromDB, this, &controller::compareCountOfItems); // сравниваем данные БД с данными стима

    QObject::connect(this, &controller::getMissingItems, [this](){
        QObject::connect(m_parser, &parser::sendCountOfPages, m_reader, &itemReader::cycleOfReadItems); // отправляем количество данных в цикл чтения
        QObject::connect(m_reader, &itemReader::readCatalogIsFinished, m_parser, &parser::readItemsFromJson); // отправляем пачку данных в парсер
        QObject::connect(m_parser, &parser::brockenRequest, m_reader, &itemReader::readItems); // в случае ошибки повторяем запрос
        QObject::connect(m_parser, &parser::namesIsFilled, this, &controller::setListOfItems); // устанавливаем список названий предметов в контроллер
        QObject::connect(this, &controller::listOfItemsIsObtained, this, &controller::compareData); // сравниваем данные стима с БД
        QObject::connect(this, &controller::dataIsCompared, m_reader, &itemReader::cycleOfReadPages); // отправляем заполненный список названий в цикл получения id предмета и читаем по пачкам
        QObject::connect(m_reader, &itemReader::readPageOfItemIsFinished, m_parser, &parser::parsPageOfItem); // отправляем html код страницы скина в парсер
        QObject::connect(m_parser, &parser::heapIsFinished, m_reader, &itemReader::cycleOfReadPages); // отправляем оставшиеся предметы в цикл получения id предмета
        QObject::connect(m_parser, &parser::brockenPageOfItem, m_reader, &itemReader::pageWithTooManyRequests); // в случае ошибки повторяем запрос
        QObject::connect(m_parser, &parser::namesAndIdsIsReceived, this, &controller::addIdsToNewItems); // получаем id предметов
        QObject::connect(this, &controller::pushNewDataToPgSQL, this, &controller::pushToPgSQL); // отправляем данные в БД
        m_reader->getCountOfItemsJson();
    }); // получаем недостающие данные

    QObject::connect(this, &controller::continueReadItems, [this](){
        QObject::connect(this, &controller::listOfItemsFromDB, m_reader, &itemReader::cycleOfLoadingDataOfItem); // устанавливаем список названий предметов в контроллер
        QObject::connect(m_reader, &itemReader::sendJsonOfData, m_parser, &parser::parsDataOfItem); // отправляем json с данными в парсер
        QObject::connect(m_parser, &parser::dataOfItemIsReceived, m_reader, &itemReader::cycleOfLoadingDataOfItem); // получаем данные из БД
        getListOfItemsFromDB();
        QObject::connect(m_parser, &parser::gettingDataIsOvered, this, &controller::pushDataOfItemsToPgSQL); // отправляем данные в БД
    }); // получаем данные скина
}