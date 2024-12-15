#ifdef _MSC_VER
#include <windows.h>
#endif
#include <synchapi.h>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QMediaDevices>
#include <QtMultimedia/QAudioDevice>
#include "globalvar.h"
#include "threadnewsreport.h"
#include "NetworkManager.h"

ThreadNewsReport::ThreadNewsReport(GlobalVar *pGlobalVar,QObject *parent)
    : QObject{parent}
{
    m_pGlobalVar =pGlobalVar;
    tts=new QTextToSpeech(this);
    HasAudioDevices=false;
    QDateTime c=QDateTime::currentDateTime();
    id=c.addSecs(-1800).toString("yyyyMMddhhmmss");

    jinShiNewsReportCurTime = m_pGlobalVar->settings->value("jinShiNewsReportCurTime").toString();
    QString qStr=m_pGlobalVar->settings->value("eastNewsReportCurTime").toString();
    eastNewsReportCurTime = qStr.toLongLong();
    if (eastNewsReportCurTime>12000000 and c.date().month()==1)
    {   eastNewsReportCurTime=10100000;}
    else if (qStr.length() >12){
        QString qStr = m_pGlobalVar->settings->value("eastNewsReportCurTime").toString();
        qStr = qStr.mid(4,8);
        qDebug() <<"qStr="<<qStr;
        eastNewsReportCurTime =qStr.toLongLong();
    }

    QList<QAudioDevice> inputDevices = QMediaDevices::audioInputs();
    QList<QAudioDevice> outputDevices = QMediaDevices::audioOutputs();

    if (inputDevices.isEmpty() || outputDevices.isEmpty()) {
        qWarning() << "No audio devices found.";
       return ;
    }
    HasAudioDevices=true;
    tts->setLocale(QLocale::Chinese);
    tts->setRate(m_pGlobalVar->speechrate);
    tts->setPitch(0.0);
    tts->setVolume(0.6);
}

void ThreadNewsReport::getNewsData()
{
    if (tts->state() == QTextToSpeech::Speaking)
        return;
    newsContent="";
    NetworkManager networkManager;
    QString qUrl ="https://finance.eastmoney.com/yaowen.html";
    allData= networkManager.getSync<QByteArray>(QUrl(qUrl));
    initEastNews();
    initEastNewsReport();
    qUrl =   "https://www.jin10.com/flash_newest.js?t=1667528593473";
    allData= networkManager.getSync<QByteArray>(QUrl(qUrl));
    initNewsReport();
}

void ThreadNewsReport::SpeechNow()
{
   if (newsContent.isEmpty() or not m_pGlobalVar->isSayNews) // fixed 2024.10.17
        return;
    tts->say(newsContent);
}

void ThreadNewsReport::initEastNews()
{
    QString s=m_pGlobalVar->peelStr(QString(allData),"id=\"artitileList1\"","-1");
    QPair<QString, QString> pair;
    eastNewsList.clear();
    for (int i=1;i<=50;++i)
    {
        QStringList dataList;
        pair=m_pGlobalVar->cutStr(s,"<li id","</li>");

        QString content=m_pGlobalVar->peelStr(pair.first,"<a ","-1");
        if (content.isEmpty())
            continue;
        s=pair.second;
        QString temp="href=\"";
        dataList<<content.mid(content.indexOf(temp)+temp.length(),m_pGlobalVar->peelStr(content,temp,"-1").indexOf("\""));
        dataList<<m_pGlobalVar->getContent(content);
        dataList<<m_pGlobalVar->getContent(m_pGlobalVar->peelStr(content,"class=\"time\"","-1"));
        eastNewsList.append(dataList);
    }
    std::sort(eastNewsList.begin(),eastNewsList.end(),[](QStringList a,QStringList b){
        return a[2]>b[2];
    });
}

void ThreadNewsReport::initNewsReport()
{
  if (HasAudioDevices)
  {
    QString cur_time=QDateTime::currentDateTime().toString("yyyyMMddhhmmss").mid(10,2);
    if (cur_time <= "01")
    {
        if (count < 3)
        {
            if (tts->state() == QTextToSpeech::Ready)
                tts->say("休息时间,起来锻炼了");
            count+=1;
        }
    }
    else if (("20" <= cur_time and cur_time <= "21") or ("40" <= cur_time and cur_time<= "41"))
    {
        if (count < 3)
        {
            if (tts->state() == QTextToSpeech::Ready)
                tts->say("转转头,伸个懒腰");
            count+=1;
        }
    }
    else
        count = 0;

    if (allData.size()<=14) //fxied 2024.10.17
    {
       if (newsContent.isEmpty() or not m_pGlobalVar->isSayNews) // fixed 2024.10.17
       {
           return;
       }
       //qDebug()<<newsContent;
       tts->say(newsContent);
       return;
    }
    QByteArray temp=allData.mid(13,allData.size()-14);
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(temp, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonArray array = doc.array();

        int jsNums=array.size()-1;

        while( jsNums!=-1)
        {
            if (isQuit){
                break;
            }


           sayJsNews(array[jsNums].toObject());
           jsNums-=1;

        }      
    }
    if (newsContent.isEmpty() or not m_pGlobalVar->isSayNews) // fixed 2024.10.17
    {
        return;
    }
    //qDebug()<<newsContent;
    tts->say(newsContent);
  }
}

void ThreadNewsReport::sayJsNews(QJsonObject object)
{
    QString newId=object.value("id").toString().mid(0,14);
    if (id>=newId){
        //qDebug()<<id <<">=" << newId;
        return;
    }
    if (object.value("data").toObject().contains("content"))
    {
        QString newsText=object.value("data").toObject().value("content").toString();
        if (newsText.contains("<a") or newsText.contains("点击查看") or newsText.contains("金十图示") or
            newsText.contains("＞＞") or newsText.contains("...") or newsText.contains("......") or
            newsText.contains(">>") or newsText.contains("……") or newsText.contains("免费领取")
            or newsText.contains("盯盘神器"))
            return;
        if (newsText=="" or newsText=="-")
            return;
        // QString dt=QDateTime::fromString(object.value("time").toString().mid(0,19), "yyyy-MM-ddThh:mm:ss").addSecs(28800).toString("yyyy-MM-dd hh:mm:ss");        
        QString dt, qStrDateTime=object.value("time").toString();
        QDateTime dateTime = QDateTime::fromString(qStrDateTime, "yyyy-MM-dd hh:mm:ss");
        if(dateTime.isValid()) {
            dt = dateTime.toString("yyyyMMddhhmmss");
            if (jinShiNewsReportCurTime>=dt)
                return;
        }

        QString sText = newsText;
        sText.replace("<b>","");
        sText.replace("</b","");        
        sText.replace("<br/>","");
        sText.replace("</span>","");
        sText.replace("<span class=\"section-news\">","");
        sText.replace("<br>","");
        
        newsContent+=sText;

        id=newId;
        emit getNewsFinished("<font size=\"4\" color=red>"+qStrDateTime+"</font>"+"<font size=\"4\">"+
                             newsText+"</font>");
        m_pGlobalVar->settings->setValue("jinShiNewsReportCurTime",dt);
    }
}

void ThreadNewsReport::initEastNewsReport()
{
    if (HasAudioDevices)
    {

        if (eastNewsList.count() == 0) //fxied 2024.10.17
        {
            return;
        }

        int et, eastNums = eastNewsList.count() - 1;

        int a;
        QString eastTime, str, m, d, s;

        while (eastNums != -1)
        {
            if (isQuit) {
                break;
            }


            str = eastNewsList.at(eastNums)[2];
            a = str.indexOf("月");
            m = str.mid(0, a);
            d = str.mid(a + 1, str.indexOf("日") - a - 1);
            if (d.length() == 1)
                d = "0" + d;
            s = str.right(5);
            eastTime = m + d + s.left(2) + s.right(2);

            et = eastTime.toInt();

            sayEastNews(eastNewsList[eastNums], et);

            eastNums -= 1;
        }
        eastNewsList.clear();
    }
}

void ThreadNewsReport::sayEastNews(QStringList l, int time)
{
    if (eastNewsReportCurTime>=time){      
        return;
    }

    QString newsText=l[1];
    newsContent+="东方财经:"+newsText+"。";

    emit getNewsFinished("<font size=\"4\" color=red>"+l[2]+"</font>"+"<span> <a href="+l[0]+">"+
                         "<font size=\"4\">"+newsText+"</font>"+"</a> </span>");
    eastNewsReportCurTime=time;
    m_pGlobalVar->settings->setValue("eastNewsReportCurTime",time);
}
