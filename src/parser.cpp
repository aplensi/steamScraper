#include "../include/parser.h"

void parser::parsBotUpdate(QJsonDocument jsonDoc)
{
    int updateId = 0;
    int chatId = 0;
    QString name = "";
    QString text = "";
    QRegularExpression setIdRegex("/setid:(\\d+)");
    QRegularExpression showIncIdRegex("/showinventory:(\\d+)");
    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray itemsArray = jsonObj.value("result").toArray();
    if(!itemsArray.isEmpty()){
        for (const QJsonValue& value : itemsArray) {
            QJsonObject messageObj = value.toObject();
            QJsonObject message = messageObj.value("message").toObject();
            updateId = value.toObject().value("update_id").toInt();
            name = message.value("chat").toObject().value("username").toString();
            text = message.value("text").toString();
            chatId = message.value("chat").toObject().value("id").toInt();
            qDebug() << "chat ID: " << chatId;
            qDebug() << "Name: " << name;
            qDebug() << "Message: " << text;
            QRegularExpressionMatch match = setIdRegex.match(text);
            QRegularExpressionMatch matchShowInv = showIncIdRegex.match(text);
            if(text == "/start"){
                emit commandStart(chatId);
            }else if(text =="/commands"){
                emit commandCommand(chatId);
            }else if(match.hasMatch()){
                emit commandSetId(chatId, match.captured(1));
            }else if(text == "/getprice"){
                emit commandGetPrice(chatId);
            }else if(matchShowInv.hasMatch()){
                emit commandShowInvetory(chatId, matchShowInv.captured(1));
            }
        }
    }
    if(updateId != 0){
        emit updateIdIsSet(updateId);
    }else{
        emit emptyRequest();
    }
}

void parser::parsInventory(int chatId, QString steamId, QJsonDocument jsonDoc){
    userInventory inventory;
    userItems usItems;
    if(!jsonDoc.isNull()){
        QJsonObject jsonObj = jsonDoc.object();
        QJsonValue totalInventoryCountValue = jsonObj.value("total_inventory_count");
        QJsonArray descriptionsArray = jsonObj.value("descriptions").toArray();
        QJsonArray assetsArray = jsonObj.value("assets").toArray();
        if(totalInventoryCountValue == 0){
            emit nullCountOfItemsInventory(chatId, steamId);
        }else{
            int countOfSkins = 0;
            for (const QJsonValue& value : descriptionsArray) {
                QJsonObject itemObj = value.toObject();
                int isTredable = itemObj.value("market_tradable_restriction").toInt();
                QString classDescId = itemObj.value("classid").toString();
                if(isTredable == 7){
                    int countOfItems = 0;
                    QString classId = itemObj.value("classid").toString();
                    QString nameOfItem = itemObj.value("name").toString();
                    for(const QJsonValue& asset : assetsArray){
                        QJsonObject assetObj = asset.toObject();
                        QString classAssetId = assetObj.value("classid").toString();
                        if(classAssetId == classDescId){
                            countOfItems++;
                            countOfSkins++;
                        }
                    }
                    usItems.m_name = nameOfItem;
                    usItems.m_count = countOfItems;
                    inventory.m_listOfItems.append(usItems);
                }else{
                    continue;
                }
            }
            if(countOfSkins != 0){
                emit sendUserInventory(chatId, inventory);
                emit sendIdAndSteamId(chatId, steamId);
            }else{
                emit dontHaveItems(chatId, steamId);
            }
        }
    }else{
        emit brockenDataOfInventory(chatId, steamId);
    }
}

void parser::getCountOfItemsFromJson(QJsonDocument jsonDocl)
{
    QJsonObject jsonObj = jsonDocl.object();
    QJsonValue value = jsonObj.value("total_count");
    if (value.isDouble()) {
        int count = value.toInt();
        if(count == 0){
            emit countOfItemsIsNull();
        }else{
            m_countOfItems = count;
            emit sendCountOfPages(count);
        }
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
        }else if(m_listOfItems.length() % 500 == 0){
            qDebug() << "started new pack";
            start = m_countOfItems - (m_countOfItems - m_listOfItems.length());
            if (m_countOfItems - start > 500){
                emit startNewPack(start, 50);
            }else {
                int count = (m_countOfItems - start + 9) / 10;
                qDebug() << "Count of items less then 500" << "\nstart: " << start << " count: " << count;
                emit startNewPack(start, count);
            }
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
                m_countOfReadedItems = 0;
                emit namesAndIdsIsReceived(m_listOfItems);
            }
        }else{
            if(m_countOfItemsDB + m_countOfReadedItems == m_countOfItems){
                m_countOfReadedItems = 0;
                qDebug() << "New items are parsed.";
                qDebug() << "Count of items: " << m_listOfItems.length();
                emit namesAndIdsIsReceived(m_listOfItems);

            }
        }
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