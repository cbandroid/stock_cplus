#include <QRandomGenerator>
#include "utilityex.h"
#include "globalvar.h"
#include "downloadtask.h"
#include "requeststocsv.h"
#include "NetworkManager.h"

QDialog* RequestsToCsv::promptDlg= nullptr;
QTimer *RequestsToCsv::qTimer=nullptr;

RequestsToCsv::RequestsToCsv(GlobalVar *pGlobalVar,QList<StockInfo> *&pTableList,QDialog *parent)
    : QDialog{parent}
{

    m_pGlobalVar=pGlobalVar;
    m_pTableListCopy=pTableList;
    downloadedNums=0;
    isDownload=false;

    progressBarWindowDlg = new QDialog();
    stopBtn = new QPushButton("终止下载",progressBarWindowDlg);
    lblnumLine = new QLabel(progressBarWindowDlg);
    progressBar = new QProgressBar(progressBarWindowDlg);

//    naManager = new QNetworkAccessManager(this);
    progressBarWindowDlg->setWindowFlags(progressBarWindowDlg->windowFlags() | Qt::WindowStaysOnTopHint);
    // progressBarWindowDlg->setAttribute(Qt::WA_DeleteOnClose);
    progressBarWindowDlg->setWindowTitle("下载所有股票k线数据");
    progressBarWindowDlg->setGeometry(850, 400, 300, 150);
    QLabel *stockNums = new QLabel("剩余股票数量:",progressBarWindowDlg);
    QGridLayout *mainLayout = new QGridLayout(progressBarWindowDlg);
    progressBarWindowDlg->setLayout(mainLayout);
    mainLayout->addWidget(stockNums, 0, 1);
    mainLayout->addWidget(lblnumLine, 0, 3);
    mainLayout->addWidget(progressBar, 1, 0, 1, 5);
    mainLayout->addWidget(stopBtn, 2, 3);
    threadPool.setMaxThreadCount(QThread::idealThreadCount());
    connect(stopBtn,&QPushButton::clicked,this,[=](){
        if (stopBtn->text()=="停止下载")
        {
            threadPool.clear();
            stopBtn->setText("继续下载");
        }
        else
        {
            stopBtn->setText("停止下载");
            int nCount =pTableList->count();
            for(int i=downloadedNums;i<nCount;++i)
            {
                DownloadTask *workTask=new DownloadTask(pGlobalVar,pTableList,this);
                workTask->nums=i;
                threadPool.start(workTask);
            }
        }

    });
    connect(progressBarWindowDlg,&QDialog::finished,this,[=](){
        threadPool.clear();
        isDownload=false;
    });
}

RequestsToCsv::~RequestsToCsv()
{
    delete stopBtn ;
    delete lblnumLine;
    delete progressBar ;
    delete progressBarWindowDlg;
}


bool RequestsToCsv::getIndexList()
{
//    QDateTime start=QDateTime::currentDateTime();
    QByteArray allData;
    //m_pGlobalVar->getData(allData,3,QUrl(
    QString qUrl ="https://www.joinquant.com/data/dict/indexData";
    NetworkManager networkManager;
    allData= networkManager.getSync< QByteArray >(QUrl(qUrl));
    if (allData.isEmpty())
        return false;
    QString html=QString(allData);
    QString tbody=m_pGlobalVar->peelStr(html,"<tbody","-1");
    QPair<QString, QString> pair;
    QString temp;
    QList<QStringList> indexList;
    QStringList dataList;
    while (1)
    {
        if (tbody.indexOf("<td")==-1)
            break;
        dataList.clear();
        for (int i=0;i<5;++i)
        {
            pair=m_pGlobalVar->cutStr(tbody,"<td","</td>");

            QString content=m_pGlobalVar->getContent(pair.first);
//            qDebug()<<content;
            switch(i)
            {
            case 4:
                temp=content.toUpper();
                break;
            default:
                temp=content;
                break;
            }
            dataList<<temp;
            tbody=pair.second;
        }
//        qDebug()<<dataList;
        indexList.append(dataList);
    }

    QFile file(m_pGlobalVar->currentPath+"/list/abbreviation_index_list.csv");
    if (file.open(QFile::WriteOnly))
    {
        int nCount=indexList.count();
        QStringList dataList;
        for (int i=0;i<nCount;++i)
        {
            dataList.clear();
            for (int j=0;j<5;++j)
            {
                dataList<<indexList.at(i)[j];
            }
            file.write(dataList.join(",").toLocal8Bit()+"\n");
        }
        dataList.clear();
        dataList<<"899050"<<"北证50"<<""<<""<<"BZ50";
        file.write(dataList.join(",").toLocal8Bit()+"\n");
    }
    file.close();
    return true;
}

bool RequestsToCsv::getPlateList()
{
    QList<QStringList> plateList;
    QString url[]={"http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=40&po=0&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&wbp2u=|0|0|0|web&fid=f3&fs=m:90+t:1+f:!50&fields=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f12,f13,f14,f15,f16,f17,f18,f20,f21,f23,f24,f25,f26,f22,f33,f11,f62,f128,f136,f115,f152,f124,f107,f104,f105,f140,f141,f207,f208,f209,f222&_=1665566741514",
                     "http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=600&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&wbp2u=7111416627128474|0|1|0|web&fid=f3&fs=m:90+t:3+f:!50&fields=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f12,f13,f14,f15,f16,f17,f18,f20,f21,f23,f24,f25,f26,f22,f33,f11,f62,f128,f136,f115,f152,f124,f107,f104,f105,f140,f141,f207,f208,f209,f222&_=1682126899835",
                     "http://1.push2.eastmoney.com/api/qt/clist/get?pn=1&pz=100&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&wbp2u=7111416627128474|0|1|0|web&fid=f3&fs=m:90+t:2+f:!50&fields=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f12,f13,f14,f15,f16,f17,f18,f20,f21,f23,f24,f25,f26,f22,f33,f11,f62,f128,f136,f115,f152,f124,f107,f104,f105,f140,f141,f207,f208,f209,f222&_=1682127442774"};
    NetworkManager networkManager;
    QByteArray allData;
    for (int i=0;i<3;++i)
    {
        allData= networkManager.getSync< QByteArray >(QUrl(url[i]));
        if (allData.isEmpty())
            continue;
        dealWithPlateList(plateList,allData);
    }
    std::sort(plateList.begin(),plateList.end(),[](QStringList a,QStringList b){
        return a[2]<b[2];
    });
    QFile file(m_pGlobalVar->currentPath+"/list/concept_industry_board.csv");
    if (file.open(QFile::WriteOnly)){
         int nCount=plateList.count();
        for (int i=0;i<nCount;++i)
            file.write(plateList.at(i)[0].toLocal8Bit()+","+plateList.at(i)[1].toLocal8Bit()+","+plateList.at(i)[2].toLocal8Bit()+"\n");
    }
    file.close();
    return true;
}

void RequestsToCsv::dealWithPlateList(QList<QStringList> &list,const QByteArray &allData)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();
        int nSize=data.size();
        QJsonValue value;
        QVariantMap ceilMap;
         QStringList temp;
        for (int i = 0; i < nSize; ++i)
        {
            value = data.at(i);
            ceilMap = value.toVariant().toMap();
            temp<<ceilMap.value("f12").toString()<<ceilMap.value("f14").toString()<<CNToEL(ceilMap.value("f14").toString());
            list.append(temp);
            temp.clear();
        }
    }
}

/*
 *   # 通过抓包工具Fiddler分析请求头信息得到的起始url,page=1
    start_urls = ['http://quote.eastmoney.com/centerv2/hsgg/xg']

    def parse(self, response):
        # 获取第一页response
        response = requests.get('http://nufm.dfcfw.com/EM_Finance2014NumericApplication/JS.aspx?type=CT&cmd=C.BK05011&sty=FCOIATA&sortType=(ChangePercent)&sortRule=1&page=1&pageSize=20&js=var%20dfFHBxOP={rank:[(x)],pages:(pc),total:(tot)}&token=7bc05d0d4c3c22ef9fca8c2a912d779c&jsName=quote_123&_g=0.628606915911589&_=1523274953883')
        # 正则匹配得到所有页数
        total = re.findall('total:(.*?)}',response.text)[0]
        page = int(total)//20 + 1
        # 循环遍历各页
        for i in range(1,page+1):
            # 拼接所有页的url,总计新股17页
            url = 'http://nufm.dfcfw.com/EM_Finance2014NumericApplication/JS.aspx?type=CT&cmd=C.BK05011&sty=FCOIATA&sortType=(ChangePercent)&sortRule=1&page='+str(i)+'&pageSize=20&js=var%20dfFHBxOP={rank:[(x)],pages:(pc),total:(tot)}&token=7bc05d0d4c3c22ef9fca8c2a912d779c&jsName=quote_123&_g=0.628606915911589&_=1523274953883'
            # 生成request对象,回调函数传给函数parse_num
            request = Request(url, callback=self.parse_num)
            # request传递给框架的downloader处理,生成response给下一个函数
            yield request

    def parse_num(self, response):
        # 取得response中的text文本
        text = response.text
        # 利用爆炸和替换取得股票列表
        stock_list = eval(text.split('=')[1].split(':')[1].replace(',pages',''))
        # 遍历取得股票列表中个股所需信息
        for stock in stock_list:
            stock_msg_list = stock.split(',')
            # 股票代码
            stock_num = stock_msg_list[1]
            # 股票名称
            stock_name = stock_msg_list[2]
            # 股票最新价
            stock_price = stock_msg_list[3]
            # 股票涨跌幅
            stock_change_range = stock_msg_list[5].replace('%','')
            # 股票涨跌额
            stock_change_price = stock_msg_list[4]
            # 将股票代码'30'开头的创业板股票剔除
            if not stock_num.startswith('30'):
f2：最新价
f3：涨跌幅
f4：涨跌额
f5：成交量（手）
f6：成交额
f7：振幅
f8：换手率
f9：市盈率
f10：量比
f12：股票代码
f14：股票名称
f15：最高
f16：最低
f17：今开
f18：昨收
f22：市净率
 */
QString RequestsToCsv::getStockList()
{
    QJsonObject json;
    json.insert("api_name", "stock_basic");
    json.insert("token", m_pGlobalVar->settings->value("token").toString());
    json.insert("fields", "ts_code,symbol,name,area,industry,list_date,cnspell");

    QJsonDocument doc;
    doc.setObject(json);
    QByteArray dataArray = doc.toJson(QJsonDocument::Compact);
    QByteArray allData;

    //挖地鼠API接口
    QString qUrl ="http://api.waditu.com";

    NetworkManager networkManager;

    networkManager.TryPostExSync(qUrl,dataArray,allData);

    QJsonParseError jsonError;
    doc = QJsonDocument::fromJson(allData, &jsonError);

  //  qDebug()<<"getStockList " <<QString(allData);

    if (allData.size()<800)
    {
        if (jsonError.error == QJsonParseError::NoError)
        {
            QJsonObject jsonObject = doc.object();
            QString s=jsonObject.value("msg").toString();
            return "股票处理失败，"+s.left(16);
        }
        else
            return "股票处理失败";
    }
    QFile file(m_pGlobalVar->currentPath+"/list/stock_list.csv");
    if (file.open(QFile::WriteOnly))
    {
        if (jsonError.error == QJsonParseError::NoError)
        {
            QJsonObject jsonObject = doc.object();
            QJsonArray data=jsonObject.value("data").toObject().value("items").toArray();
            int nSize=data.size();
            QStringList dataList;
            for (int i = 0; i < nSize; ++i)
            {

                dataList<<data.at(i)[0].toString()<<data.at(i)[1].toString()<<data.at(i)[2].toString()<<
                    data.at(i)[3].toString()<<data.at(i)[4].toString()<<data.at(i)[5].toString().toUpper()<<data.at(i)[6].toString();
                file.write(dataList.join(",").toLocal8Bit()+"\n");
//                file.write(data.at(i).toString().toLocal8Bit()+"\n");
                dataList.clear();
            }
        }
    }
    file.close();
    return "股票处理成功";
}


void RequestsToCsv::getEastStockList()
{
     QByteArray resData;
    int randomNumber = QRandomGenerator::global()->bounded(99) + 1;
    time_t tm = QDateTime::currentDateTime().toMSecsSinceEpoch();
    QString qStrMSecs=QString::number(tm);
    QString qUrl="http://"+QString::number(randomNumber)+".push2.eastmoney.com/api/qt/clist/get?cb=jQuery1124025377978787991706_1731053611353&pn=1&pz=20&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&dect=1&wbp2u=|0|0|0|web&fid=f3&fs=m:0+t:6,m:0+t:80,m:1+t:2,m:1+t:23,m:0+t:81+s:2048&fields=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f12,f13,f14,f15,f16,f17,f18,f20,f21,f23,f24,f25,f22,f11,f62,f128,f136,f115,f152&_="+qStrMSecs;
    NetworkManager networkManager;
    resData= networkManager.getSync<QByteArray>(QUrl(qUrl));

    QString result =  QString(resData);

    QStringList list= result.split("jQuery1124025377978787991706_1731053611353");
    result = list[1].split("(")[1].split(");")[0];

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(result.toUtf8(), &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();
        int nTotal =  jsonObject.value("data").toObject().value("total").toInt();

        QJsonValue value;
        QVariantMap ceilMap;
        int nSize = data.size();
        for (int i = 0; i < nSize; ++i)
        {
            value = data.at(i);
            ceilMap = value.toVariant().toMap();
            qDebug() << ceilMap.value("f12").toString()<<","<<ceilMap.value("f14").toString()<<CNToEL(ceilMap.value("f14").toString());
        }

        int left = nTotal-20;
        int  Pages=  left/20;
        int n;
        QString strMarket,strSymbol;
        int nMarket;
        for (n=2;n<Pages;n++)
        {
            qUrl="http://"+QString::number(randomNumber)+".push2.eastmoney.com/api/qt/clist/get?cb=jQuery1124025377978787991706_1731053611353&pn="+QString::number(n)+"&pz="+ QString::number(20)+"&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&dect=1&wbp2u=|0|0|0|web&fid=f3&fs=m:0+t:6,m:0+t:80,m:1+t:2,m:1+t:23,m:0+t:81+s:2048&fields=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f12,f13,f14,f15,f16,f17,f18,f20,f21,f23,f24,f25,f22,f11,f62,f128,f136,f115,f152&_="+qStrMSecs;
            resData= networkManager.getSync<  QByteArray >(QUrl(qUrl));
            result =  QString(resData);
            list= result.split("jQuery1124025377978787991706_1731053611353");
            result = list[1].split("(")[1].split(");")[0];
            doc = QJsonDocument::fromJson(result.toUtf8(), &jsonError);

            if (jsonError.error == QJsonParseError::NoError)
            {
                jsonObject = doc.object();
                data=jsonObject.value("data").toObject().value("diff").toArray();

                nSize = data.size();
                for (int i = 0; i < nSize; ++i)
                {
                   //  qDebug() << data.at(i).toString();
                    value = data.at(i);
                    ceilMap = value.toVariant().toMap();
                    nMarket=ceilMap.value("f13").toInt();
                    if ( nMarket==1)
                      strMarket=".SH";
                    else
                      strMarket=".SZ";
                    strSymbol=ceilMap.value("f12").toString()+ strMarket;
                    qDebug() <<strSymbol+ strMarket<<","<<strSymbol<< ","<<ceilMap.value("f14").toString()<<","<<CNToEL(ceilMap.value("f14").toString());
                }
            }
        }

        int lastPage =  left -Pages*20;
        if (lastPage>0)
        {         
            qUrl="http://"+QString::number(randomNumber)+".push2.eastmoney.com/api/qt/clist/get?cb=jQuery1124025377978787991706_1731053611353&pn="+QString::number(lastPage)+"&pz="+ QString::number(20)+"&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&dect=1&wbp2u=|0|0|0|web&fid=f3&fs=m:0+t:6,m:0+t:80,m:1+t:2,m:1+t:23,m:0+t:81+s:2048&fields=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f12,f13,f14,f15,f16,f17,f18,f20,f21,f23,f24,f25,f22,f11,f62,f128,f136,f115,f152&_="+qStrMSecs;
            resData= networkManager.getSync<  QByteArray >(QUrl(qUrl));
            result =  QString(resData);
            list= result.split("jQuery1124025377978787991706_1731053611353");
            result = list[1].split("(")[1].split(");")[0];

            doc = QJsonDocument::fromJson(result.toUtf8(), &jsonError);

            if (jsonError.error == QJsonParseError::NoError)
            {
                jsonObject = doc.object();
                data=jsonObject.value("data").toObject().value("diff").toArray();

                nSize = data.size();
                for (int i = 0; i < nSize; ++i)
                {
                    value = data.at(i);
                    ceilMap = value.toVariant().toMap();
                    qDebug() << ceilMap.value("f12").toString()<<","<<ceilMap.value("f14").toString()<<CNToEL(ceilMap.value("f14").toString());
                }
            }

        }
    }
}

bool RequestsToCsv::dealWithAllList()
{
//    QDateTime start=QDateTime::currentDateTime();
    bool bResult=true;
    QList<QStringList> allDataList;
    QFile saveFile(m_pGlobalVar->currentPath+"/list/abbreviation_list.csv");
    if (saveFile.open(QFile::WriteOnly))
    {
        QFile file(m_pGlobalVar->currentPath+"/list/abbreviation_index_list.csv");
        if (file.open(QFile::ReadOnly))
        {
            QTextCodec *codec = QTextCodec::codecForName("GBK");

            //        QStringList Data = read.readAll().split("\n",Qt::SkipEmptyParts);
            QStringList Data=codec->toUnicode(file.readAll()).split("\n",Qt::SkipEmptyParts);
            QStringList temp;
            QStringList strLine;
            for(int i=0;i<Data.count();++i)
            {              
                strLine = Data.at(i).split(",");
                allDataList.append(temp<<strLine[0]<<strLine[1]<<strLine[4]);
                temp.clear();
            }
        }
        file.close();
        QFile file1(m_pGlobalVar->currentPath+"/list/stock_list.csv");
        if (file1.open(QFile::ReadOnly))
        {
            QTextCodec *codec = QTextCodec::codecForName("GBK");

            //        QStringList Data = read.readAll().split("\n",Qt::SkipEmptyParts);
            QStringList Data=codec->toUnicode(file1.readAll()).split("\n",Qt::SkipEmptyParts);
            QStringList temp;
            QStringList strLine;
            for(int i=0;i<Data.count();++i)
            {                
                strLine = Data.at(i).split(",");
                allDataList.append(temp<<strLine[1]<<strLine[2]<<strLine[5]);
                temp.clear();
            }
        }
        file1.close();
        QFile file2(m_pGlobalVar->currentPath+"/list/concept_industry_board.csv");
        if (file2.open(QFile::ReadOnly))
        {
            QTextCodec *codec = QTextCodec::codecForName("GBK");

            //        QStringList Data = read.readAll().split("\n",Qt::SkipEmptyParts);
            QStringList Data=codec->toUnicode(file2.readAll()).split("\n",Qt::SkipEmptyParts);
            QStringList temp;
            QStringList strLine;
            for(int i=0;i<Data.count();++i)
            {              
                strLine = Data.at(i).split(",");
                allDataList.append(temp<<strLine[0]<<strLine[1]<<strLine[2]);
                temp.clear();
            }
        }
        file2.close();
        std::sort(allDataList.begin(),allDataList.end(),[](QStringList a,QStringList b){
            return a[2]<b[2];
        });
        int nCount=allDataList.count();
        for (int i=0;i<nCount;++i)
        {
            saveFile.write(allDataList.at(i).join(",").toLocal8Bit()+"\n");
        }
    }
    else
        bResult= false;
    saveFile.close();

    QFile saveDigitFile(m_pGlobalVar->currentPath+"/list/digit_list.csv");
    if (saveDigitFile.open(QFile::WriteOnly))
    {
        std::sort(allDataList.begin(),allDataList.end(),[](QStringList a,QStringList b){
            return a[0]<b[0];
        });
        int nCount=allDataList.count();
        for (int i=0;i<nCount;++i)
        {
            saveDigitFile.write(allDataList.at(i).join(",").toLocal8Bit()+"\n");
        }
    }
    else
        bResult= false;
    saveDigitFile.close();
    //    qDebug()<<start.msecsTo(QDateTime::currentDateTime());
    return bResult;
}

void RequestsToCsv::OnAccept()
{
    qTimer->stop();
    delete qTimer;
    promptDlg->accept();
    delete  promptDlg;
}

bool RequestsToCsv::downStockIndexPlateInfo()
{
    promptDlg=new QDialog(this);
    promptDlg->setWindowFlags(promptDlg->windowFlags() | Qt::WindowStaysOnTopHint);
    promptDlg->setWindowTitle("下载指数板块个股信息");
    QTextEdit *promptText=new QTextEdit(promptDlg);
    promptText->setStyleSheet("background-color: rgb(211, 211, 211);");
//    promptDlg->setWindowFlag(Qt::Popup);
    promptDlg->setGeometry(850, 400, 300, 150);
    QVBoxLayout *Vlay=new QVBoxLayout(promptDlg);
    promptDlg->setLayout(Vlay);
    Vlay->addWidget(promptText);
    promptDlg->show();
    promptText->append("开始下载指数、板块、个股信息...请稍等");

    qTimer = new QTimer(promptDlg);
    connect(qTimer, &QTimer::timeout, this, &RequestsToCsv::OnAccept);
    connect(promptDlg, & QDialog::rejected, this, &RequestsToCsv::OnAccept);

    QString qStrDate =m_pGlobalVar->settings->value("isIndexDone").toString();
    //QDateTime curTime=QDateTime::currentDateTime();
    //curTime=m_pGlobalVar->GetLatestTradeTime(curTime);
    QDateTime curTime=m_pGlobalVar->curWorkDay();
    curDate=curTime.toString("yyyy-MM-dd");
    bool bDone=false;
    if (qStrDate !=curDate)
    {
     if (getIndexList()){
        promptText->append("指数处理成功");
        bDone=true;
        m_pGlobalVar->settings->setValue("isIndexDone",curDate);
     }
     else{
        if (getIndexList()){
            promptText->append("指数处理成功");
             bDone=true;
            m_pGlobalVar->settings->setValue("isIndexDone",curDate);
        }
        else{
            promptText->append("指数处理失败");
        }
      }
    }
    qStrDate =m_pGlobalVar->settings->value("isPlateDone").toString();
    if (qStrDate !=curDate)
    {
      if (getPlateList()){
        promptText->append("板块处理成功");
        bDone=true;
        m_pGlobalVar->settings->setValue("isPlateDone",curDate);
      }
      else
      {
        if (getPlateList()){
            promptText->append("板块处理成功");
             bDone=true;
            m_pGlobalVar->settings->setValue("isPlateDone",curDate);
        }
        else{
            promptText->append("板块处理失败");            
         }
      }
    }
    qStrDate =m_pGlobalVar->settings->value("isSymbolDone").toString();
    bool bResult = true;
    if (qStrDate !=curDate)
    {
      QString qStr = getStockList();

      promptText->append(qStr);

      if (qStr.contains("成功")){
           bDone=true;
        m_pGlobalVar->settings->setValue("isSymbolDone",curDate);
        bResult=true;
      }
      else {
        bResult= false;
      }
    }
    if ( bDone){
       dealWithAllList();
    }
    qTimer->start(15*1000); // 设置定时器时间为15*1000毫秒
    return bResult;
}

void RequestsToCsv::isDirExist(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists())
        return;
    dir.mkdir(m_pGlobalVar->currentPath+fullPath);
    dir.mkdir(m_pGlobalVar->currentPath+fullPath+"/sh");
    dir.mkdir(m_pGlobalVar->currentPath+fullPath+"/bj");
    dir.mkdir(m_pGlobalVar->currentPath+fullPath+"/sz");
}

void RequestsToCsv::downloadAllStockK(QList<StockInfo> *&pTableListCopy)
{
    m_pTableListCopy=pTableListCopy;
    QString s =m_pGlobalVar->settings->value("isDownloadK").toString();
    QDateTime curTime=m_pGlobalVar->curRecentWorkDay(0);
    curDate=curTime.toString("yyyy-MM-dd");
    if (s==curDate)
    {
        QMessageBox::information(this,"提示", "已经下载过了", QMessageBox::Ok);
        return;
    }
    if (isDownload)
    {
        progressBarWindowDlg->show();
        return;
    }
    isDownload=true;
    stopBtn->setText("停止下载");
    stopBtn->setEnabled(true);
    int n=pTableListCopy->count();
    lblnumLine->setText(QString::number(n-downloadedNums));
    progressBar->setRange(0, n);
    progressBarWindowDlg->show();
    isDirExist("/list/data");

    for(int i=downloadedNums;i<n;++i)
    {
        DownloadTask *workTask=new DownloadTask(m_pGlobalVar,pTableListCopy,this);
        workTask->nums=i;
        threadPool.start(workTask);
    }
}

Q_INVOKABLE void RequestsToCsv::setText()
{
    downloadedNums+=1;
    int n=m_pTableListCopy->count();
    lblnumLine->setText(QString::number(n-downloadedNums));
    progressBar->setValue(downloadedNums);
    if (downloadedNums==n)
    {
        stopBtn->setEnabled(false);
        stopBtn->setText("下载完成");
        m_pGlobalVar->settings->setValue("isDownloadK",curDate);
    }
}

//QString add_zero_for_string(int content,int len,bool direction)
//{
//    QString con=QString::number(content);
//    int strLen=con.length();
//    if (strLen<len)
//        while (strLen<len)
//        {
//            if (direction)
//                con="0"+con;
//            else
//                con=con+"0";
//            strLen=con.length();
//        }
//    return con;
//}

//void RequestsToCsv::baoShareStockK()
//{
//    QString MESSAGE_SPLIT="\1";
//    QString BAOSTOCK_CLIENT_VERSION = "00.8.80";
//    QString MESSAGE_TYPE_LOGIN_REQUEST = "00";
//    QString msg_body = "login" + MESSAGE_SPLIT + user_id + MESSAGE_SPLIT + password + MESSAGE_SPLIT + "0";
//    QString msg_header = BAOSTOCK_CLIENT_VERSION + MESSAGE_SPLIT + MESSAGE_TYPE_LOGIN_REQUEST + MESSAGE_SPLIT
//                         + add_zero_for_string(msg_body.length(), 10, true);
//    QString head_body = msg_header + msg_body;
//    crc32(Bytef());
//    QTcpSocket *tcpSocket = new QTcpSocket(this);
//    QString ip = "10.25.7.4";
//    qint16 port = 10030;

//    //主动与服务器建立连接
//    tcpSocket->connectToHost(QHostAddress(ip),port);
//    connect(tcpSocket,&QTcpSocket::connected,[=](){
//        qDebug()<<"成功和服务器建立好连接";
//    });
//}
