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
    void readParsWrite();
    void getCountOfPages();
signals:
    void pagesAreObtained();
public slots:
    void pushToPgSQL(QVector<itemsOfPage> listOfItems);
private:
    steamReader* m_reader;
    parser* m_parser;
    int m_countOfPages = 0;
    bool m_pgConnected = false;
    QSqlDatabase db;
    void cycleOfPages(int countOfWidgets);
};

#endif