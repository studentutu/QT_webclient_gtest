//
// Created by rfedor on 25.12.2021.
//
#pragma once
#ifndef QT_REST_COMMUNICATION_WIDGET_RESTMANAGER_H
#define QT_REST_COMMUNICATION_WIDGET_RESTMANAGER_H
#include <QtCore>
#include <QObject>
#include <QString>
#include <QtNetwork/QTcpSocket>
#include <QIODevice>
#include <QByteArray>
#include <QMap>
#include <QMetaObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>

class RestManager : public QObject{

Q_OBJECT
public:
    explicit RestManager(QObject *parent = 0);
    ~RestManager();


    void GET(const QString hostName);
    void POST(const QString hostName, QMap<QString, QString> data);
    void PUT(const QString hostName, QMap<QString, QString> data);
    void HEAD(const QString hostName);


signals:
    void sendSignal(QString data);

public slots:
    void handleFinished(QNetworkReply *networkReply);
    void errorOccurred(QNetworkReply::NetworkError code);
    void readyRead();

private:
    QUrlQuery constructPostData(QMap<QString, QString> data);
    QNetworkRequest* constructNetworkRequest(const QString hostName, QMap<QString, QString> headers);
    QNetworkAccessManager *networkManager;
    QMap<QString, QString> headers;
};


#endif //QT_REST_COMMUNICATION_WIDGET_RESTMANAGER_H
