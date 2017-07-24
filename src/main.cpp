#include <QCoreApplication>
#include <QAtomicInt>
#include <memory>
#include <QVector>
#include <QUrl>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QThread>

#include "Bot.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QAtomicInt voteCount(0);

    QCommandLineParser cp;
    QCommandLineOption optPoll(
                "poll",
                "poll",
                "poll number"
                );

    QCommandLineOption optChoice(
                "choice",
                "choice",
                "our chosen winner"
                );

    QCommandLineOption optBots(
                "bots",
                "bots",
                "number of bots (threads) to run"
                );

    QCommandLineOption optCookieBase(
                "cookieBase",
                "cookieBase",
                "cookie base url"
                );

    QCommandLineOption optVoteBase(
                "voteBase",
                "voteBase",
                "vote base url"
                );

    cp.addOption(optPoll);
    cp.addOption(optChoice);
    cp.addOption(optBots);
    cp.addOption(optCookieBase);
    cp.addOption(optVoteBase);

    cp.process(a);

    // both arguments are required
    if (cp.isSet(optPoll) == false ||
        cp.isSet(optChoice) == false ||
        //cp.isSet(optBots) == false ||
        cp.isSet(optCookieBase) == false ||
        cp.isSet(optVoteBase) == false)
    {
        cp.showHelp(0);
    }

    const int numThreads {
        1 // more than one thread seems not feasible due to silent voting limit

// clamp number of threads to 1-100
//        std::max(1,
//                 std::min(100,
//                          cp.value(optBots).toInt()
//                          )
//                 )
    };

    QUrl cookieBaseUrl(QUrl::fromUserInput(
                           cp.value(optCookieBase)
                           )
                       );

    QUrl voteBaseUrl(QUrl::fromUserInput(
                         cp.value(optVoteBase)
                         )
                     );

    // start and run specified number of bots until
    // the user aborts the program manually
    for (int n = 0; n < numThreads; n++) {
        QThread* t {
            new QThread(&a)
        };

        HelloInternet::Bot* b {
            new HelloInternet::Bot(
                        &voteCount,
                        n,
                        cp.value(optPoll),
                        cp.value(optChoice),
                        cookieBaseUrl,
                        voteBaseUrl
                    )
        };

        b->connect(t,
                   &QThread::finished,
                   b,
                   &QObject::deleteLater
                   );

        b->connect(t,
                   &QThread::started,
                   b,
                   &HelloInternet::Bot::run
                   );

        b->moveToThread(t);
        t->start();
    }

    return a.exec();
}
