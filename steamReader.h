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
    steamReader(int startPage, int countOfPages);
    ~steamReader();
public slots:
    void isLoaded(bool result);
private:
QWebEngineProfile *m_profile;
QWebEngineView* m_view;
QNetworkProxy* m_proxy;
QTimer* m_timer;
int m_countOfPages;
int m_currentPage = 1;
int m_currentTry = 0;
bool m_isLoaded = false;
bool m_isStarted = false;
void openPage();
void writePage(std::string nPage, QString html);
void startProxy();
};

#endif