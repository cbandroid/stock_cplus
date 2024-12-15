#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <functional>
#include <QEventLoop>


class NetworkManager : public QObject {
    Q_OBJECT

public:
    using ErrorCallback = std::function<void(const QString&)>;
    using ReplyCallback = std::function<void(QNetworkReply *)>;

    NetworkManager(QObject* parent = nullptr);
    ~NetworkManager();

    bool TryGetSync(const QUrl& url,QNetworkRequest request,QByteArray &responseData );
    bool TryPostExSync(const QUrl& url,QByteArray postBody,QByteArray &responseData);
    bool TryPostExSync(const QUrl& url,QNetworkRequest request ,QByteArray postBody,QByteArray &responseData);
    bool TryPostExJson(const QUrl& url,QByteArray postBody,QByteArray &responseData);
    bool TryPostExJson(const QUrl& url,QNetworkRequest request ,QByteArray postBody,QByteArray &responseData);


    template <typename T>
    void getAsync(const QUrl& url, std::function<T(const QByteArray&)> parser, std::function<void(const T&)> callback, ErrorCallback errorCallback = nullptr) {
        QNetworkRequest request(url);
        request.setRawHeader("Accept","application/json, text/javascript, */*; q=0.01");
        request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
        request.setRawHeader("Accept-Language","zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
        request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko");

        QNetworkReply* reply = manager->get(request);

        connect(reply, &QNetworkReply::finished, this, [this, reply, parser, callback, errorCallback]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                T result = parser(response);
                callback(result);
            } else {
                if (errorCallback) {
                    errorCallback(reply->errorString());
                } else {
                    qWarning() << "Network error:" << reply->errorString();
                }
            }
            reply->deleteLater();
        });
    }

    template <typename T>
    void getAsync(const QUrl& url, std::function<void(const T&)> callback, ErrorCallback errorCallback = nullptr) {
        QNetworkRequest request(url);
        request.setRawHeader("Accept","application/json, text/javascript, */*; q=0.01");
        request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
         request.setRawHeader("Accept-Language","zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
        request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko");

        QNetworkReply* reply = manager->get(request);

        connect(reply, &QNetworkReply::finished, this, [this, reply,  callback, errorCallback]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                 callback( response);              
            } else {
                if (errorCallback) {
                    errorCallback(reply->errorString());
                } else {
                    qWarning() << "Network error:" << reply->errorString();
                }
            }
            reply->deleteLater();
        });
    }

    template <typename T>
    void getAsync(const QUrl& url, ErrorCallback errorCallback = nullptr) {
        QNetworkRequest request(url);
        request.setRawHeader("Accept","application/json, text/javascript, */*; q=0.01");
        request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
          request.setRawHeader("Accept-Language","zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
        request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko");

        QNetworkReply* reply = manager->get(request);

        connect(reply, &QNetworkReply::finished, this, [this, reply,  errorCallback]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                T result =response ;
            } else {
                if (errorCallback) {
                    errorCallback(reply->errorString());
                } else {
                    qWarning() << "Network error:" << reply->errorString();
                }
            }
            reply->deleteLater();
        });
    }

    template <typename T>
    T getSync(const QUrl& url, std::function<T(const QByteArray&)> parser, ErrorCallback errorCallback = nullptr) {
        QEventLoop Eventloop;
        T result;

        getAsync<T>(url, parser, [&](const T& data) {
            result = data;
            Eventloop.quit();
        }, [&](const QString& error) {
                        if (errorCallback) {
                            errorCallback(error);
                        }
                        Eventloop.quit();
                    });

        Eventloop.exec();
        return result;
    }

    template <typename T>
    T getSync(const QUrl& url, ErrorCallback errorCallback = nullptr) {
        QEventLoop Eventloop;
        T result;

        getAsync<T>(url, [&](const T& data) {
            result = data;
            Eventloop.quit();
        }, [&](const QString& error) {
                        if (errorCallback) {
                            errorCallback(error);
                        }
                       Eventloop.quit();
                    });

        Eventloop.exec();
        return result;
    }


    template <typename T>
    void getAAsync(const QUrl& url,QNetworkRequest request ) {
        request.setUrl(url);
        QNetworkReply* reply = manager->get(request);

        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            T result;
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                result =response ;
            } else {
                qWarning() << "Network error:" << reply->errorString();
            }
            reply->deleteLater();
        });
    }

    template <typename T>
    T getSyncEx(const QUrl& url,QNetworkRequest request) {
        QEventLoop loop;
        T result;

        getAAsync<T>(url,request,[&](const T& data) {
            result = data;
            loop.quit();
        });

        loop.exec();
        return result;
    }

private:
    QNetworkAccessManager* manager;

public:
    QNetworkReply* pReply;
};

#endif // NETWORKMANAGER_H
