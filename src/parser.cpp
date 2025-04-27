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
    m_countOfItems = 0;
    QJsonObject jsonObj = jsonDocl.object();
    QJsonValue value = jsonObj.value("total_count");
    if (value.isDouble()) {
        int count = value.toInt();
        m_countOfItems = count;
        emit sendCountOfPages(count);
    } else {
        qDebug() << "Error: total_count is not a number.";
    }
}

void parser::readItemsFromJson(QJsonDocument jsonDoc)
{
    int countofitems = m_listOfItems.length();
    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray itemsArray = jsonObj.value("results").toArray();
    int start = jsonObj.value("start").toInt();
    int totalCount = jsonObj.value("total_count").toInt();
    for (const QJsonValue& value : itemsArray) {
        QJsonObject itemObj = value.toObject();
        m_items.m_name = itemObj.value("name").toString();
        m_items.m_id = 0;
        m_listOfItems.append(m_items);
    }
    if(totalCount == 0){
        qDebug() << "Error: total_count is 0.";
        emit brockenRequest(start);
    }else{
        qDebug() << m_listOfItems.length();
        if(m_countOfItems == m_listOfItems.length()){
            qDebug() << "All items are parsed.";
            emit namesIsFilled(m_listOfItems);   
        }
    }
}

void parser::parsPageOfItem(QString html, QString nameOfItem)
{
    QRegularExpression regex(R"(.*Market_LoadOrderSpread\(\s*(\d+)\s*\).*;)");
    QRegularExpressionMatch match = regex.match(html);

    if (match.hasMatch()) {
        int capturedNumber = match.captured(1).toInt();
        for(auto& i : m_listOfItems){
            if(i.m_name == nameOfItem){
                i.m_id = capturedNumber;
                m_countOfReadedItems++;
                m_finishedThreads++;
                qDebug() << "Item name:" << i.m_name << " || ID of item:" << i.m_id;
            }
        }
        if(m_finishedThreads == 100){
            QVector<itemsOfPage> newList;
            for(auto& i : m_listOfItems){
                if(i.m_id == 0){
                    newList.append(i);
                }
            }
            m_finishedThreads = 0;
            qDebug() << "count of items in heap:" << newList.length();
            emit heapIsFinished(newList);
        }

        if(m_countOfItemsDB == 0){
            if(m_countOfReadedItems == m_countOfItems){
                qDebug() << "All items are parsed.";
                emit namesAndIdsIsReceived(m_listOfItems);
            }
        }else{
            if(m_countOfItemsDB + m_countOfReadedItems == m_countOfItems){
                qDebug() << "New items are parsed.";
                qDebug() << "Count of items: " << m_listOfItems.length();
                emit namesAndIdsIsReceived(m_listOfItems);
            }
        }
        qDebug() << "Count of items:" << m_countOfItemsDB + m_countOfReadedItems << " from: " << m_countOfItems;
    }else {
        emit brockenPageOfItem(nameOfItem);
    }
}

void parser::parsDataOfItem(QJsonDocument jsonDoc, int id)
{
    QJsonObject jsonObj = jsonDoc.object();
    m_item.m_id = id;
    m_item.m_countOfSale = jsonObj.value("sell_order_count").toString().remove(',').toInt();
    m_item.m_salePrice = jsonObj.value("sell_order_price").toString().remove('$').remove(",").toDouble();
    m_item.m_countOfPurchase = jsonObj.value("buy_order_count").toString().remove(',').toInt();
    m_item.m_purchasePrice = jsonObj.value("buy_order_price").toString().remove('$').remove(",").toDouble();
    m_listOfDataOfItem.append(m_item);

    qDebug() << "ID: " << m_item.m_id << " || Sale price: " << m_item.m_salePrice << " || Purchase price: " << m_item.m_purchasePrice;
    qDebug() << "Count of sale: " << m_item.m_countOfSale << " || Count of purchase: " << m_item.m_countOfPurchase << "\n";

    if(m_listOfDataOfItem.length() == m_countOfItemsDB){
        qDebug() << "All items are parsed.";
        qDebug() << "Count of items: " << m_listOfDataOfItem.length();
        emit gettingDataIsOvered(m_listOfDataOfItem);
        m_listOfDataOfItem.clear();
    }else if(m_listOfDataOfItem.length() % 200 == 0){
        QVector<itemsOfPage> newList = m_listOfItemsDB;
        int length = m_countOfItems - (m_countOfItems - m_listOfDataOfItem.length());
        newList.remove(0, length);
        emit dataOfItemIsReceived(newList);
    }
}

void parser::setListOfItemsDB(QVector<itemsOfPage> listOfItems)
{
    m_listOfItemsDB = listOfItems;
}

void parser::setCountOfDBItems(int count)
{
    m_countOfItemsDB = count;
}

QVector<itemsOfPage> parser::getListOfItems()
{
    return m_listOfItems;
}

void parser::parsPageOfMarketPlace(QString line)
{
    QRegularExpression regex(R"(<span[^>]*class="market_listing_item_name"[^>]*>([^<]*)</span>)");
    QRegularExpressionMatch match = regex.match(line);
    if (match.hasMatch()) {
        qDebug() << "Item name:" << match.captured(1);
    }
}