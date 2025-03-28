#include "include/controller.h"

int main(int argc, char** argv){

    QApplication a(argc, argv);

    steamReader* reader = new steamReader(25);
    parser* pars = new parser();

    QObject::connect(reader, &steamReader::pushToParse, pars, &parser::readBuffer);

    return a.exec();
}
