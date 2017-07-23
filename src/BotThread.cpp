#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTextStream>
#include <QAtomicInt>

#include <functional>

#include "BotThread.h"

namespace HelloInternet
{

BotThread::BotThread(
        QObject* parent,
        QTextStream *stream,
        QAtomicInt *counter,
        const QUrl &url,
        int id
        )
    : QThread(parent)
    , m_stdout(stream)
    , m_voteCounter(counter)
    , m_url(url)
    , m_id(id)
{
}

BotThread::~BotThread()
{
}

void
BotThread::run()
{
    QNetworkAccessManager net;
    QNetworkRequest request(
                m_url
                );

    std::function<void()> sendRequest;
    std::function<void(QNetworkReply*)> onReply;

    sendRequest = [&] {
        QNetworkReply* reply {
            net.get(request)
        };

        Q_ASSERT(reply);
        if (reply == nullptr) return;

        QObject::connect(
                    reply,
                    &QNetworkReply::finished,
                    this,
                    std::bind(onReply,
                              reply),
                    Qt::DirectConnection
                    );
    };

    onReply = [&] (QNetworkReply* reply) {
        *m_stdout << "[" << m_id << "] " << ++(*m_voteCounter) << endl;

        reply->deleteLater();
        sendRequest();
    };

    sendRequest();

    exec();
}

}
