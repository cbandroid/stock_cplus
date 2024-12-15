#include "threadcandlechart.h"
#include "globalvar.h"
#include "NetworkManager.h"
#include <QFile>
#include "tech/KData.h"

ThreadCandleChart::ThreadCandleChart(GlobalVar *pGlobalVar,CKData *&pKline, QObject *parent)
    : QObject{parent}
{
    m_pGlobalVar =pGlobalVar;
    pNetManager = nullptr;

    m_pKline=pKline;
    bReq=false;
    Oldfreq="";
    m_strPreSymbol="";
}

ThreadCandleChart::~ThreadCandleChart()
{
    if (pNetManager != nullptr)
      delete  pNetManager;
}

/*
# 沪深京A股
url = "https://62.push2.eastmoney.com/api/qt/clist/get?cb=jQuery1124007675389012158473_1703949729655&pn=1&pz=20&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&wbp2u=|0|0|0|web&fid=f3&fs=m:0+t:6,m:0+t:80,m:1+t:2,m:1+t:23,m:0+t:81+s:2048&fields=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f12,f13,f14,f15,f16,f17,f18,f20,f21,f23,f24,f25,f22,f11,f62,f128,f136,f115,f152&_=1703949729656"
cmd = {
    "沪深京A股": "f3&fs=m:0+t:6,m:0+t:80,m:1+t:2,m:1+t:23,m:0+t:81+s:2048",
    "上证A股": "f3&fs=m:1+t:2,m:1+t:23",
    "深证A股": "f3&fs=m:0+t:6,m:0+t:80",
    "北证A股": "f3&fs=m:0+t:81+s:2048",
    "新股": "f26&fs=m:0+f:8,m:1+f:8",
    "创业板": "f3&fs=m:0+t:80",
    "科创板": "f3&fs=m:1+t:23",
    "沪股通": "f26&fs=b:BK0707",
    "深股通": "f26&fs=b:BK0804",
    "B股": "f3&fs=m:0+t:7,m:1+t:3",
    "风险警示板": "f3&fs=m:0+f:4,m:1+f:4",
}*/


//获取单只股票的历史K线数据
//beg: str = '19000101', # 开始日期，19000101，表示 1900年1月1日
//end: str = '20500101', # 结束日期
//klt: int = 101,  # 行情之间的时间间隔 1、5、15、30、60分钟; 101:日; 102:周; 103:月
//fqt: int = 1, # 复权方式，0 不复权 1 前复权 2 后复权
//   EASTMONEY_KLINE_FIELDS = {'f51': '日期', 'f52': '开盘', 'f53': '收盘', 'f54': '最高', 'f55': '最低',
//'f56': '成交量', 'f57': '成交额', 'f58': '振幅', 'f59': '涨跌幅', 'f60': '涨跌额', 'f61': '换手率',}
//  # 东方财富网的行情数据接口
//url = f'http://push2.eastmoney.com/api/qt/stock/kline/get?cb=jQuery112408288605192407325_1616943776057&secid={code}.SS&fields1=f1,f2,f3,f4,f5,f6&fields2=f51,f52,f53,f54,f55,f56,f57,f58,f59,f60,f61&klt=101&fqt=1&beg={period}&end={period}&_=1616943776058'

void ThreadCandleChart::getAllCandleChart(QString freq, QString adjustFlag,bool isFirst)
{
    if (m_pGlobalVar->curCode.isEmpty())
        return;

    bReq=true;
    time_t tm = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (m_strPreSymbol!="" and m_strPreSymbol!=m_pGlobalVar->curCode)
    {
      m_strPreSymbol=m_pGlobalVar->curCode;
    }
    QString startDate,endDate;
    if (isFirst and (freq=="101" or freq=="102" or freq=="103" or freq=="104" or freq=="105")){
        startDate=QDateTime::currentDateTime().addDays(-KRANGE*2).toString("yyyyMMdd");
    }
    else if ( isFirst and (freq=="1" or freq=="7" )){
       startDate=QDateTime::currentDateTime().addDays(-7).toString("yyyyMMdd");
    }
    else if ( isFirst and freq=="5" ){
       startDate=QDateTime::currentDateTime().addDays(-KRANGE*7*2/3).toString("yyyyMMdd");
    }
    else if ( isFirst and freq=="30" ){
       startDate=QDateTime::currentDateTime().addDays(-KRANGE*2).toString("yyyyMMdd");
    }
    else if ( isFirst and (freq=="15" or freq=="60" )){
      startDate=QDateTime::currentDateTime().addDays(-KRANGE*7/2).toString("yyyyMMdd");
    }
    else{
       startDate="19900101";
    }

    endDate=QDateTime::currentDateTime().toString("yyyyMMdd");
    NetworkManager networkManager;

    QString qUrl,qStrMSecs;
    qStrMSecs=QString::number(tm);

      QString qStrSmplmt;
      endDate=QDateTime::currentDateTime().toString("yyyyMMdd");
      if (freq=="5" or freq=="15"  ){
          startDate="0";
          qStrSmplmt=QString::number(860);
      }
      else if (freq=="30") {

        startDate=QDateTime::currentDateTime().addMonths(-12).toString("yyyyMMdd");
        qStrSmplmt=QString::number(22*8*6);
      }
      else if ( freq=="60") {
        startDate="0";
        qStrSmplmt=QString::number(22*4*6);
      }
      else
      {        
        qStrSmplmt=QString::number(10000);
        startDate=QDateTime::currentDateTime().addMonths(-36).toString("yyyyMMdd"); //20500101
      }
      m_strPreHead="jQuery35108113868572465672_"+qStrMSecs;
      tm = QDateTime::currentDateTime().toMSecsSinceEpoch();
      qStrMSecs=QString::number(tm);
      qUrl= "https://push2his.eastmoney.com/api/qt/stock/kline/get?cb="+ m_strPreHead+"&secid="+m_pGlobalVar->getComCode()+"&ut=fa5fd1943c7b386f172d6893dbfba10b&fields1=f1,f2,f3,f4,f5,f6&fields2=f51,f52,f53,f54,f55,f56,f57,f58,f59,f60,f61&klt="+freq+"&fqt="+adjustFlag+"&beg="+startDate+"&end="+endDate+"&smplmt="+qStrSmplmt+"&lmt=1000000&_="+qStrMSecs;


      responseData = networkManager.getSync< QByteArray >(QUrl(qUrl));

      initCandleChartList(freq,isFirst);
      emit getCandleChartFinished();
      bReq = false;

    Oldfreq=freq;
    OldAdjustFlag=adjustFlag;

}

void ThreadCandleChart::initCandleChartList(QString freq,bool isFirst)
{
    if (!responseData.contains(m_strPreHead.toUtf8()))
    {        
        m_pKline->Clear();        
        return;
    }
    QList<QByteArray> list= responseData.split('(');
    QByteArray  result = list[1].split(')')[0];
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson( result, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
       // qDebug() <<responseData;
        QJsonObject jsonObject = doc.object();
        QString code=jsonObject.value("data").toObject().value("code").toString();
        int dktotal=jsonObject.value("data").toObject().value("dktotal").toInt();
      //  qDebug() << "Symbol " <<code << "," <<m_pGlobalVar->curCode;

        QString strSymbol=m_pGlobalVar->curCode;
        if (m_pGlobalVar->curCode.contains("."))
            strSymbol=m_pGlobalVar->curCode.split(".")[1];
        if (code!=strSymbol){
           // if (freq!="1" )
           // {
                m_pKline->Clear();                
           // }
            return;
        }
        QJsonArray data=jsonObject.value("data").toObject().value("klines").toArray();

        QStringList list;

        int nRcvCount =  data.size();
      //  qDebug() <<"dktotal="<<dktotal<<"," <<nRcvCount ;
        m_pKline->Clear();
        if (nRcvCount==0)
        {            
            return;
        }
        QDateTime dateTime;
        CL::Kline kd;
        memset(&kd,0x00,sizeof(CL::Kline));
        int nKdCount = m_pKline->size() ;

        int nStartIdx=0;
        int i=0;
//"2024-12-05", "104.82", "108.02", "112.68", "104.82", "756978", "8261905286.56", "7.61", "4.59", "4.74", "13.69"
       // QList<candleChartInfo>::Iterator ptr =  m_pGlobalVar->mCandleChartList.begin();
        for (; i < nRcvCount; ++i)
        {
            list=data.at(i).toString().split(",");
          //  qDebug()<<list;

            if (freq=="1" or freq=="7" or freq=="5" or freq=="30" or freq=="15" or freq=="60"){
                dateTime = QDateTime::fromString(list[0], "yyyy-MM-dd hh:mm");
                kd.date = dateTime.toSecsSinceEpoch();
            }
            else
            {
                dateTime = QDateTime::fromString(list[0], "yyyy-MM-dd");
                kd.date = dateTime.toSecsSinceEpoch();
            }

            kd.open=list[1].toFloat();
            kd.close=list[2].toFloat();
            kd.high=list[3].toFloat();
            kd.low=list[4].toFloat();
            kd.vol=list[5].toFloat();
            kd.amount=list[6].toFloat();

            kd.pctChg=list[8].toFloat();
            kd.turn=list[10].toFloat();

            m_pKline->push_back(kd);

        }
    }
}
