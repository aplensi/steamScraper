#include "include/controller.h"

int main(int argc, char** argv){
    QApplication a(argc, argv);
    controller* controll = new controller();
    controll->connectToPgSQL("server", "123", "0.0.0.0", 5432, "qwer");
    controll->createTable();
    controll->createTableOfItems();
    controll->startCycleOfProgram();

    return a.exec();
}
