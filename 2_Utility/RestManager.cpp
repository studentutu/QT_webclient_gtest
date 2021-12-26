//
// Created by rfedor on 25.12.2021.
//
#pragma once

#include "RestManager.h"


///
/// RequestManager constructor
///
/// Description: sets up a network access manager that
///              abstract the HTTP/TCP protocol
RestManager::RestManager(QObject *parent) : QObject(parent) {
    // create network manager
    this->networkManager = new QNetworkAccessManager(this);
    this->networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
    connect(this->networkManager, SIGNAL(finished(QNetworkReply * )), this, SLOT(handleFinished(QNetworkReply * )));

    // set HTTP headers
    headers["User-Agent"] = "RequestManager 1.0";
}

/// Destructor
/// \brief RequestManager::~RequestManager
///
RestManager::~RestManager() {
    delete this->networkManager;
}

///
/// \brief RequestManager::HEAD
/// \param hostName
///
void RestManager::HEAD(const QString hostName) {
    // step 1: create http request with custom headers
    QNetworkRequest *request = this->constructNetworkRequest(hostName, this->headers);

    // step 2: HEAD to this resource
    this->networkManager->head(*request);
}

///
/// \brief RequestManager::PUT
/// \param hostName
///
void RestManager::PUT(const QString hostName, QMap<QString, QString> data) {
    // step 1: create http request with custom headers
    QNetworkRequest *request = this->constructNetworkRequest(hostName, this->headers);

    // step 2: get PUT data
    QUrlQuery putData = this->constructPostData(data);

    // step 3: PUT to this resource
    this->networkManager->put(*request, putData.toString(QUrl::FullyEncoded).toUtf8());
}

/// Create a HTTP POST request and setup signals/slots
/// \brief RequestManager::POST
/// \param hostName
/// \param data
///
void RestManager::POST(const QString hostName, QMap<QString, QString> data) {
    // step 1: create http request with custom headers
    QNetworkRequest *request = this->constructNetworkRequest(hostName, this->headers);

    // step 2: get POST data
    QUrlQuery postData = this->constructPostData(data);

    // step 3: POST to this resource
    this->networkManager->post(*request, postData.toString(QUrl::FullyEncoded).toUtf8());
}

/// Create a HTTP GET request and setup signals/slots
/// \brief RequestManager::GET
/// \param hostName
///
void RestManager::GET(const QString hostName) {
    // step 1: create http request with custom User-Agent headers
    QNetworkRequest *request = this->constructNetworkRequest(hostName, this->headers);

    // step 2: send http request
    QNetworkReply *reply = this->networkManager->get(*request);
    //reply->deleteLater();
    connect(reply, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void RestManager::readyRead() {
    qDebug() << "readyRead()";
}

/*
 *
 * SIGNALS/SLOTS
 *
 */
/// HTTP network request has finished
/// \brief RequestManager::handleFinished
/// \param networkReply
///
void RestManager::handleFinished(QNetworkReply *networkReply) {
    // free later
    networkReply->deleteLater();

    // no error in request
    if (networkReply->error() == QNetworkReply::NoError) {
        // get HTTP status code
        qint32 httpStatusCode = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // 200
        if (httpStatusCode >= 200 && httpStatusCode < 300) // OK
        {
            this->sendSignal(networkReply->readAll());
        } else if (httpStatusCode >= 300 && httpStatusCode < 400) // 300 Redirect
        {
            // Get new url, can be relative
            QUrl relativeUrl = networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

            // url can be relative, we use the previous url to resolve it
            QUrl redirectUrl = networkReply->url().resolved(relativeUrl);

            // redirect to new url
            networkReply->manager()->get(QNetworkRequest(redirectUrl));

            // maintain manager
            return;
        } else if (httpStatusCode >= 400 && httpStatusCode < 500) // 400 Error
        {
            qDebug() << httpStatusCode << " Error!";
        } else if (httpStatusCode >= 500 && httpStatusCode < 600) // 500 Internal Server Error
        {
            qDebug() << httpStatusCode << " Error!";
        } else {
            qDebug() << "Status code invalid! " << httpStatusCode;
        }
    } else {
        qDebug() << "errorString: " << networkReply->errorString();
    }

    networkReply->manager()->deleteLater();
}

/// Error in HTTP request
/// \brief RequestManager::onError
/// \param code
///
void RestManager::errorOccurred(QNetworkReply::NetworkError code) {
    qDebug() << "onError: " << code;
}

/*
 *
 *  HELPERS
 *
 */

/// Create correct POST data
/// \brief RequestManager::constructPostData
/// \param data
/// \return
///
QUrlQuery RestManager::constructPostData(QMap<QString, QString> data) {
    // Create POST/PUT data
    QUrlQuery postData;
    QMapIterator<QString, QString> iterator(data);

    // add all keys from map
    while (iterator.hasNext()) {
        iterator.next();
        postData.addQueryItem(iterator.key(), iterator.value());
    }
    return postData;
}

/// Create network request
/// \brief RequestManager::constructNetworkRequest
/// \param hostName
/// \param headers
/// \return
///
QNetworkRequest *RestManager::constructNetworkRequest(const QString hostName, QMap<QString, QString> headers) {
    // create HTTP request and set hostname
    QNetworkRequest *request = new QNetworkRequest();
    request->setUrl(QUrl(hostName));

    // setup error handling
    QObject::connect(this, SIGNAL(errorOccurred), SLOT(errorOccurred));

//    QObject::connect(&request, SIGNAL(errorOccurred(QNetworkReply::NetworkError)), this,
//                     SLOT(errorOccurred(QNetworkReply::NetworkError)));

    // add headers

    if (!headers.isEmpty()) {
        QMapIterator<QString, QString> iterator(headers);
        while (iterator.hasNext()) {
            iterator.next();
            request->setRawHeader(QByteArray::fromStdString(iterator.key().toStdString()),
                                  QByteArray::fromStdString(iterator.value().toStdString()));
        }
    }

    return request;
}