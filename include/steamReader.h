#ifndef STEAMREADER_H
#define STEAMREADER_H

#include "parser.h"
#include <QWebEngineView>
#include <QNetworkProxy>
#include <QDir>
#include <QTimer>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QApplication>
#include <QWidget>
#include <QUrl>
#include <QObject>
#include <fstream>
#include <QString>
#include <iostream>

class steamReader : public QObject{
    Q_OBJECT
public:
    steamReader(int countOfPages); //   from the first page
    steamReader(int startPage, int countOfPages); //    in range of pages
    steamReader(int countOfPages, int* counterOfExec); //   from the first page with a counter
    steamReader(int startPage, int countOfPages, int* counterOfExec); //    in range of pages with a counter
    ~steamReader();
private slots:
    void isLoaded(bool result);
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