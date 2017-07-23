#ifndef Bot_H
#define Bot_H

#include <QUrl>
#include <QObject>

class QTextStream;
class QAtomicInt;
class QNetworkAccessManager;

namespace HelloInternet
{

class Bot : public QObject
{
    Q_OBJECT
public:
    Bot(QAtomicInt* counter,
        int id,
        QString pollnumber,
        QString ourpick,
        QUrl cookieBaseUrl,
        QUrl voteBaseUrl);

    ~Bot();

public slots:
    void run();

protected slots:
    void vote();
    void getCookie();
    void onReplyVote();
    void onReplyGetCookie();

private:
    QAtomicInt* m_voteCounter {};
    int m_id {};
    QNetworkAccessManager* m_net {};
    QString m_cookie;
    QString m_pollnumber;
    QString m_ourpick;
    QUrl m_cookieBaseUrl;
    QUrl m_voteBaseUrl;
};

}


#endif // Bot_H
