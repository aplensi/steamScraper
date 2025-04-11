#include "../include/parser.h"

void parser::readFile(QString fileName)
{
    m_listOfItems.clear();
    m_file.setFileName(fileName);
    m_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString line;
    while(!m_file.atEnd()){
        line = m_file.readLine();
        parsLine(line);
    }
    m_file.close();
    for(auto i : m_listOfItems){
        qDebug() << i;
    }
}

void parser::readBuffer(QString html)
{
    m_listOfItems.clear();
    QString sLine;
    while(true){
        sLine = html.left(html.indexOf('\n') + 1);
        if(sLine.indexOf('\n') == -1){
            break;
        } 
        parsLine(sLine);
        html = html.mid(html.indexOf('\n') + 1);
    }
    for(auto i : m_listOfItems){
        //qDebug() << i;
    }
    emit sendListOfItems(m_listOfItems);
}

void parser::getCountOfPagesFromBuffer(QString html)
{
    int count = 0;
    QString sLine;
    QRegularExpression regex(R"(<span class="market_paging_pagelink">\s*(\d+)\s*</span>)");
    QRegularExpressionMatchIterator it = regex.globalMatch(html);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        count = match.captured(1).toInt();
    }
    emit sendCountOfPages(count);
}

QVector<itemsOfPage> parser::getListOfItems()
{
    return m_listOfItems;
}

void parser::parsLine(QString line)  // Need refactoring!!!
{
    if(line.indexOf("result_") != -1 && line.indexOf("_name") != -1){
        line = line.mid(line.indexOf(">") + 1);
        line = line.left(line.indexOf("<"));
        m_items.m_name = line;
        m_items.m_lastCheck = m_dateTime.currentDateTime().toString();
        m_listOfItems.append(m_items);
    } else if(line.indexOf("market_listing_num_listings_qty") != -1){
        line = line.mid(line.indexOf(">") + 1);
        line = line.left(line.indexOf("<"));
        m_items.m_count = line.toInt();
    } else if(line.indexOf("\"normal_price") != -1 || line.indexOf("sale_price") != -1){
        bool normal = false;
        if(line.indexOf("\"normal_price") != -1){
            normal = true;
        }
        line = line.mid(line.indexOf(">") + 2);
        line = line.left(line.indexOf("<"));
        line = line.left(line.indexOf(" "));
        if(normal == true){
            m_items.m_NormalPrice = line.toFloat();
        } else{
            m_items.m_SalePrice = line.toFloat();
        }
    }
}