#include "../include/controller.h"

void controller::connectToPgSQL(QString userName, QString passWord, QString address, int port, QString nameDatabase)
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName(nameDatabase);
    db.setUserName(userName);
    db.setPassword(passWord);
    db.setHostName(address);
    db.setPort(port);
    pgConnected = db.open();
    if(pgConnected){
        qDebug() << "db is connected.";
    }else{
        qDebug() << "db isn't connected. " << db.lastError();
        QStringList drivers = QSqlDatabase::drivers();
        qDebug() << "Available drivers:" << drivers;
    }
}
void controller::pushToPgSQL(QVector<itemsOfPage> listOfItems)
{
    if(pgConnected){
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
    if(pgConnected){
        QSqlQuery query(db);
        query.exec("CREATE TABLE IF NOT EXISTS items ("
                   "name VARCHAR(255), "
                   "count INTEGER, "
                   "normal_price REAL, "
                   "sale_price REAL, "
                   "last_check TEXT);");
        if(query.lastError().isValid()){
            qDebug() << "Error creating table:" << query.lastError();
        }else{
            qDebug() << "Table created successfully.";

        }
    }else{
        qDebug() << "Database is not connected.";
    }
}

int controller::getCountOfPages()
{
    steamReader* reader = new steamReader(1,1);
    parser* pars = new parser();
    QObject::connect(reader, &steamReader::pushToParse, pars, &parser::getCountOfPagesFromBuffer);
    return 0;
}