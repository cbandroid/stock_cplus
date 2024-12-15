#include "globalvar.h"
#include "downloadtask.h"
#include "NetworkManager.h"

DownloadTask::DownloadTask(GlobalVar *pGlobalVar,QList<StockInfo> *pTableList,QObject *obj):
    m_pObj(obj)
{
    m_pGlobalVar =pGlobalVar;
    m_pTableList=pTableList;
    QDateTime curTime=pGlobalVar->curRecentWorkDay(0);
    curDate=curTime.toString("yyyy-MM-dd");
    endDate=curTime.toString("yyyyMMdd");
}

void DownloadTask::downloadK()
{
    QString code=m_pTableList->at(nums).code;
    QString path;
    QString startDate;
    if (code.left(1)=="6")
    {
        path="/list/data/sh/"+code+".csv";
        code= "1."+code;
    }
    else if (code.left(1)=="8" or code.left(1)=="4")
    {
        path="/list/data/bj/"+code+".csv";
        code= "0."+code;
    }
    else
    {
        path="/list/data/sz/"+code+".csv";
        code= "0."+code;
    }

    QFile file(m_pGlobalVar->currentPath+path);
    if (file.open(QFile::ReadOnly) and file.size()>0)
    {
        QTextStream in(&file);
        QString s = in.readAll();
        int post=s.lastIndexOf("\n",s.length()-3);
        if (post==-1)
            in.seek(0);
        else
            in.seek(post+1);
        QString oneLine = in.readLine();
        QString str=oneLine.mid(0,oneLine.indexOf(","));
        if (str==curDate)
            return;
        startDate=QDateTime::fromString(str,"yyyy-MM-dd").addDays(1).toString("yyyyMMdd");
        //            qDebug()<<startDate;
    }
    else
        startDate="0";
    file.close();

    if (file.open(QFile::Append))
    {
         QByteArray allData;
        QString qUrl="http://push2his.eastmoney.com/api/qt/stock/kline/get?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58,f59,f60,f61&beg="+startDate+"&end="+endDate+"&ut=fa5fd1943c7b386f172d6893dbfba10b&rtntype=6&secid="+code+"&klt=101&fqt=0";
        NetworkManager networkManager;
        allData = networkManager.getSync<QByteArray>(QUrl(qUrl));

        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
        if (jsonError.error == QJsonParseError::NoError)
        {
            QJsonObject jsonObject = doc.object();
            QJsonArray data=jsonObject.value("data").toObject().value("klines").toArray();
            int nSize= data.size();
            for (int j = 0; j < nSize; ++j)
                file.write(data.at(j).toString().toLocal8Bit()+"\n");
        }
    }
    file.close();
}
