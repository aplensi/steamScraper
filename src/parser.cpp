#include "../include/parser.h"

void parser::readFile(QString fileName)
{
    m_listOfItems.clear();
    m_file.setFileName(fileName);
    m_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString line;
    while(!m_file.atEnd()){
        line = m_file.readLine();
        parsPageOfMarketPlace(line);
    }
    m_file.close();
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
        parsPageOfMarketPlace(sLine);
        html = html.mid(html.indexOf('\n') + 1);
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

void parser::getCountOfItemsFromJson(QJsonDocument jsonDocl)
{
    QJsonObject jsonObj = jsonDocl.object();
    QJsonValue value = jsonObj.value("total_count");
    if (value.isDouble()) {
        int count = value.toInt();
        qDebug() << "Count of items:" << count;
        emit sendCountOfPages(count);
    } else {
        qDebug() << "Error: total_count is not a number.";
    }
}

void parser::readItemsFromJson(QJsonDocument jsonDoc)
{
    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray itemsArray = jsonObj.value("results").toArray();
    for (const QJsonValue& value : itemsArray) {
        QJsonObject itemObj = value.toObject();
        m_items.m_name = itemObj.value("name").toString();
        //qDebug() << "Item name:" << m_items.m_name;
        m_listOfItems.append(m_items);
    }
    qDebug() << m_listOfItems.length();
    emit sendListOfItems(m_listOfItems);
}

QVector<itemsOfPage> parser::getListOfItems()
{
    return m_listOfItems;
}

void parser::parsPageOfMarketPlace(QString line)  // Need refactoring!!!
{
    QRegularExpression regex(R"(<span[^>]*class="market_listing_item_name"[^>]*>([^<]*)</span>)");
    QRegularExpressionMatch match = regex.match(line);
    if (match.hasMatch()) {
        qDebug() << "Item name:" << match.captured(1);
    }
}