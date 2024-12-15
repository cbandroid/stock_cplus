#include "utilityex.h"
#include "NetworkManager.h"
#include "globalvar.h"

GlobalVar::GlobalVar()
{
    curCode="600519";
    curName="贵州茅台";

    isBoard=false;
    EPSReportDate="每股收益";
    PEName="市盈率";

    preClose=0;
    hisPreClose=0;
    WhichInterface = CNMARKET;
    isKState=false;
    isUsZhStock=false;


    pMainTech=nullptr;
    pSubTech=nullptr;


    mTableListNum=0;

    offsetEnd=0;

    KRange=KRANGE;
    offsetLocal=KRange;

    for(int i=0;i<5;i++){
       timeShareHighLowPoint[i]=0;
       areaFlag[i]=true;
    }

    for(int i=0;i<10;i++){
       buySellPrice[i]=0;
       buySellNum[i]=0;
    }
    for(int i=0;i<10;i++){
        baseInfoData[i]=0;
    }

    for(int i=0;i<4;i++){
       curSortNum[i]=3;
       is_asc[i]=false;
    }

    isSayNews=true;
    speechrate=0.3f;
    trendsTotal=0;
    curBlock=0;


    columns[0]="f12";
    columns[1]="f14";
    columns[2]="f2";
    columns[3]="f3";
    columns[4]="f8";
    columns[5]="f6";
    columns[6]="f22";
    columns[7]="f9";
    columns[8]="f20";
    columns[9]="f21";
    columns[10]="f25";
    columns[11]="f24";
    columns[12]="f5";
    columns[13]="f15";
    columns[14]="f16";
    columns[15]="f17";
    columns[16]="f18";

}

//上海市场,深圳市场
QDateTime GlobalVar::GetLatestTradeTime( QDateTime tmNow ) const
{
    QDate tNow = tmNow.date();
    int  nWeek  = tNow.dayOfWeek();
    QDateTime tTradeLatest;
    QString qStrCurTime = tmNow.toString("hhmm");
    int cur_time = qStrCurTime.toInt();

    //1=Mon, ..., 6=Sat,7=Sun
    if( 6 == nWeek)
    {
        tNow = tNow.addDays(-1);//指向星期五
        tTradeLatest = QDateTime(tNow,QTime(15,0,0));
    }
    else if ( 7 == nWeek)
    {
        tNow = tNow.addDays(-2);//指向星期五
        tTradeLatest = QDateTime(tNow,QTime(15,0,0));
    }
    //9:25前已经开始发送当前交易数据
    else if ( cur_time < 845 )
    {
        if ( 1 == nWeek) // Mon 2011.8.29
        {
             tNow = tNow.addDays(-3);//指向星期五
        }
        else
        {
             tNow = tNow.addDays(-1);//指向前一天
        }
        tTradeLatest = QDateTime(tNow,QTime(15,0,0));
    }
    else if ( cur_time>=1130 &&   cur_time<1300 )
    {
        tTradeLatest = QDateTime(tNow,QTime(11,30,0));
    }
    else if (  cur_time >= 1500 )
    {
       tTradeLatest = QDateTime(tNow,QTime(15,0,0));
    }
    else
    {
        tTradeLatest= tmNow ;
    }
    return tTradeLatest;
}

bool GlobalVar::isZhWorkDay(QDateTime curTime,int select) const
{
    QStringList vacation=settings->value("Vacation_ZH").toStringList();
    QDateTime local=QDateTime::currentDateTime();
    QString cur_date=curTime.toString("MMdd");
    int cur_time= curTime.toString("hhmmss").toInt();

    int time=150000;
    if (select==1)
        time=163000;
    else if(select==2)
        time=230000;
    if (not vacation.contains(cur_date) && isWorkDay(curTime))
    {
        if (local==curTime && cur_time <84500 && cur_time>time)
            return false;
        return true;
    }
    else
        return false;
}

QDateTime GlobalVar::curRecentWorkDay(int select) const
{
    QDateTime curTime=QDateTime::currentDateTime();
    for (int i=0;i<15;++i){
        if (isZhWorkDay(curTime.addDays(-i),select)){          
            return curTime.addDays(-i);
        }
    }
    return curTime;
}

QDateTime GlobalVar::curWorkDay() const
{
    QDateTime curTime=QDateTime::currentDateTime();
    QStringList vacation=settings->value("Vacation_ZH").toStringList(); 
    QDateTime Latest,LatestDate = GetLatestTradeTime( curTime );

    QString cur_date;
    for (int i=0;i<15;++i)
    {
      Latest = LatestDate.addDays(-i);
      cur_date=Latest.toString("MMdd");
      if (not vacation.contains(cur_date))
      {
          return Latest;
      }
    }
    return LatestDate;
}

bool GlobalVar::isZhMarketDay(QDateTime curTime) const
{
    QStringList vacation=settings->value("Vacation_ZH").toStringList();
//    QDateTime curTime=QDateTime::currentDateTime();
    QString cur_date=curTime.toString("MMdd");
    int cur_time= curTime.toString("hhmmss").toInt();
//    qDebug()<<cur_date<<vacation;
    if (not vacation.contains(cur_date) && isWorkDay(curTime) && ((cur_time>=91500&&cur_time<=113010) || (cur_time>=130000&&cur_time<=150010)))
        return true;
    else
        return false;
}

bool GlobalVar::isUSMarketDay(QDateTime curTime) const
{
    QStringList vacation=settings->value("Vacation_US").toStringList();
//    QDateTime curTime=QDateTime::currentDateTime();
    QString cur_date=curTime.addSecs(-46800).toString("MMdd");
    int cur_time= curTime.addSecs(-46800).toString("hhmmss").toInt();
    int startTime=83000;
    int endTime=150010;
    quint8 month = curTime.date().month();
    if (int(month)>10 or int(month)<3)
    {
        startTime=93000;
        endTime=160010;
    }
    //    qDebug()<<cur_time;
    if (not vacation.contains(cur_date) && isWorkDay(curTime) && (cur_time>=startTime and cur_time<=endTime))
        return true;
    else
        return false;
}

bool GlobalVar::isUKMarketDay(QDateTime curTime) const
{
    QStringList vacation=settings->value("Vacation_EN").toStringList();
    //    QDateTime curTime=QDateTime::currentDateTime();
    QString cur_date=curTime.addSecs(-28800).toString("MMdd");
    int cur_time= curTime.addSecs(-28800).toString("hhmmss").toInt();
    int startTime=70000;
    int endTime=173010;
    quint8 month = curTime.date().month();
    if (int(month)>10 or int(month)<4)
        startTime=80000;
    if (not vacation.contains(cur_date) && isWorkDay(curTime) && (cur_time>=startTime and cur_time<=endTime))
        return true;
    else
        return false;
}

bool GlobalVar::isHKMarketDay(QDateTime curTime) const
{
    QStringList vacation=settings->value("Vacation_HK").toStringList();
//    QDateTime curTime=QDateTime::currentDateTime();
    QString cur_date=curTime.toString("MMdd");
    int cur_time= curTime.toString("hhmmss").toInt();
    //    qDebug()<<cur_date<<vacation;
    if (not vacation.contains(cur_date) && isWorkDay(curTime) && ((cur_time>=90000&&cur_time<=121510) || (cur_time>=130000&&cur_time<=161510)))
        return true;
    else
        return false;
}

bool GlobalVar::isTradeTime(QDateTime curTime,int nDelta) const
{
    int Tradetime = (curTime.toSecsSinceEpoch()+ 8 * 3600) % 86400;  // (当前时间 + 8小时 * 3600秒)/(24小时* 3600秒)
    if (WhichInterface==CNMARKET or WhichInterface==FUNDFLOW)
    {
         QDate date = curTime.date();
         int weekDay = date.dayOfWeek();
         if (weekDay==6 or weekDay==7)
             return false;
        //初始化一般在8：45前完成
        // 9:15...11:30 时间在9:15开始接收数据,集合竞价开始
         if (Tradetime >= (9 * 3600 + 15 * 60+ nDelta)  && Tradetime <=( 11 * 3600 + 30 * 60+ nDelta) ) // 9:25...11:30
        {
            return  true;
        }
        else if (Tradetime >= (13 * 3600+ nDelta)  && Tradetime <= (15 * 3600 + 30 * 60+ nDelta) ) // 13:00...15:30
        {
            return true;
        }
    }
    else if (WhichInterface==HKMARKET)
    {
        if (Tradetime >= (9 * 3600+ nDelta)   && Tradetime <= (12 * 3600+ nDelta) ) // 9:00...12:00
        {
            return  true;
        }
        else if (Tradetime >= (13 * 3600- nDelta)  && Tradetime <= (16 * 3600 +nDelta) ) // 13:00...16:00
        {
            return true;
        }
    }
    else if (WhichInterface==USMARKET or WhichInterface==UKMARKET )
    {
        if (Tradetime >= (21 * 3600- nDelta)   && Tradetime <= (23 * 3600+ nDelta) ) // 21:00...23:00
        {
            return  true;
        }
        else if (Tradetime >= 0  && Tradetime <= (2 * 3600+30*60+ nDelta) ) // 00:00...2:30
        {
            return true;
        }
    }
    return false;
}


bool GlobalVar::getVacation() const
{
    QByteArray allData;
   // QNetworkRequest request;
    //    QString url="http://www.sse.com.cn/disclosure/dealinstruc/closed/";
    QString area[4]={"zh","us","hk","en"};
    QString url1="http://ddx.gubit.cn/holiday/";
    //    request.setUrl(QUrl(url));
    //    request.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36");
    //    GlobalVar::getData(allData,2,request);
    //    if (allData.isEmpty())
    //        return;
    //    QString html=QString(allData);
    //    QString str=GlobalVar::peelStr(html,"<tbody>","-1");
    //    QList<QStringList> data;
    //    while(1)
    //    {
    //        if (str.indexOf("<tr")==-1)
    //            break;
    //        QPair<QString, QString> pair=GlobalVar::cutStr(str,"<tr","</tr");
    //        QString s=GlobalVar::peelStr(pair.first,"<tr","-1");

    //    //    qDebug()<<str;
    //        QStringList l;
    //        GlobalVar::getAllContent(s,l,"<td");
    ////        qDebug()<<l;
    //        data.append(l);
    //        str=pair.second;
    //    }
    //    QStringList date;
    //    for (int i=0;i<data.count();++i)
    //    {
    //        QString s=data.at(i)[1].split("休市")[0];
    //        QStringList t=s.split("至");
    //        QDateTime t1[2];
    //        for (int j=0;j<t.count();++j)
    //        {
    //            QString d=t[j].split("日")[0];
    //            int n=d.indexOf("年");
    //            int m=d.indexOf("月");
    //            if (d.length()-m<3)
    //                d.insert(m+1,"0");
    //            if (m-n<3)
    //                d.insert(n+1,"0");
    //            if (i>0)
    //                d=QDateTime::currentDateTime().toString("yyyy")+"-"+d;
    //            t1[j]=QDateTime::fromString(d.replace("年","-").replace("月","-"),"yyyy-MM-dd");
    //            if (t.count()==1)
    //                t1[1]=t1[0].addDays(1);
    //            else
    //                t1[1]=t1[1].addDays(1);
    //        }
    //        while(1)
    //        {
    //            if (t1[0]==t1[1])
    //                break;
    //            if (t1[0].toString("yyyy")==QDateTime::currentDateTime().toString("yyyy"))
    //                date.append(t1[0].toString("MMdd"));
    //            t1[0]=t1[0].addDays(1);
    //        }
    //    }
    //    GlobalVar::settings->setValue("Vacation_ZH",date);
    NetworkManager networkManager;
    QPair<QString, QString> pair;
    QString html, qStr,str;
    for (int i=0;i<int(sizeof(area)/sizeof(area[0]));++i)
    {

        allData = networkManager.getSync<QByteArray>(QUrl(url1+area[i]));


        if (allData.isEmpty())
            return false;
        html=QString(allData);
        str=peelStr(html,"<tbody>","-1");
        QStringList l;

        while(1)
        {
            if (str.indexOf("<tr")==-1)
                break;
            pair=cutStr(str,"<tr","</tr");
            qStr=peelStr(pair.first,"<tr","-1");
            getAllContent(qStr,l,"<td class=\"tw1\"");
            str=pair.second;
        }
        QStringList s;
        int nSize=l.size();
        for (int j=0;j<nSize;++j)
            s<<l[j].mid(5,2)+l[j].right(2);
        if (s.isEmpty())
            continue;
       settings->setValue("Vacation_"+area[i].toUpper(),s);
    }
    settings->setValue("isSetVacation",QDateTime::currentDateTime().toString("yyyy"));
    return true;
}

bool GlobalVar::isInt(QString s) const
{
    bool isInt;
    s.toInt(&isInt);
    return isInt;
}

QString GlobalVar::getComCode() const
{
//    qDebug()<<GlobalVar::curCode;
    if (curCode.length()==5 && isInt(curCode))
        return "116."+curCode;
    else if (curCode.left(1)=="1")
        return curCode;
    else
    {
        if (curCode.left(1)=="6" or curCode.left(1)=="5")
            return "1."+curCode;
        else
            return "0."+curCode;
    }
}

QString GlobalVar::getStockSymbol() const
{
    QString symbol;
    if (curCode.left(1)=="6")
        symbol="sh"+curCode;
    else if (curCode.left(1)=="8" or curCode.left(1)=="4" or curCode.left(1)=="9")
        symbol="bj"+curCode;
    else if (curCode.left(1)=="3" or curCode.left(1)=="0")
        symbol="sz"+curCode;
    else if(curCode.left(2)=="1.")
        symbol="sh"+curCode.split(".")[1];
    else if(curCode.left(3)=="399")
        symbol="sz"+curCode;
    return symbol;
}


QString GlobalVar::getCookies(QString url)
{
    QNetworkRequest request;
    QNetworkAccessManager naManager;
    naManager.setAutoDeleteReplies(true);
    naManager.setTransferTimeout(2500);
    request.setUrl(QUrl(url));
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36");

    QNetworkReply *reply = naManager.get(request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QTimer timer;
    timer.singleShot(1000, &loop, SLOT(quit()));
    timer.start();
    loop.exec();
    QByteArray array=reply->rawHeader("Set-Cookie");
    reply->deleteLater();
    return QString(array);
}

QString GlobalVar::peelStr(const QString &s,QString begin,QString end) const
{
    int bPos=s.indexOf(begin);
    //if ( bPos==-1)
    //    return "";
     bPos +=begin.length();
    int ePos;
    if (end=="-1")
        ePos=end.toInt();
    else
        ePos=s.indexOf(end)+end.length();
    return s.mid(bPos,ePos);
}

QString GlobalVar::getContent(const QString &s)
{
    int bPos=s.indexOf(">")+1;
    int ePos=s.indexOf("<",bPos);
    return s.mid(bPos,ePos-bPos);
}

QString GlobalVar::getAttributeContent(const QString &s, QString attribute,QString symbol)
{
    int b=s.indexOf(attribute)+attribute.length()+2;
    int e=s.indexOf(symbol,b);
    return s.mid(b,e-b);
}

void GlobalVar::getAllContent(QString &content, QStringList &strList, QString begin) const
{
    int bPos,ePos;
    QString s,str;
    while(1)
    {
        if (content.indexOf(begin)==-1)
            break;
        content=GlobalVar::peelStr(content,begin,"-1");
//        qDebug()<<content;
        bPos=content.indexOf(">")+1;
        ePos=content.indexOf("<");
        if (ePos<=bPos)
        {
            s=content.mid(ePos+1,1);
            str=cutStr(content,"<"+s,"</"+s).first;
            getAllContent(str,strList,"<");
        }
        else
            strList.append(content.mid(bPos,ePos-bPos));
    }
}

QPair<QString, QString> GlobalVar::cutStr(const QString &s,QString begin,QString end)  const
{
    int bPos=s.indexOf(begin);
    int ePos=s.indexOf(end,bPos+1)+end.length();
    QPair<QString, QString> pair;
    if (ePos<=bPos)
    {
        QString str=peelStr(s,end,"-1");
        pair=cutStr(str,begin,end);
    }
    else
    {
        pair.first=s.mid(bPos,ePos-bPos);
        if (begin==end)
            pair.second=s.mid(ePos-end.length(),-1);
        else
            pair.second=s.mid(ePos,-1);
    }
    return pair;
}

