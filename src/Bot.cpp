#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTextStream>
#include <QAtomicInt>
#include <QDateTime>
#include <QRegExp>
#include <QTimer>

#include <functional>

#include "Bot.h"

namespace HelloInternet
{

Bot::Bot(QAtomicInt *counter,
        int id,
        QString pollnumber,
        QString ourpick,
        QString cookieBaseUrl,
        QString voteBaseUrl
        )
    : QObject(nullptr)
    , m_voteCounter(counter)
    , m_id(id)
    , m_pollnumber(pollnumber)
    , m_ourpick(ourpick)
    , m_cookieBaseUrl(cookieBaseUrl)
    , m_voteBaseUrl(voteBaseUrl)
{
}

Bot::~Bot()
{
}

void Bot::run()
{
    m_net = new QNetworkAccessManager(this);
    getCookie();
}

void Bot::getCookie()
{
    const qint64 msecSinceEpoch {
        QDateTime::currentMSecsSinceEpoch()
    };

    QUrl url(QUrl::fromUserInput(
                 QString(
                     "%1/%2?%3"
                     )
                 .arg(m_cookieBaseUrl)
                 .arg(m_pollnumber)
                 .arg(msecSinceEpoch)
                 )
             );

    QNetworkRequest request(
                url
                );
    request.setRawHeader("Host",
                         "polldaddy.com"
                         );
    request.setRawHeader("Referer",
                         QUrl::fromUserInput(m_voteBaseUrl).toEncoded()
                         );
    request.setRawHeader("User-Agent",
                         "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.115 Safari/537.36"
                         );

    qDebug() << "Requesting cookie .. " << url.toEncoded();

    QNetworkReply* reply {
        m_net->get(request)
    };

    Q_ASSERT(reply);
    if (reply == nullptr) return;

    QObject::connect(
                reply,
                &QNetworkReply::finished,
                this,
                &Bot::onReplyGetCookie);
}

void Bot::vote()
{
    QUrl url(QUrl::fromUserInput(
                 QString(
                     "http://polls.polldaddy.com/vote-js.php"
                     "?p=%1"
                     "&b=0"
                     "&a=%2,"
                     "&o="
                     "&va=16"
                     "&cookie=0"
                     "&n=%3"
                     "&url=%4"
                     )
                 .arg(m_pollnumber)
                 .arg(m_ourpick)
                 .arg(m_cookie)
                 .arg(m_voteBaseUrl)
                 )
             );

    QNetworkRequest request(
                url
                );

    qDebug() << "Voting ... " << url.toEncoded();

    request.setRawHeader("Referer",
                         QUrl::fromUserInput(m_voteBaseUrl).toEncoded()
                         );
    request.setRawHeader("User-Agent",
                         "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.115 Safari/537.36"
                         );

    QNetworkReply* reply {
        m_net->get(request)
    };

    Q_ASSERT(reply);
    if (reply == nullptr) return;

    QObject::connect(
                reply,
                &QNetworkReply::finished,
                this,
                &Bot::onReplyVote);
}

void Bot::onReplyGetCookie()
{
    QNetworkReply* reply {
        qobject_cast<QNetworkReply*>(sender())
    };
    if (reply == nullptr) return;

    QString content(reply->readAll());
    reply->deleteLater();

    QRegExp rx("'(.*)'");

    QString lastCookie(m_cookie);

    int index { rx.indexIn(content) };
    if (index >= 0) {
        m_cookie = rx.cap(1);
    }

    qDebug() << "[COOKIE] " << m_cookie;

    if (m_cookie == lastCookie) {
        qDebug() << "Cookie has not changed ... delaying";
        QTimer::singleShot(
                    1000,
                    this,
                    &Bot::getCookie
                    );
    } else{
        qDebug() << "New cookie. Voting now.";
        vote();
    }
}

void Bot::onReplyVote()
{
    QNetworkReply* reply {
        qobject_cast<QNetworkReply*>(sender())
    };
    if (reply == nullptr) return;

    qDebug() << reply->readAll();
    qDebug() << "[" << m_id << "] " << ++(*m_voteCounter) << endl;
    reply->deleteLater();

    getCookie();
}
}
