#ifndef STEAMREADER_H
#define STEAMREADER_H

#include "includes.h"

class steamReader : public QObject{
    Q_OBJECT
public:
    steamReader(int m_endPage); //   from the first page
    steamReader(int startPage, int m_endPage); //    in range of pages
private slots:
    void isLoaded(bool result);
signals:
    void pushToParse(QString html);
    void readerFinished();
private:
    QWebEngineProfile *m_profile;
    QWebEnginePage* m_page;
    QNetworkProxy* m_proxy;
    QTimer* m_timer;
    QDir dir;
    int m_endPage;
    int m_currentPage = 1;
    int m_currentTry = 0;  
    bool m_isLoaded = false;
    bool m_isStarted = false;
    void openPage();
    void writePage(std::string nPage, QString html);
    void htmlStatus(bool isStarted);
    void startProxy();
};

#endif