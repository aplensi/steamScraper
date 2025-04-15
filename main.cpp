#include "include/controller.h"

int main(int argc, char** argv){
    QApplication a(argc, argv);

    controller* controll = new controller();
    controll->connectToPgSQL("ser", "1234", "0.0.0.0", 5432, "rust");
    controll->createTable();

    int countOfWidgets;
    int countOfPages;
    bool cycle;

    std::cout << "\033[2J\033[H";
    std::cout << "Enter count of widgets: ";
    std::cin >> countOfWidgets;
    std::cout << "Enter count of pages: ";
    std::cin >> countOfPages;
    std::cout << "Cycle? (0 - no, 1 - yes): ";
    std::cin >> cycle;

    if(countOfPages < 1){
        controll->loadPages(countOfWidgets, cycle);
    }else{
        controll->loadPages(countOfWidgets, countOfPages, cycle);
    }

    return a.exec();
}
