#include <qt6/QtWidgets/qapplication.h>
#include "steamReader.h"

int main(int argc, char** argv){

    QApplication a(argc, argv);

    steamReader* reader;

    for(int i = 1; i < 41; i++){
        reader = new steamReader(i);
    }

    return a.exec();
}
