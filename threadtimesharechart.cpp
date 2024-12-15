#include "globalvar.h"
#include "threadtimesharechart.h"
#include "NetworkManager.h"

ThreadTimeShareChart::ThreadTimeShareChart(GlobalVar *pGlobalVar,QList<timeShareChartInfo> *&pTimeShareChartList,QObject *parent)
    : QObject{parent}
{
    m_pGlobalVar =pGlobalVar;
    m_pTimeShareChartList=pTimeShareChartList;
}

ThreadTimeShareChart::~ThreadTimeShareChart()
{
}

void ThreadTimeShareChart::getSSEData()
{
    m_pTimeShareChartList->clear();
    QString url="https://push2his.eastmoney.com/api/qt/stock/trends2/sse?mpi=2000&fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58&ut=fa5fd1943c7b386f172d6893dbfba10b&iscr=0&ndays=1&secid="+m_pGlobalVar->getComCode()+"&_=1666401553893";
    QByteArray* qByteArray=new QByteArray();
    QNetworkRequest request;
    QNetworkAccessManager *naManager =new QNetworkAccessManager(this);
    naManager->setAutoDeleteReplies(true);
    naManager->setTransferTimeout(2500);
    request.setRawHeader("Accept-Language","zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko");

    request.setUrl(QUrl(url));
    reply= naManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=](){
       // qDebug()<<"结束"<<preGCode<<m_pGlobalVar->curCode;
            preGCode="";
            reply->disconnect();            
            qByteArray->clear();
            delete qByteArray;
            naManager->deleteLater();
            delete naManager;

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
                        reply->disconnect();
                        allData=tempData;
                        initSSETimeShareChartList();
                        emit getTimeShareChartFinished();
                        qByteArray->clear();
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
                    reply->disconnect();
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
    QString s=m_pGlobalVar->curCode.left(3);
    QList<QString> list;
    list<<"100"<<"122"<<"133"<<"103"<<"104";
    if (not list.contains(s))
    {
        NetworkManager networkManager;
        QString qUrl ="https://push2his.eastmoney.com/api/qt/stock/trends2/get?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58&ut=fa5fd1943c7b386f172d6893dbfba10b&iscr=0&ndays=1&secid="+m_pGlobalVar->getComCode()+"&_=1666401553893";
        allData = networkManager.getSync<QByteArray>(QUrl(qUrl));
        initTimeShareChartList();
        emit getTimeShareChartFinished();
        allData.clear();
    }
    else if (preGCode!=m_pGlobalVar->curCode or r)
    {
       preGCode=m_pGlobalVar->curCode;
       m_pGlobalVar->timeShareHighLowPoint[0]=-9.0E20f;
       m_pGlobalVar->timeShareHighLowPoint[1]=9.0E20f;
       getSSEData();
    }
}

void ThreadTimeShareChart::initTimeShareChartList()
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);

    m_pTimeShareChartList->clear();

    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QString code=jsonObject.value("data").toObject().value("code").toString();
        QString strSymbol=m_pGlobalVar->curCode;
        if (m_pGlobalVar->curCode.contains("."))
            strSymbol=m_pGlobalVar->curCode.split(".")[1];
        if (code!=strSymbol)
            return;
        m_pGlobalVar->preClose=jsonObject.value("data").toObject().value("preClose").toDouble();
        float preClose=  m_pGlobalVar->preClose;
        int ph=110;
        int pl=90;
        float a=int(preClose*ph+0.5f)/100.0f;
        float b=int(preClose*pl+0.5f)/100.0f;
        if (m_pGlobalVar->curCode.left(1)=="3" or m_pGlobalVar->curCode.left(3)=="688")
        {
            ph=120;
            pl=80;
            a=int(preClose*ph+0.5f)/100.0f;
            b=int(preClose*pl+0.5f)/100.0f;
        }
        else if(m_pGlobalVar->curCode.left(1)=="4" or m_pGlobalVar->curCode.left(1)=="8")
        {
            ph=130;
            pl=70;
            a=int(preClose*ph)/100.0f;
            b=int(preClose*pl)/100.0f;
        }

        m_pGlobalVar->timeShareHighLowPoint[3]=myper(a,preClose); // StopH
        m_pGlobalVar->timeShareHighLowPoint[4]=myper(b,preClose); // StopL
        QJsonArray data=jsonObject.value("data").toObject().value("trends").toArray();
        m_pGlobalVar->trendsTotal=jsonObject.value("data").toObject().value("trendsTotal").toInt();

        m_pGlobalVar->timeShareHighLowPoint[0]=-9.0E20f;
        m_pGlobalVar->timeShareHighLowPoint[1]=9.0E20f;
        m_pGlobalVar->timeShareHighLowPoint[2]=-9.0E20f;

        float pp=0;

        timeShareChartInfo info;
        QStringList list;

            int nSize=data.size();

            //"2024-11-22 09:30,18.05,18.05,18.05,18.05,21150,38175750.00,18.050"
            for (int i = 0; i < nSize; ++i)
            {
              // qDebug() <<data.at(i).toString();

                list=data.at(i).toString().split(",");
                info.time=list[0];                
                info.price=list[2].toFloat();
                info.vol=list[5].toFloat();
                info.avePrice=list[7].toFloat();

                if ( info.price>m_pGlobalVar->timeShareHighLowPoint[0])
                    m_pGlobalVar->timeShareHighLowPoint[0]= info.price;
                if ( info.price<m_pGlobalVar->timeShareHighLowPoint[1])
                    m_pGlobalVar->timeShareHighLowPoint[1]= info.price;

                if (pp<info.price)
                    info.direct=2;
                else if (pp>info.price)
                    info.direct=1;
                else
                    info.direct=3;
                pp=info.price;

                if (info.vol>m_pGlobalVar->timeShareHighLowPoint[2])
                    m_pGlobalVar->timeShareHighLowPoint[2]=info.vol;
              m_pTimeShareChartList->append(info);
            }
        if (m_pGlobalVar->timeShareHighLowPoint[2]<0)
            m_pGlobalVar->timeShareHighLowPoint[2]=0;

    }
}

void ThreadTimeShareChart::initSSETimeShareChartList()
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData.mid(6,allData.size()-6), &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QString code=jsonObject.value("data").toObject().value("code").toString();
        QString strSymbol=m_pGlobalVar->curCode;
        if (m_pGlobalVar->curCode.contains("."))
            strSymbol=m_pGlobalVar->curCode.split(".")[1];
        if (code!=strSymbol)
            return;

        QJsonArray data=jsonObject.value("data").toObject().value("trends").toArray();
     //   qDebug()<<data;
        timeShareChartInfo info;
        QStringList list;
    //    float h, l;
        if (m_pTimeShareChartList->isEmpty())
        {
            float preClose= m_pGlobalVar->preClose;
            float p=jsonObject.value("data").toObject().value("preClose").toDouble();
            if (p!=0){
                m_pGlobalVar->preClose=jsonObject.value("data").toObject().value("preClose").toDouble();
                preClose=  m_pGlobalVar->preClose;
            }

            int ph=110;
            int pl=90;
            float a=int( preClose*ph+0.5f)/100.0f;
            float b=int( preClose*pl+0.5f)/100.0f;
            if (m_pGlobalVar->curCode.left(1)=="3" or m_pGlobalVar->curCode.left(3)=="688")
            {
                ph=120;
                pl=80;
                a=int( preClose*ph+0.5f)/100.0f;
                b=int( preClose*pl+0.5f)/100.0f;
            }
            else if(m_pGlobalVar->curCode.left(1)=="4" or m_pGlobalVar->curCode.left(1)=="8")
            {
                ph=130;
                pl=70;
                a=int( preClose*ph)/100.0f;
                b=int( preClose*pl)/100.0f;
            }

            m_pGlobalVar->timeShareHighLowPoint[3]=myper(a,preClose); //stopH
            m_pGlobalVar->timeShareHighLowPoint[4]=myper(b,preClose); //stopL
            m_pGlobalVar->timeShareHighLowPoint[2]=0;

            m_pGlobalVar->timeShareHighLowPoint[0]=-9.0E20f;
            m_pGlobalVar->timeShareHighLowPoint[1]=9.0E20f;
            m_pGlobalVar->timeShareHighLowPoint[2]=-9.0E20f;

            pp=0;
            int t=jsonObject.value("data").toObject().value("trendsTotal").toInt();
            if (t!=0){
                m_pGlobalVar->trendsTotal=jsonObject.value("data").toObject().value("trendsTotal").toInt();
            }

                int nSize=data.size();
                //qDebug() <<"nSize4="<<nSize;
                for (int i = 0; i < nSize; ++i)
                {
                     //qDebug() <<data.at(i).toString();
                    list=data.at(i).toString().split(",");
                    info.time=list[0];
                    info.price=list[2].toFloat();
                    info.vol=list[5].toFloat();
                    info.avePrice=list[7].toFloat();

                    if ( info.price>m_pGlobalVar->timeShareHighLowPoint[0])
                        m_pGlobalVar->timeShareHighLowPoint[0]= info.price;
                    if ( info.price<m_pGlobalVar->timeShareHighLowPoint[1])
                        m_pGlobalVar->timeShareHighLowPoint[1]= info.price;

                    if (info.vol>m_pGlobalVar->timeShareHighLowPoint[2])
                        m_pGlobalVar->timeShareHighLowPoint[2]=info.vol;
                    if (pp<info.price)
                        info.direct=2;
                    else if (pp>info.price)
                        info.direct=1;
                    else
                        info.direct=3;
                    pp=info.price;
                   m_pTimeShareChartList->append(info);
                }

        }
        else
        {
            int n=m_pTimeShareChartList->count();

            if (n==1)
                pp=m_pGlobalVar->preClose;
            else
                pp=m_pTimeShareChartList->at(n-2).price;

            float backPP=m_pTimeShareChartList->at(n-1).price;



                int nSize=data.size();
                //qDebug() <<"nSize6="<<nSize;
                for (int i = 0; i < nSize; ++i)
                {
                   //  qDebug() <<data.at(i).toString();
                    list=data.at(i).toString().split(",");
                    info.time=list[0];
                    info.price=list[2].toFloat();
                    info.avePrice=list[7].toFloat();
                    info.vol=list[5].toFloat();

                    if ( info.price>m_pGlobalVar->timeShareHighLowPoint[0])
                        m_pGlobalVar->timeShareHighLowPoint[0]= info.price;
                    if ( info.price<m_pGlobalVar->timeShareHighLowPoint[1])
                        m_pGlobalVar->timeShareHighLowPoint[1]= info.price;

                    if (info.vol>m_pGlobalVar->timeShareHighLowPoint[2])
                        m_pGlobalVar->timeShareHighLowPoint[2]=info.vol;
                    if (info.time!=m_pTimeShareChartList->at(n-1).time)
                    {
                        pp=backPP;
                        if (pp<info.price)
                            info.direct=2;
                        else if (pp>info.price)
                            info.direct=1;
                        else
                            info.direct=3;
                        backPP=pp=info.price;
                       m_pTimeShareChartList->append(info);
                       n=m_pTimeShareChartList->count();
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
                        m_pTimeShareChartList->replace(n-1,info);
                    }
                }

        }

        if (m_pGlobalVar->timeShareHighLowPoint[2]<0)
            m_pGlobalVar->timeShareHighLowPoint[2]=0;
    }
}
