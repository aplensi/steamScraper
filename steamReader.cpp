#include "steamReader.h"

steamReader::steamReader(int countOfPages) : m_countOfPages(countOfPages)
{
    openPage(QString::number(m_curentPage));
}

steamReader::~steamReader()
{
    qDebug() << "destructor";
}

void steamReader::openPage(QString str)
{
    startProxy();
    m_profile = new QWebEngineProfile;
    m_view = new QWebEngineView(m_profile);
    m_view->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
    m_view->load(QUrl("https://steamcommunity.com/market/search?appid=252490#p" + str + "_name_asc"));
    m_view->setGeometry(0, 0, 300, 600);
    m_view->show();

    connect(m_view, &QWebEngineView::loadFinished, this, [this, str](bool sucess){
        if (sucess) {
            getPage(str);
        } else{
            qDebug() << "load failed! " << str;
            QTimer::singleShot(1000, this, [this, str]() {
                m_view->close();
                delete m_view;
                delete m_profile;
                openPage(str);
            });
        }
    });
}

void steamReader::getPage(QString str)
{
    m_timer = new QTimer;
    m_timer->setSingleShot(true);
    
    m_view->page()->toHtml([this, str](const QString html){
        if(html.contains("market_listing_table_header"))
        {
            std::ofstream ofs("pages/page "+str.toStdString()+".html");
            ofs << html.toStdString();
            qDebug() << "+++++ | load of page num " << str << " is finished | +++++";
            m_view->close();
            delete m_view;
            delete m_profile;
            m_curentPage++;
            if(m_curentPage < m_countOfPages)
            {
                m_tryCount = 0;
                openPage(QString::number(m_curentPage));
            } else {
                delete this;
            }
        } else{
            QTimer::singleShot(250, this, [this, str]() {
                if(m_tryCount < 3)
                {
                    qDebug() << "try: " << m_tryCount;
                    m_tryCount += 0.25;
                    getPage(str);
                } else{
                    m_tryCount = 0;
                    m_view->close();
                    delete m_view;
                    delete m_profile;
                    openPage(QString::number(m_curentPage));
                }
            });
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
