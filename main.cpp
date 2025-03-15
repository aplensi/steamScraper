#include <QApplication>
#include "steamReader.h"

int main(int argc, char** argv){

    QApplication a(argc, argv);

    steamReader reader(455);

    return a.exec();
}
