
#include "threadtimesharechart.h"

ThreadTimeShareChart::ThreadTimeShareChart(QObject *parent)
    : QObject{parent}
{
    // request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36");
}


ThreadTimeShareChart::~ThreadTimeShareChart()
{
  //  QCoreApplication::processEvents();

  //  if (naManager !=nullptr)
  //      delete naManager;
}

void ThreadTimeShareChart::getSSEData()
{
    GlobalVar::mTimeShareChartList.clear();
    QString url="https://push2his.eastmoney.com/api/qt/stock/trends2/sse?mpi=2000&fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58&ut=fa5fd1943c7b386f172d6893dbfba10b&iscr=0&ndays=1&secid="+GlobalVar::getComCode()+"&_=1666401553893";
    QByteArray* qByteArray=new QByteArray();
    QNetworkRequest request;
    //QNetworkAccessManager *naManager =new QNetworkAccessManager(this);
    QNetworkAccessManager *naManager =new QNetworkAccessManager(); //fixed 2024.10.8
   //  auto netManager = new QNetworkAccessManager(this);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36");
    request.setUrl(QUrl(url));
    reply= naManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=](){
       // qDebug()<<"结束"<<preGCode<<GlobalVar::curCode;
        if (preGCode==GlobalVar::curCode)
        {
            /*disconnect(reply);
            reply->deleteLater();
            delete qByteArray;
            naManager->deleteLater();
            delete naManager;
             */
            reply->disconnect();
            reply->close();
            delete qByteArray;
            naManager->deleteLater();
            delete naManager;
            getSSEData();
         //   preGCode="";
        }
        else
        {
            preGCode="";
/*
            disconnect(reply);
            reply->deleteLater();
            delete qByteArray;
            naManager->deleteLater();
            delete naManager;*/

            reply->disconnect();
            reply->close();
            delete qByteArray;
            naManager->deleteLater();
            delete naManager;
        }
    });
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // qDebug()<<reply->error();
        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray tempData=reply->readAll();
            // qDebug()<<tempData;
            if (tempData.contains("data:"))
            {
                if (tempData.contains("\"data\":{\""))
                {
                    if (tempData.mid(tempData.size()-2,2)=="\n\n")
                    {
                        allData=tempData;
                        initSSETimeShareChartList();
                        emit getTimeShareChartFinished();
                    }
                    else
                        qByteArray->append(tempData);
                }
            }
            else
            {
                qByteArray->append(tempData);
                QByteArray tempByteArray=qByteArray->data();
                if (tempByteArray.mid(tempByteArray.size()-2,2)=="\n\n")
                {
                    allData=tempByteArray;
                    initSSETimeShareChartList();
                    emit getTimeShareChartFinished();
                    qByteArray->clear();
                }
            }
        }

    });
}

void ThreadTimeShareChart::getAllTimeShareChart(bool r)
{
    QString s=GlobalVar::curCode.left(3);
    QList<QString> list;
    list<<"100"<<"122"<<"133"<<"103"<<"104";
    if (not list.contains(s))
    {
        GlobalVar::getData(allData,2,QUrl("https://push2his.eastmoney.com/api/qt/stock/trends2/get?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58&ut=fa5fd1943c7b386f172d6893dbfba10b&iscr=0&ndays=1&secid="+GlobalVar::getComCode()+"&_=1666401553893"));
        if (GlobalVar::timeOutFlag[6])
            GlobalVar::timeOutFlag[6]=false;
        else
		        {
            initTimeShareChartList();
            emit getTimeShareChartFinished();
            if (list.contains(preGCode.left(3)))
                reply->abort();
        }
    }
    else if (preGCode!=GlobalVar::curCode or r)
    // if (preGCode!=GlobalVar::curCode or r)
    {
        if (preGCode!=""){
          // reply->abort();
             reply->disconnect();
           reply->close();
           reply->deleteLater();
        }
           
            preGCode=GlobalVar::curCode;
            GlobalVar::timeShareHighLowPoint[0]=0.0;
            GlobalVar::timeShareHighLowPoint[1]=10000000.0;
            getSSEData();
        }
}

void ThreadTimeShareChart::initTimeShareChartList()
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        GlobalVar::preClose=jsonObject.value("data").toObject().value("preClose").toDouble();
        int ph=110;
        int pl=90;
        if (GlobalVar::curCode.left(1)=="3" or GlobalVar::curCode.left(1)=="8" or GlobalVar::curCode.left(3)=="688")
        {
            ph=120;
            pl=80;
        }
        float a=int(GlobalVar::preClose*ph+0.5)/100.0;
        float b=int(GlobalVar::preClose*pl+0.5)/100.0;
        GlobalVar::timeShareHighLowPoint[3]=per(a);
        GlobalVar::timeShareHighLowPoint[4]=per(b);
        QJsonArray data=jsonObject.value("data").toObject().value("trends").toArray();
        GlobalVar::trendsTotal=jsonObject.value("data").toObject().value("trendsTotal").toInt();
        GlobalVar::timeShareHighLowPoint[0]=0.0;
        GlobalVar::timeShareHighLowPoint[1]=10000000.0;
        GlobalVar::timeShareHighLowPoint[2]=0.0;
        float pp=0;
        QList<timeShareChartInfo> mTimeShareChartList;
        timeShareChartInfo info;
        QStringList list;
        float h;
        float l;
        if (GlobalVar::curCode.left(2)=="1." or GlobalVar::curCode.left(3)=="399")
            for (int i = 0; i < data.size(); ++i)
            {
                list=data.at(i).toString().split(",");
                info.time=list[0];
                info.price=per(list[2].toFloat());
                info.vol=list[5].toFloat();
                info.avePrice=per(list[7].toFloat());
                if (pp<info.price)
                    info.direct=2;
                else if (pp>info.price)
                    info.direct=1;
                else
                    info.direct=3;
                pp=info.price;
                h=per(list[3].toFloat());
                l=per(list[4].toFloat());
                if (h>GlobalVar::timeShareHighLowPoint[0])
                    GlobalVar::timeShareHighLowPoint[0]=h;
                if (l<GlobalVar::timeShareHighLowPoint[1])
                    GlobalVar::timeShareHighLowPoint[1]=l;
                if (info.avePrice>GlobalVar::timeShareHighLowPoint[0])
                    GlobalVar::timeShareHighLowPoint[0]=info.avePrice;
                if (info.avePrice<GlobalVar::timeShareHighLowPoint[1])
                    GlobalVar::timeShareHighLowPoint[1]=info.avePrice;
                if (info.vol>GlobalVar::timeShareHighLowPoint[2])
                    GlobalVar::timeShareHighLowPoint[2]=info.vol;
                mTimeShareChartList.append(info);
            }
        else
            for (int i = 0; i < data.size(); ++i)
            {
                list=data.at(i).toString().split(",");
                info.time=list[0];
                info.price=per(list[2].toFloat());
                info.vol=list[5].toFloat();
                info.avePrice=per(list[7].toFloat());
                if (pp<info.price)
                    info.direct=2;
                else if (pp>info.price)
                    info.direct=1;
                else
                    info.direct=3;
                pp=info.price;
                h=per(list[3].toFloat());
                l=per(list[4].toFloat());
                if (h>GlobalVar::timeShareHighLowPoint[0])
                    GlobalVar::timeShareHighLowPoint[0]=h;
                if (l<GlobalVar::timeShareHighLowPoint[1])
                    GlobalVar::timeShareHighLowPoint[1]=l;
                if (info.vol>GlobalVar::timeShareHighLowPoint[2])
                    GlobalVar::timeShareHighLowPoint[2]=info.vol;
                mTimeShareChartList.append(info);
            }
        if (GlobalVar::timeShareHighLowPoint[0]<0)
            GlobalVar::timeShareHighLowPoint[0]=0;
        if (GlobalVar::timeShareHighLowPoint[1]>0)
            GlobalVar::timeShareHighLowPoint[1]=0;
        GlobalVar::mTimeShareChartList=mTimeShareChartList;
    }
}

void ThreadTimeShareChart::initSSETimeShareChartList()
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData.mid(6,allData.size()-6), &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("trends").toArray();
        // qDebug()<<data;
        timeShareChartInfo info;
        QStringList list;
        float h;
        float l;
        if (GlobalVar::mTimeShareChartList.isEmpty())
        {
            float p=jsonObject.value("data").toObject().value("preClose").toDouble();
            if (p!=0)
                GlobalVar::preClose=jsonObject.value("data").toObject().value("preClose").toDouble();
            int ph=110;
            int pl=90;
            if (GlobalVar::curCode.left(1)=="3" or GlobalVar::curCode.left(1)=="8" or GlobalVar::curCode.left(3)=="688")
            {
                ph=120;
                pl=80;
            }
            GlobalVar::timeShareHighLowPoint[3]=per(int(GlobalVar::preClose*ph+0.5)/100.0);
            GlobalVar::timeShareHighLowPoint[4]=per(int(GlobalVar::preClose*pl+0.5)/100.0);
            GlobalVar::timeShareHighLowPoint[2]=0;
            pp=0;
            int t=jsonObject.value("data").toObject().value("trendsTotal").toInt();
            if (t!=0)
                GlobalVar::trendsTotal=jsonObject.value("data").toObject().value("trendsTotal").toInt();
            if (GlobalVar::curCode.left(2)=="1." or GlobalVar::curCode.left(3)=="399")
            {
                for (int i = 0; i < data.size(); ++i)
                {
                    list=data.at(i).toString().split(",");
                    info.time=list[0];
                    info.price=per(list[2].toFloat());
                    info.vol=list[5].toFloat();
                    info.avePrice=per(list[7].toFloat());
                    h=per(list[3].toFloat());
                    l=per(list[4].toFloat());
                    if (h>GlobalVar::timeShareHighLowPoint[0])
                        GlobalVar::timeShareHighLowPoint[0]=h;
                    if (l<GlobalVar::timeShareHighLowPoint[1])
                        GlobalVar::timeShareHighLowPoint[1]=l;
                    if (info.avePrice>GlobalVar::timeShareHighLowPoint[0])
                        GlobalVar::timeShareHighLowPoint[0]=info.avePrice;
                    if (info.avePrice<GlobalVar::timeShareHighLowPoint[1])
                        GlobalVar::timeShareHighLowPoint[1]=info.avePrice;
                    if (info.vol>GlobalVar::timeShareHighLowPoint[2])
                        GlobalVar::timeShareHighLowPoint[2]=info.vol;
                    if (pp<info.price)
                        info.direct=2;
                    else if (pp>info.price)
                        info.direct=1;
                    else
                        info.direct=3;
                    pp=info.price;
                    GlobalVar::mTimeShareChartList.append(info);
                }
            }
            else
                for (int i = 0; i < data.size(); ++i)
                {
                    list=data.at(i).toString().split(",");
                    info.time=list[0];
                    info.price=per(list[2].toFloat());
                    info.vol=list[5].toFloat();
                    info.avePrice=per(list[7].toFloat());
                    h=per(list[3].toFloat());
                    l=per(list[4].toFloat());
                    if (h>GlobalVar::timeShareHighLowPoint[0])
                        GlobalVar::timeShareHighLowPoint[0]=h;
                    if (l<GlobalVar::timeShareHighLowPoint[1])
                        GlobalVar::timeShareHighLowPoint[1]=l;
                    if (info.vol>GlobalVar::timeShareHighLowPoint[2])
                        GlobalVar::timeShareHighLowPoint[2]=info.vol;
                    if (pp<info.price)
                        info.direct=2;
                    else if (pp>info.price)
                        info.direct=1;
                    else
                        info.direct=3;
                    pp=info.price;
                    GlobalVar::mTimeShareChartList.append(info);
                }
        }
        else
        {
            int n=GlobalVar::mTimeShareChartList.count();

            if (n==1)
                pp=GlobalVar::preClose;
            else
                pp=GlobalVar::mTimeShareChartList.at(n-2).price;

            float backPP=GlobalVar::mTimeShareChartList.at(n-1).price;

            if (GlobalVar::curCode.left(2)=="1." or GlobalVar::curCode.left(3)=="399")
                for (int i = 0; i < data.size(); ++i)
                {
                    list=data.at(i).toString().split(",");
                    info.time=list[0];
                    info.price=per(list[2].toFloat());
                    info.vol=list[5].toFloat();
                    info.avePrice=per(list[7].toFloat());
                    h=per(list[3].toFloat());
                    l=per(list[4].toFloat());
                    if (h>GlobalVar::timeShareHighLowPoint[0])
                        GlobalVar::timeShareHighLowPoint[0]=h;
                    if (l<GlobalVar::timeShareHighLowPoint[1])
                        GlobalVar::timeShareHighLowPoint[1]=l;
                    if (info.avePrice>GlobalVar::timeShareHighLowPoint[0])
                        GlobalVar::timeShareHighLowPoint[0]=info.avePrice;
                    if (info.avePrice<GlobalVar::timeShareHighLowPoint[1])
                        GlobalVar::timeShareHighLowPoint[1]=info.avePrice;
                    if (info.vol>GlobalVar::timeShareHighLowPoint[2])
                        GlobalVar::timeShareHighLowPoint[2]=info.vol;
                    if (info.time!=GlobalVar::mTimeShareChartList.at(n-1).time)
                    {
                        pp=backPP;
                        if (pp<info.price)
                            info.direct=2;
                        else if (pp>info.price)
                            info.direct=1;
                        else
                            info.direct=3;
                        backPP=pp=info.price;
                        GlobalVar::mTimeShareChartList.append(info);
                        n=GlobalVar::mTimeShareChartList.count();
                    }
                    else
                    {
                        if (pp<info.price)
                            info.direct=2;
                        else if (pp>info.price)
                            info.direct=1;
                        else
                            info.direct=3;
                        backPP=info.price;
                        GlobalVar::mTimeShareChartList.replace(n-1,info);
                    }
                }
            else
                for (int i = 0; i < data.size(); ++i)
                {
                    list=data.at(i).toString().split(",");
                    info.time=list[0];
                    info.price=per(list[2].toFloat());
                    info.avePrice=per(list[7].toFloat());
                    info.vol=list[5].toFloat();
                    h=per(list[3].toFloat());
                    l=per(list[4].toFloat());
                    if (h>GlobalVar::timeShareHighLowPoint[0])
                        GlobalVar::timeShareHighLowPoint[0]=h;
                    if (l<GlobalVar::timeShareHighLowPoint[1])
                        GlobalVar::timeShareHighLowPoint[1]=l;
                    if (info.vol>GlobalVar::timeShareHighLowPoint[2])
                        GlobalVar::timeShareHighLowPoint[2]=info.vol;
                    if (info.time!=GlobalVar::mTimeShareChartList.at(n-1).time)
                    {
                        pp=backPP;
                        if (pp<info.price)
                            info.direct=2;
                        else if (pp>info.price)
                            info.direct=1;
                        else
                            info.direct=3;
                        backPP=pp=info.price;
                        GlobalVar::mTimeShareChartList.append(info);
                        n=GlobalVar::mTimeShareChartList.count();
                    }
                    else
                    {
                        if (pp<info.price)
                            info.direct=2;
                        else if (pp>info.price)
                            info.direct=1;
                        else
                            info.direct=3;
                        backPP=info.price;
                        GlobalVar::mTimeShareChartList.replace(n-1,info);
                    }
                }
        }
        if (GlobalVar::timeShareHighLowPoint[0]<0)
            GlobalVar::timeShareHighLowPoint[0]=0;
        if (GlobalVar::timeShareHighLowPoint[1]>0)
            GlobalVar::timeShareHighLowPoint[1]=0;
    }
    // qDebug()<<GlobalVar::mTimeShareChartList.count();
}
