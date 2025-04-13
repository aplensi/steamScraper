#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "includes.h"
#include "parser.h"
#include "steamReader.h"

class controller : public QObject{
    Q_OBJECT
public:
    void connectToPgSQL(QString userName, QString passWord, QString address, int port, QString nameDatabase);
    void createTable();

    void loadPages(int countOfWidgets, int countOfPages);
    void loadPages(int countOfWidgets);
    void getCountOfPages();
signals:
    void pagesAreObtained();
public slots:
    void pushToPgSQL(QVector<itemsOfPage> listOfItems);
    void collectDataFromPages(QVector<itemsOfPage> listOfItems);
private:
    steamReader* m_reader;
    parser* m_parser;
    QElapsedTimer m_timer;
    int m_countOfWidgets = 0;
    int m_countOfPages = 0;
    int m_countOfFinished = 0;
    bool m_pgConnected = false;
    QVector<itemsOfPage> m_listOfItems;
    QSqlDatabase db;
    void cycleOfPages(int countOfWidgets);
};

#endif