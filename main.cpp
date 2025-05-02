#include "include/controller.h"
#include "include/telegramBot.h"

int main(int argc, char** argv){
    QApplication a(argc, argv);

    controller* controll = new controller();
    telegramBot* bot = new telegramBot;

    QThread* threadBot = new QThread;
    QThread* threadItemReader = new QThread;

    bot->moveToThread(threadBot);
    controll->moveToThread(threadItemReader);

    QObject::connect(threadBot, &QThread::started, [bot, controll](){
        bot->setToken("пам пам пам:токен tg");
        bot->setUpdateFile("updateId");
        bot->cycleOfGetUpdates();
        bot->connectToDb("server", "123", "0.0.0.0", 5432, "rustskins");
        bot->getUpdates();
    });

    QObject::connect(threadItemReader, &QThread::started, [controll](){
        controll->connectToPgSQL("server", "123", "0.0.0.0", 5432, "rustskins");
        controll->createTable();
        controll->createTableOfItems();
        controll->createStableListOfData();
        controll->createTableListOfBotUsers();
        controll->cycleOfProgram();
    });

    threadBot->start();
    threadItemReader->start();
    
    return a.exec();
}
