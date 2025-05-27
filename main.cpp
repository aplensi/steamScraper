#include "include/controller.h"
#include "include/telegramBot.h"

int main(int argc, char** argv){
    QApplication a(argc, argv);

    controller* controll = new controller();
    telegramBot* bot = new telegramBot;

    controll->connectToPgSQL("server", "2345", "0.0.0.0", 5432, "rustskins");
    controll->createTable();
    controll->createTableOfItems();
    controll->createTableAveragePrice();
    controll->createStableListOfData();
    controll->createTableListOfBotUsers();
    controll->createTableOfUsersItems();
    controll->cycleOfProgram();

    bot->setToken("23452345");
    bot->setUpdateFile("updateId");
    bot->cycleOfGetUpdates();
    bot->connectToDb("server", "2345", "0.0.0.0", 5432, "rustskins");
    bot->getUpdates();
    
    return a.exec();
}
