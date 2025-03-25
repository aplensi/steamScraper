#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QSqlDatabase>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <vector>
#include <QFile>
#include <QTextStream>

struct itemsOfPage{
    QString m_name;
    int m_count;
    float m_NormalPrice;
    float m_SalePrice;
    QString m_lastCheck;
};

class parser{

public:
    void readFile(QString fileName);
    void readBuffer(QString html);
    void writeToSql(itemsOfPage Items);
    void writeToSql(std::vector<itemsOfPage>* m_listOfItems);
    void connectToPgSQL();

private:
    QString parsLine(QString line);
    QString m_html;
    QFile m_file;
    QDateTime m_dateTime;
    std::vector<itemsOfPage>* m_listOfItems;
};

#endif