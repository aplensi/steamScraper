#include <QApplication>
#include "include/steamReader.h"
#include "include/parser.h"

int main(int argc, char** argv){

    QApplication a(argc, argv);

    steamReader* reader;
    reader = new steamReader(1, 50);
    reader = new steamReader(26, 50);
    reader = new steamReader(51, 75);
    reader = new steamReader(76, 100);


    // parser pars;
    // pars.readFile("pages/page 1.html");

    return a.exec();
}
