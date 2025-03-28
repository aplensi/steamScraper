#ifndef STEAMREADER_H
#define STEAMREADER_H

#include "includes.h"

class steamReader : public QObject{
    Q_OBJECT
public:
    steamReader(int countOfPages); //   from the first page
    steamReader(int startPage, int countOfPages); //    in range of pages
    steamReader(int countOfPages, int* counterOfExec); //   from the first page with a counter
    steamReader(int startPage, int countOfPages, int* counterOfExec); //    in range of pages with a counter
private slots:
    void isLoaded(bool result);
signals:
    void pushToParse(QString html);
private:
    QWebEngineProfile *m_profile;
    QWebEngineView* m_view;
    QNetworkProxy* m_proxy;
    QTimer* m_timer;
    QDir dir;
    int m_countOfPages;
    int m_currentPage = 1;
    int m_currentTry = 0;
    int* m_counterOfExec;   
    bool m_isLoaded = false;
    bool m_isStarted = false;
    void openPage();
    void writePage(std::string nPage, QString html);
    void htmlStatus(bool isStarted);
    void startProxy();
};

#endif