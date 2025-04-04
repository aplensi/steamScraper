#include "include/controller.h"

int main(int argc, char** argv){

    QApplication a(argc, argv);

    steamReader* reader = new steamReader(25);
    parser* pars = new parser();
    controller* cont = new controller();
    
    cont->connectToPgSQL("postgres", "123", "0.0.0.0", 5432, "rustskins");
    cont->createTable();

    QObject::connect(reader, &steamReader::pushToParse, pars, &parser::readBuffer);
    QObject::connect(pars, &parser::sendListOfItems, cont, &controller::pushToPgSQL);

    return a.exec();
}
