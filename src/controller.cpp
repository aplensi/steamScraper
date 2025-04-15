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

    PGresult* res = PQexec(conn, "COPY items (name, count, normal_price, sale_price, last_check) FROM STDIN");
    if (PQresultStatus(res) != PGRES_COPY_IN) {
        qDebug() << "COPY command failed:" << PQerrorMessage(conn);
        PQclear(res);
        return;
    }
    PQclear(res);

    for (const auto& item : listOfItems) {
        QString line = QString("%1\t%2\t%3\t%4\t%5\n")
                           .arg(item.m_name)
                           .arg(item.m_count)
                           .arg(item.m_NormalPrice)
                           .arg(item.m_SalePrice)
                           .arg(item.m_lastCheck);
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

    qDebug() << "Time push to sql elapsed:" << timerPush.elapsed() / 1000.0 << "seconds";
    qDebug() << "Timer of program elapsed:" << m_timer.elapsed() / 1000.0 << "seconds";
    qDebug() << "Data pushed to PostgreSQL.";
    emit dataIsPushedToPgSQL();
}

void controller::createTable()
{
    if (!m_pgConnected || conn == nullptr) {
        qDebug() << "Database is not connected.";
        return;
    }

    const char* createTableSQL =
        "CREATE TABLE IF NOT EXISTS items ("
        "name VARCHAR(255), "
        "count INTEGER, "
        "normal_price REAL, "
        "sale_price REAL, "
        "last_check TIMESTAMP);";

    PGresult* res = PQexec(conn, createTableSQL);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        qDebug() << "Error creating table:" << PQerrorMessage(conn);
    } else {
        qDebug() << "Table created successfully.";
    }

    PQclear(res);
}

void controller::getCountOfPages()
{
    steamReader* reader = new steamReader(1);
    parser* pars = new parser();

    QObject::connect(reader, &steamReader::pushToParse, pars, &parser::getCountOfPagesFromBuffer);
    QObject::connect(pars, &parser::sendCountOfPages, [this, reader, pars](int count){
        m_countOfPages = count;
        delete reader;
        delete pars;
        emit pagesAreObtained();
    });
}

void controller::cycleOfPages(int countOfWidgets)
{
    int currentPage = 1;
    int div = m_countOfPages / countOfWidgets;
    qDebug() << "Count of widgets: " << countOfWidgets;
    for(int i = 0; i < countOfWidgets; i++){
        m_parser = new parser();
        if(i + 1 >= countOfWidgets){
            m_reader = new steamReader(currentPage,  m_countOfPages);
            qDebug() << "widget: " << i + 1 << " from: " << currentPage << " to: " << m_countOfPages;
        } else{
            m_reader = new steamReader(currentPage, currentPage + div - 1);
            qDebug() << "widget: " << i + 1 << " from: " << currentPage << " to: " << currentPage + div - 1;
            currentPage += div;
        }
        connect(m_reader, &steamReader::pushToParse, m_parser, &parser::readBuffer);
        connect(m_parser, &parser::sendListOfItems, this, &controller::collectDataFromPages);
        connect(m_reader, &steamReader::readerFinished, [this, countOfWidgets](){
            m_countOfFinished++;
            if(m_countOfFinished == countOfWidgets){
                qDebug() << "All readers finished.";
            }
        });
    }
}

void controller::collectDataFromPages(QVector<itemsOfPage> listOfItems) 
{
    m_listOfItems.append(listOfItems);
    qDebug() << "List of items size: " << m_listOfItems.size();
    qDebug() << "Count of finished: " << m_countOfFinished << " from: " << m_countOfWidgets;
    if(m_countOfFinished == m_countOfWidgets){
        qDebug() << "All items collected.";
        delete m_reader;
        delete m_parser;
        m_reader = nullptr;
        m_parser = nullptr;
        m_countOfFinished = 0;
        pushToPgSQL(m_listOfItems);
        m_listOfItems.clear();
    }
}

void controller::loadPages(int countOfWidgets, int countOfPages)
{
    m_timer.start();
    m_countOfWidgets = countOfWidgets;
    m_countOfPages = countOfPages;
    cycleOfPages(countOfWidgets);
}

void controller::loadPages(int countOfWidgets, int countOfPages, bool cycle)
{
    m_timer.start();
    m_countOfWidgets = countOfWidgets;
    m_countOfPages = countOfPages;
    m_cycle = cycle;
    loadPages();
}

void controller::loadPages(int countOfWidgets) 
{
    m_timer.start();
    m_countOfWidgets = countOfWidgets;
    loadPages();
}

void controller::loadPages(int countOfWidgets, bool cycle)
{
    m_timer.start();
    m_countOfWidgets = countOfWidgets;
    m_cycle = cycle;
    loadPages();
}

void controller::loadPages()
{
    m_timer.start();
    if(m_countOfPages == 0){
        getCountOfPages();
        connect(this, &controller::pagesAreObtained, [this](){
            qDebug() << "Count of pages: " << m_countOfPages;
            cycleOfPages(m_countOfWidgets);
        });
    }else{
        cycleOfPages(m_countOfWidgets);
    }

    if(!m_inCycle){
        m_inCycle = true;
        connect(this, &controller::dataIsPushedToPgSQL, [this](){
            delete m_reader;
            delete m_parser;
            if(m_cycle){
                loadPages();
            } else{
                qDebug() << "Program finished.";
                exit(0);
            }
        });
    }
}