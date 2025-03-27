#ifndef PARSER_H
#define PARSER_H

#include "includes.h"

struct itemsOfPage{
    QString m_name;
    int m_count;
    float m_NormalPrice;
    float m_SalePrice;
    QString m_lastCheck;

    friend QDebug operator<<(QDebug debug, const itemsOfPage &items) {
        debug.nospace() << "Item(Name: "<<items.m_name<<"; Count: "<<items.m_count<<"; NormalPrice: "<<items.m_SalePrice<<
        "; SalePrice: "<<items.m_SalePrice<<"; lastCheck: "<<items.m_lastCheck<<")";
        return debug;
    }

};

class parser{

public:
    void readFile(QString fileName);
    void readBuffer(QString html);
    QVector<itemsOfPage> getListOfItems();
private:
    void parsLine(QString line);
    QString m_html;
    QFile m_file;
    QDateTime m_dateTime;
    itemsOfPage m_items;
    QVector<itemsOfPage> m_listOfItems;
};

#endif