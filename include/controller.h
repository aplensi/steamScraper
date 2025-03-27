#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "includes.h"
#include "parser.h"
#include "steamReader.h"

class controller{
public:
    void connectToPgSQL(/* need put parametrs */);
    void loadPages(int countOfWidgets, int countOfPages);
    void loadPages(int countOfWidgets);
    void readParsWrite();
private:
    bool pgConnected = false;
    void writeToSql(itemsOfPage items);
    void writeToSql(std::vector<itemsOfPage>* listOfItems);
};

#endif