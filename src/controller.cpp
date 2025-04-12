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
    if(m_pgConnected){
        QSqlQuery query(db);
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
    }else{
        qDebug() << "Database is not connected.";
    }
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
            m_reader = new steamReader(currentPage, currentPage + div);
            qDebug() << "widget: " << i + 1 << " from: " << currentPage << " to: " << currentPage + div - 1;
            currentPage += div;
        }
    }
    connect(m_reader, &steamReader::pushToParse, m_parser, &parser::readBuffer);
    connect(m_parser, &parser::sendListOfItems, this, &controller::pushToPgSQL);
}

void controller::loadPages(int countOfWidgets, int countOfPages)
{
    m_countOfPages = countOfPages;
    cycleOfPages(countOfWidgets);
}

void controller::loadPages(int countOfWidgets) 
{
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