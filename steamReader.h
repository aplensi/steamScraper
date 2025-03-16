#ifndef STEAMREADER_H
#define STEAMREADER_H

#include <QWebEngineView>
#include <QNetworkProxy>
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
    steamReader(int countOfPages);
    ~steamReader();
private:
QWebEngineProfile *m_profile;
QWebEngineCookieStore *m_cookies;
QWebEngineView* m_view;
QNetworkProxy* m_proxy;
QTimer* m_timer;
int m_countOfPages;
int m_curentPage = 1;
double m_tryCount = 0;
void openPage(QString str);
void openFewPages();
void getPage(QString str);
void startProxy();
};

#endif