#include "include/controller.h"

int main(int argc, char** argv){

    QApplication a(argc, argv);

    controller controll;
    controll.connectToPgSQL("postgres", "123", "0.0.0.0", 5432, "rustskins");
    controll.createTable();
    controll.loadPages(8);

    return a.exec();
}
