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

    int getCountOfPages();
    void loadPages(int countOfWidgets, int countOfPages);
    void loadPages(int countOfWidgets);
    void readParsWrite();
public slots:
    void pushToPgSQL(QVector<itemsOfPage> listOfItems);
private:
    bool pgConnected = false;
    QSqlDatabase db;
};

#endif