#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "includes.h"
#include "parser.h"
#include "steamReader.h"
#include "itemReader.h"

class controller : public QObject{
    Q_OBJECT
public:
    void connectToPgSQL(QString userName, QString passWord, QString address, int port, QString nameDatabase);
    void createTable();
    void getCountOfItemsInDB();
    void getListOfItemsFromDB();

    void loadPages(int countOfWidgets, int countOfPages, bool cycle);
    void loadPages(int countOfWidgets, bool cycle);
    void loadPages(int countOfWidgets, int countOfPages);
    void loadPages(int countOfWidgets);
    void getCountOfPages();
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
public slots:
    void setListOfItems(QVector<itemsOfPage> listOfItems);
    void setCountOfItems(int count);
    void pushToPgSQL(QVector<itemsOfPage> listOfItems);
    void collectDataFromPages(QVector<itemsOfPage> listOfItems);
    void compareCountOfItems();
    void compareData();
private:
    steamReader* m_reader;
    parser* m_parser;
    QElapsedTimer m_timer;
    int m_countOfWidgets = 0;
    int m_countOfPages = 0;
    int m_countOfFinished = 0;
    int m_countOfItemsInDB = 0;
    int m_countOfItems = 0;
    bool m_pgConnected = false;
    bool m_cycle = false;
    bool m_inCycle = false;
    QVector<itemsOfPage> m_listOfItems;
    QVector<itemsOfPage> m_listOfItemsFromDB;
    QVector<itemsOfPage> m_listOfNewItems;
    itemsOfPage m_items;
    PGconn* conn = nullptr;
    void cycleOfPages(int countOfWidgets);
    void loadPages();
};

#endif