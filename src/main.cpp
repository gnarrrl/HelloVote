#include <QCoreApplication>
#include <QTextStream>
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

    QTextStream out(stdout);
    QAtomicInt voteCount(0);
//    QUrl votingUrl(
//                "http://polls.polldaddy.com/vote-js.php?p=9793268&b=0&a=44802685,&o=&va=16&cookie=0&n=edad4169d3|543&url=http%3A//www.radiotimes.com/news/2017-07-21/radio-times-radio-and-podcast-champion-round-4-5]"
//                );

    QCommandLineParser cp;
    QCommandLineOption optUrl("url",
                              "url",
                              "voting url"
                              );
    QCommandLineOption optBots("bots",
                               "bots",
                               "number of bots (threads) to run"
                               );

    cp.addOption(optUrl);
    cp.addOption(optBots);

    cp.process(a);

    // both arguments are required
    if (cp.isSet(optUrl) == false ||
        cp.isSet(optBots) == false)
    {
        cp.showHelp(0);
    }

    QUrl votingUrl(
                cp.value(optUrl)
                );

    // clamp number of threads to 1-100
    const int numThreads {
        std::max(1,
                 std::min(100,
                          cp.value(optBots).toInt()
                          )
                 )
    };

    // start and run specified number of bots until
    // the user aborts the program manually
    for (int n = 0; n < numThreads; n++) {
        QThread* t {
            new QThread(&a)
        };

        HelloInternet::Bot* b {
            new HelloInternet::Bot(
                    &out,
                    &voteCount,
                    votingUrl,
                    n
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
