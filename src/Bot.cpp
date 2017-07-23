#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTextStream>
#include <QAtomicInt>

#include <functional>

#include "Bot.h"

namespace HelloInternet
{

Bot::Bot(
        QTextStream *stream,
        QAtomicInt *counter,
        const QUrl &url,
        int id
        )
    : QObject(nullptr)
    , m_stdout(stream)
    , m_voteCounter(counter)
    , m_url(url)
    , m_id(id)
{
}

Bot::~Bot()
{
}

void
Bot::run()
{
    m_net = new QNetworkAccessManager(this);
    sendRequest();
}

void Bot::sendRequest()
{
    QNetworkRequest request(
                m_url
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
                &Bot::onReply);
}

void Bot::onReply()
{
    QNetworkReply* reply {
        qobject_cast<QNetworkReply*>(sender())
    };
    if (reply == nullptr) return;

    *m_stdout << "[" << m_id << "] " << ++(*m_voteCounter) << endl;
    reply->deleteLater();

    sendRequest();
}
}
