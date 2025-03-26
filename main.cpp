#include "include/steamReader.h"
#include "include/parser.h"

int main(int argc, char** argv){

    QApplication a(argc, argv);

    int i = 0;
    steamReader* reader;
    reader = new steamReader(1, 25, &i);
    reader = new steamReader(26, 50, &i);
    reader = new steamReader(51, 75, &i);
    reader = new steamReader(76, 100, &i);

    QThread* thread = QThread::create([&i](){
        int j = i;
        while(true){
            if(i != j){
                qDebug() << "==> " << i;
                j = i;
            }
        }
    });

    thread->start();

    // parser pars;
    // pars.readFile("pages/page 1.html");

    return a.exec();
}
