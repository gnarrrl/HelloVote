#ifndef BOTTHREAD_H
#define BOTTHREAD_H

#include <QThread>
#include <QUrl>

class QTextStream;
class QAtomicInt;

namespace HelloInternet
{

class BotThread : public QThread
{
    Q_OBJECT
public:
    BotThread(QObject* parent,
              QTextStream* stream,
              QAtomicInt* counter,
              const QUrl& url,
              int id);
    ~BotThread();

    virtual void run() override;

private:
    QAtomicInt* m_voteCounter {};
    QTextStream* m_stdout {};
    QUrl m_url;
    int m_id {};
};

}


#endif // BOTTHREAD_H
