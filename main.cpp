#include "include/controller.h"

int main(int argc, char** argv){

    QApplication a(argc, argv);

    controller* controll = new controller();
    controll->connectToPgSQL("postgres", "19458798", "193.29.224.114", 5432, "rustskins");
    controll->createTable();
    controll->loadPages(30);

    return a.exec();
}
