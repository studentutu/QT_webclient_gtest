#include <QApplication>
#include <QPushButton>
#include "0_Abstract/IWindowApi.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonDocument>
#include <QtNetwork/QNetworkReply>
#include <QObject>

void sendRequest();

int main(int argc, char *argv[]) {

    auto actualWindow = new IWindowApi();

    actualWindow->CreateWindow(argc, argv);
    auto *button = new QPushButton("Hello world!", nullptr);
    QObject::connect(button, &QPushButton::clicked, sendRequest);
    button->resize(200, 100);
    button->show();


    return QApplication::exec();
}

void sendRequest() {

    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply * )), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest req(
            QUrl(QString(
                    "http://api.weatherapi.com/v1/current.json?key=102159d41c644319a60173651212512&q=London&aqi=no"))
    );
//    req.setRawHeader();
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {

        QString strReply = (QString) reply->readAll();

        //parse json
        qDebug() << "Response:" << strReply;
//        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
//
//        QJsonObject jsonObj = jsonResponse.object();
//
//        qDebug() << "username:" << jsonObj["username"].toString();
//        qDebug() << "password:" << jsonObj["password"].toString();

        delete reply;
    } else {
        //failure
        qDebug() << "Failure" << reply->errorString();
        qDebug() << "Failure" << (QString) reply->readAll();
        delete reply;
    }
}