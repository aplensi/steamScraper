#include "../include/controller.h"

void controller::connectToPgSQL(QString userName, QString passWord, QString address, int port, QString nameDatabase)
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName(nameDatabase);
    db.setUserName(userName);
    db.setPassword(passWord);
    db.setHostName(address);
    db.setPort(port);
    m_pgConnected = db.open();
    if(m_pgConnected){
        qDebug() << "db is connected.";
    }else{
        qDebug() << "db isn't connected. " << db.lastError();
        QStringList drivers = QSqlDatabase::drivers();
        qDebug() << "Available drivers:" << drivers;
    }
}
void controller::pushToPgSQL(QVector<itemsOfPage> listOfItems)
{
    QElapsedTimer timerPush;
    timerPush.start();

    if(m_pgConnected){
        QSqlQuery query(db);

        if (!db.transaction()) {
            qDebug() << "Failed to start transaction:" << db.lastError();
            return;
        }

        for(const auto& item : listOfItems){
            query.prepare("INSERT INTO items (name, count, normal_price, sale_price, last_check) "
                          "VALUES (:name, :count, :normal_price, :sale_price, :last_check);");
            query.bindValue(":name", item.m_name);
            query.bindValue(":count", item.m_count);
            query.bindValue(":normal_price", item.m_NormalPrice);
            query.bindValue(":sale_price", item.m_SalePrice);
            query.bindValue(":last_check", item.m_lastCheck);
            if(!query.exec()){
                qDebug() << "Error inserting data:" << query.lastError();
            }
        }

        if (!db.commit()) {
            qDebug() << "Failed to commit transaction:" << db.lastError();
        }

    }else{
        qDebug() << "Database is not connected.";
    }
    qDebug() << "Time push to sql elapsed:" << timerPush.elapsed() / 1000.0 << "seconds";
    qDebug() << "Timer of program elapsed:" << m_timer.elapsed() / 1000.0 << "seconds";
    qDebug() << "Data pushed to PostgreSQL.";
}

void controller::createTable()
{
    if(m_pgConnected){
        QSqlQuery query(db);
        query.exec("CREATE TABLE IF NOT EXISTS items ("
                   "name VARCHAR(255), "
                   "count INTEGER, "
                   "normal_price REAL, "
                   "sale_price REAL, "
                   "last_check TIMESTAMP);");
        if(query.lastError().isValid()){
            qDebug() << "Error creating table:" << query.lastError();
        }else{
            qDebug() << "Table created successfully.";

        }
    }else{
        qDebug() << "Database is not connected.";
    }
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

void controller::loadPages(int countOfWidgets) 
{
    m_countOfWidgets = countOfWidgets;
    m_timer.start();
    if(m_countOfPages == 0){
        getCountOfPages();
        connect(this, &controller::pagesAreObtained, [this, countOfWidgets](){
            qDebug() << "Count of pages: " << m_countOfPages;
            cycleOfPages(countOfWidgets);
        });
    }else{
        cycleOfPages(countOfWidgets);
    }
}