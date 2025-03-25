#include "../include/parser.h"

void parser::readFile(QString fileName)
{
    m_file.setFileName(fileName);
    m_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString line;
    while(!m_file.atEnd()){
        line = m_file.readLine();
        QString res = parsLine(line);
        if(res != ""){
            qDebug() << res;
        }
    }
    m_file.close();
}

void parser::readBuffer(QString html)
{
    QString sLine;
    QString res;
    while(true){
        sLine = html.left(html.indexOf('\n') + 1);
        if(sLine.indexOf('\n') == -1){
            break;
        } else{
            res = parsLine(sLine);
            if(res != ""){
                qDebug() << res;
            }
        }
        html = html.mid(html.indexOf('\n') + 1);
    }
}

QString parser::parsLine(QString line)
{

    if(line.indexOf("result_") != -1 && line.indexOf("_name") != -1){
        line = line.mid(line.indexOf(">") + 1);
        line = line.left(line.indexOf("<"));
        return line;
    } else if(line.indexOf("market_listing_num_listings_qty") != -1){
        line = line.mid(line.indexOf(">") + 1);
        line = line.left(line.indexOf("<"));
        return line;
    } else if(line.indexOf("\"normal_price") != -1 || line.indexOf("sale_price") != -1){
        line = line.mid(line.indexOf(">") + 2);
        line = line.left(line.indexOf("<"));
        line = line.left(line.indexOf(" "));
        return line;
    } else {
        return "";
    }
}