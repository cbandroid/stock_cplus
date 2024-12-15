#include "globalvar.h"
#include "threadtimesharetick.h"
#include "NetworkManager.h"

ThreadTimeShareTick::ThreadTimeShareTick(GlobalVar *pGlobalVar,QList<timeShareTickInfo> *&pTimeShareTickList,QObject *parent)
    : QObject{parent}
{
    m_pGlobalVar=pGlobalVar;
    m_pTimeShareTickList=pTimeShareTickList;
    QFile file(m_pGlobalVar->currentPath+"/list/stock_list.csv");
    if (file.open(QIODevice::ReadOnly))
    {
        QTextCodec *codec = QTextCodec::codecForName("GBK");
        QStringList data=codec->toUnicode(file.readAll()).split("\n",Qt::SkipEmptyParts);
        QStringList temp;
        int nCount=data.count();
        for (int i=0;i<nCount;++i)
        {
            temp= data.at(i).split(",");
            areaData.append(temp);
        }
        std::sort(areaData.begin(),areaData.end(),[](QStringList a,QStringList b){
            return a[1]<b[1];
        });
    }
    file.close();
}

void ThreadTimeShareTick::getBuySellTimeShareTick(bool reset)
{
    QString t=m_pGlobalVar->curCode.left(3);
    QList<QString> list;
    list<<"100"<<"122"<<"133"<<"103"<<"104";
    if (list.contains(t))
    {
        if (preCode==m_pGlobalVar->curCode)
            return;
        QString url="http://push2.eastmoney.com/api/qt/stock/sse?mpi=2000&ut=fa5fd1943c7b386f172d6893dbfba10b&fltt=2&invt=1&volt=2&fields=f43,f44,f45,f46,f47,f48,f55,f58,f60,f62,f164,f167,f168,f170,f116,f84,f85,f162,f31,f32,f33,f34,f35,f36,f37,f38,f39,f40,f20,f19,f18,f17,f16,f15,f14,f13,f12,f11,f531&secid="+m_pGlobalVar->getComCode()+"&_=1666089246963";
        getSSEData(1,url);
        url="http://push2.eastmoney.com/api/qt/stock/details/sse?fields1=f1,f2,f3,f4&fields2=f51,f52,f53,f54,f55&mpi=2000&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=1&pos=-0&secid="+m_pGlobalVar->getComCode();
        m_pTimeShareTickList->clear();
        getSSEData(2,url);
        preCode=m_pGlobalVar->curCode;
    }
    else
    {        
        QString qUrl = "http://push2.eastmoney.com/api/qt/stock/get?ut=fa5fd1943c7b386f172d6893dbfba10b&fltt=2&invt=1&volt=2&fields=f43,f44,f45,f46,f47,f48,f55,f58,f60,f62,f164,f167,f168,f170,f116,f84,f85,f162,f31,f32,f33,f34,f35,f36,f37,f38,f39,f40,f20,f19,f18,f17,f16,f15,f14,f13,f12,f11,f531&secid="+m_pGlobalVar->getComCode()+"&_=1666089246963";
        NetworkManager networkManager;
        buySellData = networkManager.getSync<QByteArray>(QUrl(qUrl));
        initBuySellList();
        findStockArea();
        emit getBuySellFinished();

        QString pos="-0";
        if (preCode==m_pGlobalVar->curCode and not reset and m_pGlobalVar->WhichInterface==CNMARKET)
             pos="-10";


        qUrl = "http://push2.eastmoney.com/api/qt/stock/details/get?fields1=f1,f2,f3,f4&fields2=f51,f52,f53,f54,f55&mpi=2000&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&pos="+pos+"&secid="+m_pGlobalVar->getComCode();
        timeShareTickData= networkManager.getSync<QByteArray>(QUrl(qUrl));
        initTimeShareTickList(pos);
        emit getTimeShareTickFinished();

        preCode=m_pGlobalVar->curCode;

    }
}

void ThreadTimeShareTick::getSSEData(int nums,QString url)
{
    QByteArray* qByteArray=new QByteArray();
    QString preCode=m_pGlobalVar->curCode;
    QNetworkRequest request;
    request.setRawHeader("Accept-Language","zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko");

    request.setUrl(QUrl(url));
    QNetworkAccessManager *naManager =new QNetworkAccessManager(this);
    naManager->setAutoDeleteReplies(true);
    naManager->setTransferTimeout(2500);

    QNetworkReply *reply= naManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [=](){

        reply->deleteLater();
        disconnect(reply);
        delete qByteArray;
        naManager->deleteLater();
        delete naManager;
    });
    connect(reply, &QNetworkReply::readyRead, this, [=](){

        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray tempData=reply->readAll();
            if (tempData.contains("data:"))
            {
                if (tempData.contains("\"data\":{\""))
                {
                    if (tempData.mid(tempData.size()-2,2)=="\n\n")
                    {
                        if (nums==1)
                        {
                            disconnect(reply);

                            buySellData=tempData.mid(6,tempData.size()-8);
                            initBuySellList();
                            QString l=m_pGlobalVar->curCode.left(1);
                            if (l=="3" or l=="6" or l=="0")
                                findStockArea();
                            emit getBuySellFinished();
                           qByteArray->clear();
                        }
                        else
                        {
                            disconnect(reply);

                            timeShareTickData=tempData.mid(6,tempData.size()-8);
                            initSSETimeShareTickList();
                            emit getTimeShareTickFinished();
                            qByteArray->clear();
                        }
                    }
                    else
                        qByteArray->append(tempData);
                }
            }
            else
            {
              //  qDebug() <<"tempdata "<< tempData;
                
                qByteArray->append(tempData);
                QByteArray tempByteArray=qByteArray->data();
                //                qDebug()<<tempByteArray;
                if (tempByteArray.mid(tempByteArray.size()-2,2)=="\n\n")
                {
                    if (nums==1)
                    {
                        disconnect(reply);

                        buySellData=tempByteArray.mid(6,tempByteArray.size()-8);
                        initBuySellList();
                        QString l=m_pGlobalVar->curCode.left(1);
                        if (l=="3" or l=="6" or l=="0")
                            findStockArea();
                        emit getBuySellFinished();
                    }
                    else
                    {
                        disconnect(reply);
                        timeShareTickData=tempByteArray.mid(6,tempByteArray.size()-8);
                        initSSETimeShareTickList();
                        emit getTimeShareTickFinished();
                    }
                    qByteArray->clear();
                }
            }
        }
        else
        {
             qDebug()<<reply->error();
        }
    });
}

void ThreadTimeShareTick::initBuySellList()
{
    QJsonParseError jsonError;
  //  qDebug()<< "buySellData " <<buySellData;
    QJsonDocument doc = QJsonDocument::fromJson(buySellData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        if (buySellData.contains("f57"))
        {
            QString code=jsonObject.value("data").toObject().value("f57").toString();
            QString strSymbol=m_pGlobalVar->curCode;
            if (m_pGlobalVar->curCode.contains("."))
                strSymbol=m_pGlobalVar->curCode.split(".")[1];
            if (code!=strSymbol)
                return;
        }
        if (buySellData.contains("f60"))
            m_pGlobalVar->preClose=jsonObject.value("data").toObject().value("f60").toDouble();

        if (buySellData.contains("f58"))
            m_pGlobalVar->curName=jsonObject.value("data").toObject().value("f58").toString();
        for (int i=0;i<10;++i)
        {
            if (buySellData.contains(price[i].toUtf8()))
                m_pGlobalVar->buySellPrice[i]=jsonObject.value("data").toObject().value(price[i]).toDouble();
            else
                m_pGlobalVar->buySellPrice[i]=0;

            if (buySellData.contains(nums[i].toUtf8()))
                m_pGlobalVar->buySellNum[i]=int(jsonObject.value("data").toObject().value(nums[i]).toDouble()+0.5);
            else
                m_pGlobalVar->buySellNum[i]=0;
        }

        for (int i=0;i<14;++i)
        {

            if (buySellData.contains(baseInfo[i].toUtf8()))
                m_pGlobalVar->baseInfoData[i]=jsonObject.value("data").toObject().value(baseInfo[i]).toDouble();
            else
                m_pGlobalVar->baseInfoData[i]=0;
        }
        if ((m_pGlobalVar->WhichInterface==HKMARKET or m_pGlobalVar->WhichInterface==USMARKET) and
            buySellData.contains("f164"))
            m_pGlobalVar->baseInfoData[13]=jsonObject.value("data").toObject().value("f164").toDouble();
        // if (buySellData.contains("f55"))
        //     m_pGlobalVar->baseInfoData[12]=jsonObject.value("data").toObject().value("f55").toDouble();
        m_pGlobalVar->EPSReportDate="每股收益";
        m_pGlobalVar->PEName="市盈率";
        if (buySellData.contains("f62") and m_pGlobalVar->WhichInterface==CNMARKET)
        {
            int num=jsonObject.value("data").toObject().value("f62").toInt();
            if (num>0)
            {
                QString n[4]={"一","二","三","四"};
                m_pGlobalVar->EPSReportDate="收益("+n[num-1]+")";
                m_pGlobalVar->PEName="PE(动)";
            }
        }
    }
}

void ThreadTimeShareTick::initTimeShareTickList(QString pos)
{
    QJsonParseError jsonError;
    //qDebug()<< "pos " << pos;
    //qDebug()<< "timeShareTickData " << timeShareTickData;
    
    QJsonDocument doc = QJsonDocument::fromJson(timeShareTickData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QString code=jsonObject.value("data").toObject().value("code").toString();
        QString strSymbol=m_pGlobalVar->curCode;
        if (m_pGlobalVar->curCode.contains("."))
            strSymbol=m_pGlobalVar->curCode.split(".")[1];
        if (code!=strSymbol)
            return;
        m_pGlobalVar->preClose=jsonObject.value("data").toObject().value("prePrice").toDouble();
        QJsonArray data=jsonObject.value("data").toObject().value("details").toArray();
        int nCount=data.size();
        timeShareTickInfo info;
        QStringList list;

        if (pos=="-10")
        {
            QString qStrTime="";
            int n= m_pTimeShareTickList->size();
            if (n>0){  // fixed 2024.10.17
               qStrTime=m_pTimeShareTickList->at(n-1).time;
            }
            for (int i = 0; i < nCount; ++i)
            {
                list=data.at(i).toString().split(",");
                if (list[0]<=qStrTime)
                {
                    if (i== (nCount-1))
                        return;
                }
                else
                {
                  info.time=list[0];
                  info.price=list[1].toFloat();
                  info.nums=list[2].toInt();
                  info.direct=list[4].toInt();
                  info.tick=list[3].toInt();
                  m_pTimeShareTickList->append(info);
                }
            }
        }
        else
        {
            m_pTimeShareTickList->clear();
            for (int i = 0; i < nCount; ++i)
            {
                list=data.at(i).toString().split(",");
                info.time=list[0];
                info.price=list[1].toFloat();
                info.nums=list[2].toInt();
                info.direct=list[4].toInt();
                info.tick=list[3].toInt();
                 m_pTimeShareTickList->append(info);
            }         
        }
    }
}

void ThreadTimeShareTick::initSSETimeShareTickList()
{
    QJsonParseError jsonError;
    
   // qDebug()<< "initSSETimeShareTickList " << timeShareTickData;
    QJsonDocument doc = QJsonDocument::fromJson(timeShareTickData, &jsonError);
    
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();

        QJsonArray data=jsonObject.value("data").toObject().value("details").toArray();
        timeShareTickInfo info;
        QStringList list;
        int size = data.size();
        for (int i = 0; i < size; ++i)
        {
            list=data.at(i).toString().split(",");
            info.time=list[0];
            info.price=list[1].toFloat();
            info.nums=list[2].toInt();
            info.direct=list[4].toInt();
            info.tick=list[3].toInt();
            m_pTimeShareTickList->append(info);
        }
    }
}

void ThreadTimeShareTick::findStockArea()
{
    int mid,l = 0;
    int r = areaData.count() - 1;
    QStringList s;
    while (l <= r)
    {
        mid = (l + r) / 2;
        s=areaData.at(mid);
        if(s[1]==m_pGlobalVar->curCode)
        {
            if (s[3]!="")
                m_pGlobalVar->curName=s[2]+"("+s[3]+")"+"--"+s[4];
            break;
        }
        else if (s[1]>m_pGlobalVar->curCode)
            r = mid - 1;
        else
            l = mid + 1;
    }
}

