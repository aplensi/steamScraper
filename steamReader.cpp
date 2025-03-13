#include "steamReader.h"

steamReader::steamReader()
{
    startProxy();
    openPage(QString::number(1));
}

steamReader::steamReader(int numberOfPage)
{
    startProxy();
    openPage(QString::number(numberOfPage));
}

steamReader::~steamReader()
{
    qDebug() << "destructor";
}

void steamReader::openPage(QString str)
{
    m_view = new QWebEngineView;
    m_view->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
    m_view->load(QUrl("https://steamcommunity.com/market/search?appid=252490#p" + str + "_name_asc"));
    m_view->resize(100,300);
    m_view->show();
    connect(m_view, &QWebEngineView::loadFinished, this, [this, str](bool sucess){
        if (sucess) {
            getPage(str);
        } else{
            qDebug() << "load failed!";
            m_view->deleteLater();
            QTimer::singleShot(1000, this, [this, str]() {
                openPage(str);
            });
        }
    });
}

void steamReader::getPage(QString str)
{
    m_view->page()->toHtml([this, str](const QString html){
        if(html.contains("market_listing_table_header"))
        {
            std::ofstream ofs("pages/page "+str.toStdString()+".html");
            ofs << html.toStdString();
            m_view->hide();
            m_view->deleteLater();
            qDebug() << "+++++ | load filished! | +++++";
            this->deleteLater();
        } else{
            getPage(str);
        }
    });
}

void steamReader::startProxy()
{
    m_profile = QWebEngineProfile::defaultProfile();

    m_proxy = new QNetworkProxy;
    m_proxy->setType(QNetworkProxy::Socks5Proxy);
    m_proxy->setHostName("127.0.0.1");
    m_proxy->setPort(9050);

    QNetworkProxy::setApplicationProxy(*m_proxy);
}