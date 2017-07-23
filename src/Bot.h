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
    Bot(QTextStream* stream,
        QAtomicInt* counter,
        const QUrl& url,
        int id);

    ~Bot();

public slots:
    void run();

protected slots:
    void sendRequest();
    void onReply();

private:
    QAtomicInt* m_voteCounter {};
    QTextStream* m_stdout {};
    QUrl m_url;
    int m_id {};
    QNetworkAccessManager* m_net {};
};

}


#endif // Bot_H
