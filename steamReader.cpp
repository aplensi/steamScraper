#include "steamReader.h"

steamReader::steamReader(int countOfPages) : m_countOfPages(countOfPages)
{
    openPage();
}

steamReader::steamReader(int startPage, int countOfPages) : m_countOfPages(countOfPages), m_currentPage(startPage)
{
    openPage();
}

steamReader::~steamReader()
{
    qDebug() << "destructor";
}

void steamReader::openPage()
{
    if(m_currentPage > m_countOfPages){
        return;
    }
    if(!m_isStarted){
        m_profile = new QWebEngineProfile;
        m_view = new QWebEngineView(m_profile);
        startProxy();
        connect(m_view, &QWebEngineView::loadFinished, this, &steamReader::isLoaded);
        m_view->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
        m_view->setGeometry(0, 0, 600, 600);
        m_view->load(QUrl("https://steamcommunity.com/market/search?appid=252490#p" + QString::number(m_currentPage) + "_name_asc"));
        //m_view->show();
    } else{
        m_view->load(QUrl("https://steamcommunity.com/market/search?appid=252490#p" + QString::number(m_currentPage) + "_name_asc"));
        isLoaded(true);
    }
}

void steamReader::isLoaded(bool result)
{
    if(result){
        if(!m_isStarted){
            m_view->page()->toHtml([this](QString html){
                if(html.contains("market_listing_table_header")){
                    writePage(std::to_string(m_currentPage), html);
                    qDebug() << "+++++ | load of page num " << QString::number(m_currentPage) << " is finished | +++++";
                    m_isStarted = true;
                    m_currentTry = 0;
                    m_currentPage++;
                    openPage();
                } else{
                    if(m_currentTry < 20){
                        QTimer::singleShot(250, [this](){
                            //qDebug() << "Try: " << m_currentTry;
                            m_currentTry++;
                            isLoaded(true);
                        });
                    } else{
                        qDebug() << "load of page: " << QString::number(m_currentPage) << " is failed after start";
                        m_currentTry = 0;
                        m_view->deleteLater();
                        m_profile->deleteLater();
                        openPage();
                    }
                }
            });
        } else{
            m_view->page()->toHtml([this](QString html){
                if(html.contains("\"market_paging_pagelink active\">" + QString().number(m_currentPage))){
                    writePage(std::to_string(m_currentPage), html);
                    qDebug() << "+++++ | load of page num " << QString::number(m_currentPage) << " is finished | +++++";
                    m_currentTry = 0;
                    m_currentPage++;
                    openPage();
                } else{
                    if(m_currentTry < 20){
                        QTimer::singleShot(250, [this](){
                            //qDebug() << "Try: " << m_currentTry;
                            m_currentTry++;
                            isLoaded(true);
                        });
                    } else{
                        qDebug() << "load of page: " << QString::number(m_currentPage) << " is reloaded";
                        m_currentTry = 0;
                        m_view->deleteLater();
                        m_profile->deleteLater();
                        m_isStarted = false;
                        openPage();
                    }
                }
            });
        }
    } else{
        qDebug() << "load of page: " << QString::number(m_currentPage) << " is failed after start";
        m_view->deleteLater();
        m_profile->deleteLater();
        openPage();
    }
}

void steamReader::writePage(std::string nPage, QString html)
{
    std::ofstream ofs("pages/page " + nPage + ".html");
    ofs << html.toStdString();
}

void steamReader::startProxy()
{
    QString proxyHost = "127.0.0.1";
    int proxyPort = 9050;

    QNetworkProxy networkProxy(QNetworkProxy::Socks5Proxy, proxyHost, proxyPort);
    QNetworkProxy::setApplicationProxy(networkProxy);
}
