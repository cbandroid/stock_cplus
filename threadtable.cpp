#include <QtAlgorithms>
#include <algorithm>
#include "utilityex.h"
#include "globalvar.h"
#include "threadtable.h"
#include "NetworkManager.h"

ThreadTable::ThreadTable(GlobalVar *pGlobalVar,QList<StockInfo> *&pRisingSpeedList,QList<StockInfo> *&pTableList,QList<StockInfo> *&pTableListCopy,QList<StockInfo> *&pMyStockList,QList<QStringList> *&pFundFlowList,QStringList *&pMyStockCode,
                         QObject *parent)
    : QObject{parent}
{
    m_pGlobalVar =pGlobalVar;
    m_pRisingSpeedList=pRisingSpeedList;
    m_pTableList=pTableList;
    m_pTableListCopy=pTableListCopy;
    m_pMyStockList=pMyStockList;
    m_pFundFlowList=pFundFlowList;
    m_pMyStockCode=pMyStockCode;

    StockInfo info;
    info.code="";
    for (int i=0;i<risingSpeedSize;++i)
        m_pRisingSpeedList->append(info);

    readMyStock();
}

void ThreadTable::getTableData(bool bFirst)
{
    QString qUrl,fs="";

    if (m_pGlobalVar->WhichInterface==CNMARKET)
    {
        fs="m:0+t:6,m:0+t:80,m:1+t:2,m:1+t:23,m:0+t:81+s:2048";
        qUrl = "http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=6000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=1&fid=f22&fs="+fs+"&fields=f2,f3,f5,f6,f8,f9,f12,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667954879297";      
    }
    else
    {
        if (m_pGlobalVar->WhichInterface==HKMARKET)
            fs="m:116+t:3,m:116+t:4,m:116+t:1,m:116+t:2";
        else if (m_pGlobalVar->WhichInterface==USMARKET)
        {
            if (m_pGlobalVar->isUsZhStock)
                fs = "b:mk0201";
            else
                fs = "m:105,m:106,m:107";
        }
        else if (m_pGlobalVar->WhichInterface==UKMARKET)
            fs="m:155+t:1,m:155+t:2,m:155+t:3,m:156+t:1,m:156+t:2,m:156+t:5,m:156+t:6,m:156+t:7,m:156+t:8";

        qUrl = "http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=5000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&fid=f6&fs="+fs+"&fields=f2,f3,f5,f6,f8,f9,f12,f13,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667966922156";

    }


    NetworkManager networkManager;
    allData = networkManager.getSync<QByteArray>(QUrl(qUrl));
    initTableList(bFirst);
    if (m_pGlobalVar->WhichInterface==CNMARKET)
    {
        reFlaseMyStock();
    }
    emit getTableDataFinished();
}

bool ThreadTable::compareStockInfoData(const  StockInfo &s1, const  StockInfo &s2)
{
    return s1.code > s2.code;
}

void ThreadTable::initTableList(bool bFirst)
{

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
  //  qDebug() <<"initTableList() " << jsonError.error << allData;
    if (jsonError.error == QJsonParseError::NoError)
    {
        m_pTableListCopy->clear();

        for (int i=0;i<4;++i)
        {
            m_pGlobalVar->upNums[i]=0;
            m_pGlobalVar->downNums[i]=0;
        }
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();
        QJsonValue value;
        QVariantMap ceilMap;
        StockInfo info;

        int size =  data.size();
        if (m_pGlobalVar->WhichInterface==CNMARKET)
        {
            for (int i = 0; i < size; ++i)
            {
                value = data.at(i);
                ceilMap = value.toVariant().toMap();

                info.name = ceilMap.value("f14").toString();
                if (info.name.contains("退"))
                    continue;
                info.code = ceilMap.value("f12").toString();
                info.close = ceilMap.value("f2").toFloat();
                info.pctChg=ceilMap.value("f3").toFloat();
                info.high = ceilMap.value("f15").toFloat();
                info.low = ceilMap.value("f16").toFloat();
                info.preClose=ceilMap.value("f18").toFloat();
                info.turn=ceilMap.value("f8").toFloat();
                info.amount=ceilMap.value("f6").toFloat();
                info.velocity = ceilMap.value("f22").toFloat();
                info.pe = ceilMap.value("f9").toFloat();
                info.totalValue = ceilMap.value("f20").toFloat();
                info.circulatedValue=ceilMap.value("f21").toFloat();
                info.pctYear=ceilMap.value("f25").toFloat();
                info.pctSixty=ceilMap.value("f24").toFloat();
                info.volume = ceilMap.value("f5").toFloat();
                info.open=ceilMap.value("f17").toFloat();
                if (info.pctChg>=8)
                {
                    m_pGlobalVar->upNums[3]+=1;
                    m_pGlobalVar->upNums[2]+=1;
                    m_pGlobalVar->upNums[0]+=1;
                    int ph=110;
                    float a=int(info.preClose*ph+0.5)/100.0;
                    if (info.code.left(1)=="3" or info.code.left(3)=="688")
                    {
                        ph=120;
                        a=int(info.preClose*ph+0.5)/100.0;
                    }
                    else if(info.code.left(1)=="4" or info.code.left(1)=="8")
                    {
                        ph=130;
                        a=int(info.preClose*ph)/100.0;
                    }
                    if (info.close==a)
                        m_pGlobalVar->upNums[1]+=1;
                }
                else if (info.pctChg>=5)
                {
                    m_pGlobalVar->upNums[2]+=1;
                    m_pGlobalVar->upNums[0]+=1;

                }
                else if (info.pctChg>0)
                    m_pGlobalVar->upNums[0]+=1;
                else if (info.pctChg<=-8)
                {
                    m_pGlobalVar->downNums[3]+=1;
                    m_pGlobalVar->downNums[2]+=1;
                    m_pGlobalVar->downNums[0]+=1;
                    int pl=90;
                    float b=int(info.preClose*pl+0.5)/100.0;
                    if (info.code.left(1)=="3" or info.code.left(3)=="688")
                    {
                        pl=80;
                        b=int(info.preClose*pl+0.5)/100.0;
                    }
                    else if(info.code.left(1)=="4" or info.code.left(1)=="8")
                    {
                        pl=70;
                        b=int(info.preClose*pl)/100.0;
                    }
                    if (info.close==b)
                    {
                        m_pGlobalVar->downNums[1]+=1;
                    }
                }
                else if (info.pctChg<=-5)
                {
                    m_pGlobalVar->downNums[2]+=1;
                    m_pGlobalVar->downNums[0]+=1;
                }
                else if (info.pctChg<0)
                    m_pGlobalVar->downNums[0]+=1;
                m_pTableListCopy->append(info);
            }

            if (not m_pGlobalVar->isBoard){
                m_pTableList->clear();
                m_pTableList->assign( m_pTableListCopy->begin(),m_pTableListCopy->end());
			}
            for (int i=0;i<risingSpeedSize;++i)
                m_pRisingSpeedList->replace(i, m_pTableListCopy->at(i));
				
            sortByColumn(m_pRisingSpeedList,m_pGlobalVar->curSortNum[2],m_pGlobalVar->is_asc[2]);
            sortByColumn(m_pTableListCopy,0,true);

        }
        else
        {
            if (bFirst)
            {
               m_pGlobalVar->CodeNameData.clear();// 键盘精灵设置
            }
            QList<QStringList> allDataPinYinList;
            m_pTableList->clear();
            QString strItem;
            int size =  data.size();
            QStringList tmpList;
            for (int i = 0; i < size; ++i)
            {
                tmpList.clear();
                value = data.at(i);
                ceilMap = value.toVariant().toMap();

                info.name = ceilMap.value("f14").toString();

                if (m_pGlobalVar->WhichInterface==USMARKET or m_pGlobalVar->WhichInterface==UKMARKET){
                    info.code = ceilMap.value("f13").toString()+"."+ceilMap.value("f12").toString();

                     strItem=  ceilMap.value("f12").toString()+","+info.name +","+CNToEL(info.name)+","+ceilMap.value("f13").toString();

                    if (bFirst)
                    {
                       allDataPinYinList.append(tmpList<<ceilMap.value("f12").toString()<< info.name << CNToEL(info.name)<<ceilMap.value("f13").toString());
                    }

                }
                else{
                    // code+name+pinyin
                    info.code = ceilMap.value("f12").toString();
                    strItem=  info.code+","+info.name +","+CNToEL(info.name);
                    if (bFirst)
                    {
                       allDataPinYinList.append(tmpList<<info.code<< info.name << CNToEL(info.name));
                    }
                }
                info.close = ceilMap.value("f2").toFloat();
                info.pctChg=ceilMap.value("f3").toFloat();

                if (bFirst)
                {
                   m_pGlobalVar->CodeNameData.append(strItem);
                }

                if (info.pctChg>=8)
                {
                    m_pGlobalVar->upNums[3]+=1;
                    m_pGlobalVar->upNums[2]+=1;
                    m_pGlobalVar->upNums[0]+=1;
                }
                else if (info.pctChg>=5)
                {
                    m_pGlobalVar->upNums[2]+=1;
                    m_pGlobalVar->upNums[0]+=1;
                }
                else if (info.pctChg>0)
                    m_pGlobalVar->upNums[0]+=1;
                else if (info.pctChg<=-8)
                {
                    m_pGlobalVar->downNums[3]+=1;
                    m_pGlobalVar->downNums[2]+=1;
                    m_pGlobalVar->downNums[0]+=1;
                }
                else if (info.pctChg<=-5)
                {
                    m_pGlobalVar->downNums[2]+=1;
                    m_pGlobalVar->downNums[0]+=1;
                }
                else if (info.pctChg<0)
                    m_pGlobalVar->downNums[0]+=1;
                info.turn=ceilMap.value("f8").toFloat();
                info.amount=ceilMap.value("f6").toFloat();
                info.velocity = ceilMap.value("f22").toFloat();
                info.pe = ceilMap.value("f9").toFloat();
                info.totalValue = ceilMap.value("f20").toFloat();
                info.circulatedValue=ceilMap.value("f21").toFloat();
                info.pctYear=ceilMap.value("f25").toFloat();
                info.pctSixty=ceilMap.value("f24").toFloat();
                info.volume = ceilMap.value("f5").toFloat();
                info.high = ceilMap.value("f15").toFloat();
                info.low = ceilMap.value("f16").toFloat();
                info.open=ceilMap.value("f17").toFloat();
                info.preClose=ceilMap.value("f18").toFloat();

                 m_pTableList->append(info);
            }


           if (bFirst)
           {
             m_pGlobalVar->CodeNameData.sort();
             std::sort(allDataPinYinList.begin(),allDataPinYinList.end(),[](QStringList a,QStringList b){
                return a[2]<b[2];
             });
             m_pGlobalVar->PinYinData.clear();
             for (const QStringList& innerList : allDataPinYinList) {
               m_pGlobalVar->PinYinData.append(innerList.join(","));
             }
           }

        }
        sortByColumn( m_pTableList,m_pGlobalVar->curSortNum[0],m_pGlobalVar->is_asc[0]);

    }
}

void ThreadTable::readMyStock()
{
    *m_pMyStockCode=m_pGlobalVar->settings->value("myStock").toStringList();
    StockInfo info;
    int nCount=m_pMyStockCode->count();   
    for(int i=0;i<nCount;++i)
    {
        info.code=m_pMyStockCode->at(i);
        m_pMyStockList->append(info);
    }
}

void ThreadTable::reFlaseMyStock()
{
    if (m_pTableListCopy->isEmpty()){
        return;
    }
    StockInfo info;
    int nCount=m_pMyStockList->count();
    for (int i=0;i<nCount;++i)
    {
        info=findStock(m_pTableListCopy,m_pMyStockList->at(i).code);
        m_pMyStockList->replace(i,info);
    }    
}
