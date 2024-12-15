#include <QTimer>
#include <QUuid>
#include "NetworkManager.h"

NetworkManager::NetworkManager(QObject* parent /*= nullptr*/) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
    manager->setAutoDeleteReplies(true);
    manager->setTransferTimeout(2500); //socket连接请求超时设置,超时为 2500 毫秒。
}

NetworkManager::~NetworkManager()
{
    if (manager!=nullptr)
        delete manager;
}

bool NetworkManager::TryGetSync(const QUrl& url,QNetworkRequest request ,QByteArray &responseData)
{
    request.setUrl(url);
    request.setRawHeader("Accept","application/json, text/javascript, */*; q=0.01");
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");    
    request.setRawHeader("Accept-Language","zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko");

    QNetworkReply *reply= manager->get(request);
    QEventLoop Eventloop;

    connect(reply, &QNetworkReply::finished, this, [this, &Eventloop,reply, &responseData](){
        if (reply->error() == QNetworkReply::NoError) {
            responseData = reply->readAll();
        } else {
             responseData ="";
        }
       sender()->deleteLater();

       Eventloop.quit();
    });

    QObject::connect(reply ,&QNetworkReply::finished, &Eventloop, &QEventLoop::quit);    
    Eventloop.exec();
    int Code= reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply->deleteLater();    
    return  (Code==200?true:false);
}

bool NetworkManager::TryPostExSync(const QUrl &url,QByteArray postBody,QByteArray &responseData )
{
    QNetworkRequest request;
    return TryPostExSync(url, request,postBody,responseData );
}

bool NetworkManager::TryPostExSync(const QUrl &url,QNetworkRequest request,QByteArray postBody,QByteArray &responseData )
{
    request.setUrl(url);
     request.setRawHeader("Accept","application/json, text/javascript, */*; q=0.01");
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");

    request.setRawHeader("Accept-Language","zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko");
    QEventLoop Eventloop;
    QNetworkReply *reply = manager->post(request, postBody);

    connect(reply, &QNetworkReply::finished, this, [this, &Eventloop,reply, &responseData](){
         if (reply->error() == QNetworkReply::NoError){
             responseData= reply->readAll();
         }
         else{
             responseData="";
         }
        sender()->deleteLater();
        Eventloop.quit();
    });
    QObject::connect(reply ,&QNetworkReply::finished, &Eventloop, &QEventLoop::quit);
    Eventloop.exec();
    int Code= reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply->deleteLater();
     return  (Code==200?true:false);
}

bool NetworkManager::TryPostExJson(const QUrl& url,QByteArray postBody,QByteArray &responseData)
{
    QNetworkRequest request;
    return NetworkManager::TryPostExJson(url, request,postBody,responseData);
}

bool NetworkManager::TryPostExJson(const QUrl& url,QNetworkRequest request,QByteArray postBody,QByteArray &responseData)
{
    request.setUrl(url);
    request.setRawHeader("Accept","application/json, text/javascript, */*; q=0.01");
    request.setRawHeader("Content-Type","application/json; charset=utf-8");
    request.setRawHeader("Accept-Language","zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko");

    QEventLoop Eventloop;

    QNetworkReply *reply = manager->post(request, postBody);

   connect(reply, &QNetworkReply::finished, this, [this, &Eventloop,reply, &responseData](){
        if (reply->error() == QNetworkReply::NoError)
            responseData= reply->readAll();
        else
            responseData="";
        sender()->deleteLater();
        Eventloop.quit();
    });
    QObject::connect(reply ,&QNetworkReply::finished, &Eventloop, &QEventLoop::quit);
    Eventloop.exec();
    int Code= reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply->deleteLater();
    return  (Code==200?true:false);
}
