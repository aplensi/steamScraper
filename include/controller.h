#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "includes.h"
#include "parser.h"
#include "itemReader.h"

class controller : public QObject{
    Q_OBJECT
public:
    void connecToUsersBD(QString userName, QString passWord, QString address, int port, QString nameDatabase);
    void createStableListOfData();
    void createTableListOfBotUsers();

    void connectToPgSQL(QString userName, QString passWord, QString address, int port, QString nameDatabase);
    void createTable();
    void createTableOfItems();
    void getCountOfItemsInDB();
    void getListOfItemsFromDB();
    void getDataFromDB();
    void setConnectionsOfMethods();
    void startCycleOfProgram();
    void cycleOfProgram();
signals:
    void pagesAreObtained();
    void dataIsPushedToPgSQL();
    void countOfItemsIsObtained();
    void listOfItemsIsObtained();
    void countOfItemsFromDB(int count);
    void listOfItemsFromDB(QVector<itemsOfPage> listOfItems);
    void getMissingItems();
    void continueReadItems();
    void dataIsCompared(QVector<itemsOfPage> listOfNewItems);
    void pushNewDataToPgSQL(QVector<itemsOfPage> listOfNewItems);
    void dataOfItemIsPushedToPgSQL();
    void userAdded(int chatId, QString steamId);
public slots:
    void setListOfItems(QVector<itemsOfPage> listOfItems);
    void setCountOfItems(int count);
    void pushToPgSQL(QVector<itemsOfPage> listOfItems);
    void pushDataOfItemsToPgSQL(QVector<item> listOfItems);
    void compareCountOfItems();
    void compareData();
    void addIdsToNewItems(QVector<itemsOfPage> listOfItems);
    void pushUserToDB(int chatId, QString steamId);
private:
    void pushData(QVector<item> listOfItems, QString tableName);
    parser* m_parser;
    itemReader* m_reader;
    QElapsedTimer m_timer;
    int m_countOfItemsInDB = 0;
    int m_countOfItems = 0;
    bool m_pgConnected = false;
    bool m_pgConnectedToUserBd = false;
    int m_countOfCompares = 0;
    QVector<itemsOfPage> m_listOfItems;
    QVector<itemsOfPage> m_listOfItemsFromDB;
    QVector<itemsOfPage> m_listOfNewItems;
    itemsOfPage m_items;
    PGconn* conn = nullptr;
    PGconn* connToUserBd = nullptr;
};

#endif