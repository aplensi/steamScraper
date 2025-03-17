#include <QApplication>
#include "steamReader.h"

int main(int argc, char** argv){

    QApplication a(argc, argv);

    steamReader* reader;
    reader = new steamReader(1, 50);
    reader = new steamReader(51, 100);
    reader = new steamReader(101, 150);
    reader = new steamReader(151, 200);
    reader = new steamReader(201, 250);
    reader = new steamReader(251, 300);
    reader = new steamReader(301, 350);
    reader = new steamReader(351, 400);
    reader = new steamReader(401, 454);

    return a.exec();
}
