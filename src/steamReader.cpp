#include "../include/steamReader.h"

steamReader::steamReader(int endPage) : m_endPage(endPage)
{
    openPage();
}

steamReader::steamReader(int startPage, int endPage) : m_endPage(endPage), m_currentPage(startPage)
{
    openPage();
}

void steamReader::openPage()
{
    if(!m_isStarted){
        m_profile = new QWebEngineProfile;
        m_view = new QWebEngineView(m_profile);
        startProxy();
        connect(m_view, &QWebEngineView::loadFinished, this, &steamReader::isLoaded);
        m_view->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
        m_view->load(QUrl("https://steamcommunity.com/market/search?appid=252490#p" + QString::number(m_currentPage) + "_name_asc"));
    } else{
        m_view->load(QUrl("https://steamcommunity.com/market/search?appid=252490#p" + QString::number(m_currentPage) + "_name_asc"));
        isLoaded(true);
    }
}

void steamReader::isLoaded(bool result)
{
    if(result){
        if(!m_isStarted){
            htmlStatus(m_isStarted);
        } else{
            htmlStatus(m_isStarted);
        }
    } else{
        m_view->deleteLater();
        m_profile->deleteLater();
        openPage();
    }
}

void steamReader::writePage(std::string nPage, QString html)
{
    dir.mkdir("pages");
    std::ofstream ofs("pages/page " + nPage + ".html");
    ofs << html.toStdString();
}

void steamReader::htmlStatus(bool isStarted)
{
    QString checkLine;

    if(!isStarted){
        checkLine = "market_listing_table_header";
    } else{
        checkLine = "\"market_paging_pagelink active\">" + QString().number(m_currentPage);
    }

    m_view->page()->toHtml([this, checkLine](QString html){
        if(html.contains(checkLine)){
            writePage(std::to_string(m_currentPage), html);
            m_isStarted = true;
            m_currentTry = 0;
            qDebug() << "Page: " << QString::number(m_currentPage) << " is loaded";
            m_currentPage++;
            if(m_currentPage > m_endPage){
                delete m_view;
                delete m_profile;
                m_view = nullptr;
                m_profile = nullptr;
                emit readerFinished();
                emit pushToParse(html);
            }else{
                emit pushToParse(html);
                openPage();
            }
        } else{
            if(m_currentTry < 20){
                QTimer::singleShot(250, [this](){
                    m_currentTry++;
                    isLoaded(true);
                });
            } else{
                m_currentTry = 0;
                m_view->deleteLater();
                m_profile->deleteLater();
                m_isStarted = false;
                openPage();
            }
        }
    });
}

void steamReader::startProxy()
{
    QString proxyHost = "127.0.0.1";
    int proxyPort = 9050;

    QNetworkProxy networkProxy(QNetworkProxy::Socks5Proxy, proxyHost, proxyPort);
    QNetworkProxy::setApplicationProxy(networkProxy);
}
