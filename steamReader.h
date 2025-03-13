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
    steamReader();
    steamReader(int numberOfPage);
    ~steamReader();
private:
QWebEngineProfile *m_profile;
QWebEngineView* m_view;
QNetworkProxy* m_proxy;
void openPage(QString str);
void getPage(QString str);
void startProxy();
};

#endif