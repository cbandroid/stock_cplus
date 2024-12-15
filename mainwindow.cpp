#include <QLoggingCategory>
#include <QTextCodec>
#include <Windows.h>
#include "globalvar.h"
#include <qheaderview.h>

#include "utilityex.h"
#include "stockinfo.h"
#include "jspickstock.h"
#include "threadtable.h"
#include "threadindex.h"
#include "threadnewsreport.h"
#include "threadtimesharetick.h"
#include "threadtimesharechart.h"
#include "threadcandlechart.h"
#include "modeltablestock.h"
#include "modelfundflow.h"
#include "modeltimeshare.h"
#include "ui_mainwindow.h"
#include "ttscfgdlg.h"
#include "mainwindow.h"
#include "tech/Technique.h"

#define CIRCLE_GREEN_SHEETSTYLE  "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px;  border:1px solid black;background:green"
#define CIRCLE_RED_SHEETSTYLE  "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px;  border:1px solid black;background:red"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    m_pGlobalVar = new GlobalVar();

    pFundFlow = new FundFlow(m_pGlobalVar);
    m_pTableList = new QList<StockInfo>();
    m_pTableListCopy = new QList<StockInfo>();
    m_pRisingSpeedList = new QList<StockInfo>();
    m_pMyStockList = new QList<StockInfo>();
    m_pFundFlowList = new QList<QStringList>();

    m_pMyStockCode = new QStringList();
    m_pIndexList = new QList<IndexInfo>();
    m_pTimeShareTickList = new QList<timeShareTickInfo>();
    m_pTimeShareChartList = new QList<timeShareChartInfo>();
    m_pHisTimeShareChartList = new QList<timeShareChartInfo>();

    m_pTimeShareTickModel = new ModelTimeShare(m_pGlobalVar);

    pRequestsToCsv = new RequestsToCsv(m_pGlobalVar,m_pTableListCopy);

    pTableStock = new TableStock( m_pGlobalVar, m_pMyStockCode,  m_pMyStockList,
                                 m_pTableList,
                                 m_pRisingSpeedList,
                                 m_pTimeShareTickList);


    m_pTableModel =  new  ModelTableStock(m_pGlobalVar,m_pMyStockCode);
    m_pRisingSpeedModel=  new  ModelTableStock(m_pGlobalVar,m_pMyStockCode);
    m_pMyStockModel=  new  ModelTableStock(m_pGlobalVar,m_pMyStockCode);


    pDrawChart=new  DrawChart(m_pGlobalVar);
    pSearchStock =new SearchStock (m_pGlobalVar);
    pF10View=new  F10View(m_pGlobalVar);

    m_pKline = new CKData();

    pTech=nullptr;
    nTech=STT_VOLUME;
    bNewCalc= false;
    bWantReqKline=false;
    ifCanClick=CNCLICK;
    //Py_Initialize();
    //pModule=PyImport_ImportModule("qmt");
    m_pGlobalVar->isKState=false;
    QMTProcess=new QProcess(this);
    pPickStock=new JSPickStock(m_pGlobalVar, m_pTableList, m_pTableListCopy);
    SelectedtableView=nullptr;
    ReqKtimeCount=0;
    AleadyInCandleChartPainter = false;
    lastTime=QDateTime::currentDateTime();
    dldlastTime=QDateTime::currentDateTime();
    initGlobalVar();
    initInterface();
    initSettings();
    initThread();   
    initSignals();
   
    tim = new QTimer(this);
    tim->setInterval(500);
    connect(tim,SIGNAL(timeout()),this,SLOT(tradingTimeRunThread()));
    tim->start();    
}

MainWindow::~MainWindow()
{
    tim->stop();
    delete tim;
    SetThreadExecutionState(ES_CONTINUOUS );
    threadNewsReport->isQuit=true;
    //Py_Finalize();
    saveCode();
    pRequestsToCsv->progressBarWindowDlg->close();
    QMTProcess->close();

    for (int i=0;i<6;++i)
    {
        thread[i]->quit();
        thread[i]->wait();
        delete  thread[i];
    }
    delete pPickStock;
    delete m_pTableModel ;
    delete m_pRisingSpeedModel;
    delete m_pMyStockModel;
    delete m_pTimeShareTickModel ;

    delete  pDrawChart;

    delete  pSearchStock ;
    delete  pF10View;
    threadTable->deleteLater();
    delete threadTable;
    threadIndex->deleteLater();
    delete threadIndex;
    threadTimeShareTick->deleteLater();
    delete threadTimeShareTick;
    threadTimeShareChart->deleteLater();
    delete threadTimeShareChart;
    threadNewsReport->deleteLater();
    delete threadNewsReport;
    threadCandleChart->deleteLater();
    delete threadCandleChart;

    m_pTableList->clear();
    delete m_pTableList ;
    m_pTableListCopy->clear();
    delete m_pTableListCopy ;
    m_pRisingSpeedList->clear();
    delete m_pRisingSpeedList ;
    m_pMyStockList->clear();
    delete m_pMyStockList ;
    m_pFundFlowList->clear();
    delete m_pFundFlowList ;
    m_pMyStockCode->clear();
    delete m_pMyStockCode;
    m_pIndexList->clear();
    delete m_pIndexList ;
    m_pTimeShareTickList->clear();
    delete m_pTimeShareTickList ;
    m_pTimeShareChartList->clear();
    delete m_pTimeShareChartList ;

    m_pHisTimeShareChartList->clear();
    delete m_pHisTimeShareChartList ;


    delete pRequestsToCsv;
    delete pTableStock ;


    if (pTech!=nullptr)
       delete pTech;

    delete  m_pGlobalVar;

    m_pKline->Clear();
    delete m_pKline;

    delete ui;
}

void MainWindow::initGlobalVar()
{
    m_pGlobalVar->currentPath=QCoreApplication::applicationDirPath();
    m_pGlobalVar->settings=new QSettings(m_pGlobalVar->currentPath+"/config.ini",QSettings::IniFormat,this);
    m_pGlobalVar->curCode=m_pGlobalVar->settings->value("curCode").toString();
    m_pGlobalVar->isSayNews=m_pGlobalVar->settings->value("sayNews").toBool();
    m_pGlobalVar->speechrate=m_pGlobalVar->settings->value("speechrate").toFloat();
   // m_pGlobalVar->offsetEnd=m_pGlobalVar->settings->value("offsetEnd").toInt();
    downloadDate=m_pGlobalVar->settings->value("curTime").toString();
    feelingDate=m_pGlobalVar->settings->value("feelingTime").toString();
    account=m_pGlobalVar->settings->value("account").toString();

//    QString path=m_pGlobalVar->currentPath+"/python/";
//    Py_SetPythonHome((wchar_t *)(reinterpret_cast<const wchar_t *>(path.utf16())));
    for (int i=0;i<5;++i)
        m_pGlobalVar->areaFlag[i]=true;
    m_pGlobalVar->pRed.setColor(QPalette::WindowText, Qt::red);
    m_pGlobalVar->pGreen.setColor(QPalette::WindowText, QColor(0,191,0));
    m_pGlobalVar->pBlack.setColor(QPalette::WindowText, Qt::black);
    m_pGlobalVar->pBlue.setColor(QPalette::WindowText, Qt::blue);
    m_pGlobalVar->pWhite.setColor(QPalette::WindowText, Qt::white);
}

void MainWindow::initThread()
{
    for (int i=0;i<6;++i)
        thread[i]=new QThread(this);

    threadTable=new ThreadTable(m_pGlobalVar,m_pRisingSpeedList,m_pTableList,m_pTableListCopy,m_pMyStockList,m_pFundFlowList,m_pMyStockCode);
    threadTable->moveToThread(thread[0]);
    connect(threadTable,&ThreadTable::getTableDataFinished,this,[=](){
        pTableStock->setTableView();

        if (not m_pTimeShareChartList->isEmpty())
        {
            QDate t_ = dateEdit1->date();
            QString qStrTime =  m_pTimeShareChartList->at(0).time;
            QDateTime tt = QDateTime::fromString(qStrTime, "yyyy-MM-dd hh:mm");
            QDate tt_ = tt.date();

            if (tt_ !=t_)
                return;
        }

        feelingData[0]->setText(QString::number(m_pGlobalVar->upNums[0]));
        feelingData[1]->setText(QString::number(m_pGlobalVar->upNums[1]));
        feelingData[2]->setText(QString::number(m_pGlobalVar->upNums[2])+"/"+QString::number(m_pGlobalVar->upNums[3]));
        feelingData[4]->setText(QString::number(m_pGlobalVar->downNums[0]));
        feelingData[5]->setText(QString::number(m_pGlobalVar->downNums[1]));
        feelingData[6]->setText(QString::number(m_pGlobalVar->downNums[2])+"/"+QString::number(m_pGlobalVar->downNums[3]));
        feelingData[3]->setText(format_conversion(threadIndex->totalAmount));
        /*baseInfoData[7]->setText(QString::number(m_pGlobalVar->upNums[0]));
        baseInfoData[16]->setText(QString::number(m_pGlobalVar->downNums[0]));
        baseInfoData[8]->setText(QString::number(m_pGlobalVar->upNums[1])+"/"+QString::number(m_pGlobalVar->upNums[2]));
        baseInfoData[17]->setText(QString::number(m_pGlobalVar->downNums[1])+"/"+QString::number(m_pGlobalVar->downNums[2]));
		*/
    });
    connect(this,&MainWindow::startThreadTable,threadTable,&ThreadTable::getTableData);
    thread[0]->start();
    emit startThreadTable(true);

    threadIndex=new ThreadIndex(m_pGlobalVar,m_pIndexList,m_pFundFlowList);
    threadIndex->moveToThread(thread[1]);
    connect(threadIndex,SIGNAL(getIndexFinished()),this,SLOT(reFlashIndex()));
    connect(threadIndex,&ThreadIndex::getBlockFinished,this,[=](){
        int row=pTableStock->blockView->currentIndex().row();
        if (row==-1)
            row=0;
        pTableStock->m_fundFlowModel->setModelData(*m_pFundFlowList,false);
       pTableStock->blockView->setCurrentIndex(pTableStock->m_fundFlowModel->index(row,0));
    });
    connect(this,&MainWindow::startThreadIndex,threadIndex,&ThreadIndex::getAllIndex);
    thread[1]->start();
    emit startThreadIndex(true);

    threadTimeShareTick=new ThreadTimeShareTick(m_pGlobalVar,m_pTimeShareTickList);
    threadTimeShareTick->moveToThread(thread[2]);
    connect(threadTimeShareTick,&ThreadTimeShareTick::getTimeShareTickFinished,this,[=](){
        pTableStock->setTimeShareTickView();
    });
    connect(threadTimeShareTick,SIGNAL(getBuySellFinished()),this,SLOT(reFlashBuySellBaseInfo()));
    connect(this,&MainWindow::startThreadTimeShareTick,threadTimeShareTick,&ThreadTimeShareTick::getBuySellTimeShareTick);
    thread[2]->start();
    emit startThreadTimeShareTick(true);

    threadNewsReport=new ThreadNewsReport(m_pGlobalVar);
    threadNewsReport->moveToThread(thread[3]);
    connect(threadNewsReport,&ThreadNewsReport::getNewsFinished,this,[=](QString s){
        if (s.isEmpty())
            return;
        QTextDocument *document = newsData->document(); 
        int lineCount = document->blockCount(); // 获取文本的行数
        if  (lineCount >300) //fixed 限制使用内存
         {
            newsData->clear();
        }
        newsData->append(s);
    });
    connect(this,&MainWindow::startThreadGetNews,threadNewsReport,&ThreadNewsReport::getNewsData);
    thread[3]->start();
    emit startThreadGetNews();

    threadTimeShareChart=new ThreadTimeShareChart(m_pGlobalVar,m_pTimeShareChartList);
    threadTimeShareChart->moveToThread(thread[4]);
    connect(threadTimeShareChart,&ThreadTimeShareChart::getTimeShareChartFinished,this,[=](){
//        QWidget *pActiveWindow = QApplication::activeWindow();
//        MainWindow *pMainWindow = dynamic_cast<MainWindow*>(pActiveWindow);
//        if(pMainWindow && pMainWindow == this)
            pDrawChart->timeShareChart->update();

    });

    connect(this,&MainWindow::startThreadTimeShareChart,threadTimeShareChart,&ThreadTimeShareChart::getAllTimeShareChart);
    thread[4]->start();
    emit startThreadTimeShareChart(false);

    threadCandleChart=new ThreadCandleChart(m_pGlobalVar,m_pKline);
    threadCandleChart->moveToThread(thread[5]);
    connect(threadCandleChart,&ThreadCandleChart::getCandleChartFinished,this,[=](){
        bNewCalc=true;
        bWantReqKline=false;
        pDrawChart->candleChart->update();
    });
    connect(this,&MainWindow::startThreadCandleChart,threadCandleChart,&ThreadCandleChart::getAllCandleChart);
    thread[5]->start();

    QLoggingCategory::setFilterRules(QStringLiteral("qt.speech.tts=true \n qt.speech.tts.*=true"));

}

void MainWindow::initInterface()
{
    ui->setupUi(this);
    setWindowTitle("Stock");
    setWindowIcon(QIcon(":/new/png/png/logo.ico"));

    QScreen* screen = QGuiApplication::primaryScreen();
    int w=screen->size().width();
    int h=screen->size().height();

    this->resize(w,h);

    if (not m_pGlobalVar->isSayNews)
        ui->newsReport->setChecked(true);
    // if (m_pGlobalVar->settings->value("isSetVacation").toString()==QDateTime::currentDateTime().toString("yyyy"))
    //     ui->setVacation->setEnabled(false);
    QActionGroup *market = new QActionGroup(this);
    market->addAction(ui->HKMarket);
    market->addAction(ui->USMarket);
    market->addAction(ui->ZHMarket);
    ui->ZHMarket->setChecked(true);

    ui->horizontalLayout->setSpacing(0);
    ui->horizontalLayout->addWidget(pTableStock->stockTableView);
    ui->horizontalLayout->addWidget(pDrawChart->candleChart);
    pDrawChart->candleChart->hide();

    middleWindow=new QWidget(this);
    QVBoxLayout *middleLayout =new QVBoxLayout;
    QHBoxLayout *middleDLayout=new QHBoxLayout;
    middleWindow->setLayout(middleLayout);
    middleLayout->setSpacing(0);
    middleLayout->setContentsMargins(0,0,0,0);
    middleDLayout->setSpacing(0);
    middleDLayout->setContentsMargins(0,0,0,0);

    pTableStock->stockTableView->setMinimumWidth(486);

    pTableStock->risingSpeedView->setMaximumWidth(285);
    pTableStock->risingSpeedView->setMinimumHeight(496);
    pTableStock->myStockView->setMinimumHeight(496);

    pTableStock->blockView->setMinimumWidth(780);

    middleLayout->addWidget(pTableStock->blockView);
    middleLayout->addLayout(middleDLayout);
    middleDLayout->addWidget(pTableStock->risingSpeedView);

    middleDLayout->addWidget(pTableStock->myStockView);

    rightBaseWindow=new QWidget(this);
    rightBaseWindow->setMaximumWidth(450);
    QVBoxLayout *rightLayout =new QVBoxLayout(rightBaseWindow);
    rightBaseWindow->setLayout(rightLayout);
    rightLayout->setSpacing(0);
    rightLayout->setContentsMargins(0,0,0,0);
    //2
    rightFundWindow=new QWidget(this);
    rightFundWindow->setMaximumWidth(250);
    QVBoxLayout *rightFundLayout=new QVBoxLayout(rightFundWindow);
    rightFundWindow->setLayout(rightFundLayout);
//    rightFundLayout->setSpacing(0);
    rightFundLayout->setContentsMargins(20,30,20,2);

    ui->horizontalLayout->addWidget(middleWindow);
    ui->horizontalLayout->addWidget(rightBaseWindow);
    ui->horizontalLayout->addWidget(rightFundWindow);
    rightFundWindow->hide();

    QGridLayout *baseInfoLayout=new QGridLayout();
    baseInfoLayout->setSpacing(2);
    baseInfoLayout->setContentsMargins(10, 0, 0, 0);
    QGridLayout *buySellLayout=new QGridLayout();
    buySellLayout->setSpacing(2);
    buySellLayout->setContentsMargins(10, 5, 0, 0);
    newsData=new QTextBrowser(this);
    newsData->setMaximumHeight(300);
    QHBoxLayout *freqAdjustLayout =new QHBoxLayout();
    freqAdjustLayout->setSpacing(0);
    QGridLayout *feelingLayout=new QGridLayout();
    feelingLayout->setContentsMargins(30, 10, 30, 10);

    rightLayout->addLayout(baseInfoLayout);
    rightLayout->addLayout(buySellLayout);
    rightLayout->addLayout(feelingLayout);
    rightLayout->addWidget(newsData);
    rightLayout->addLayout(freqAdjustLayout);
    QButtonGroup *freq=new QButtonGroup(this);
    QStringList periodAdjustName={"1F","5F","30F","日线","周线","月线","不复权","前复权","后复权"};
  //   QStringList periodAdjustName={"日线","周线","月线","不复权","前复权","后复权"};
    for (int i=0;i<9;++i)
    {
        periodAdjust[i]=new QRadioButton(periodAdjustName[i],this);
        if (i<6)
            freq->addButton(periodAdjust[i]);
        freqAdjustLayout->addWidget(periodAdjust[i]);
    }
    periodAdjust[0]->setChecked(true);
    periodAdjust[6]->setChecked(true);

    pDrawChart->timeShareChart->setMinimumHeight(300);
    rightLayout->addWidget(pDrawChart->timeShareChart);

    initBaseInfoLayout(baseInfoLayout);
    initBuySellLayout(buySellLayout);
    initFeelingLayout(feelingLayout);
    //2
    periodBox->addItems({"即时", "3日排行", "5日排行", "10日排行", "20日排行"});
    periodBox->setCurrentIndex(3);
    northBox->addItems({"今日", "3日", "5日", "10日", "月", "季", "年"});
    singleStockBoard->addItems({"近一月", "近三月", "近半年", "近一年"});
    tradedetailBox->addItems({m_pGlobalVar->curRecentWorkDay(1).toString("yyyy-MM-dd"),"近3日", "近5日", "近10日", "近30日"});
    openFundBox->addItems({"全部","股票型","混合型","债券型","指数型","QDII","LOF","FOF"});
    dateEdit->setCurrentSection(QDateEdit::DaySection);
    QStringList fundFlowName={"东方财富板块资金流","同花顺板块资金流","新高新低数量","股票热度、淘股吧",
                        "异常波动","昨日强势股票","北向资金买入","龙虎榜详情","个股上榜统计","开放式基金排行"};
    for (int i=0;i<10;++i)
    {
        if (i==1)
            rightFundLayout->addWidget(periodBox);
        else if(i==6)
            rightFundLayout->addWidget(dateEdit);
        else if(i==7)
            rightFundLayout->addWidget(northBox);
        else if(i==8)
            rightFundLayout->addWidget(tradedetailBox);
        else if(i==9)
            rightFundLayout->addWidget(singleStockBoard);
        fundFlow[i]=new QPushButton(fundFlowName[i],this);
        rightFundLayout->addWidget(fundFlow[i]);
    }
    rightFundLayout->addWidget(openFundBox);
    QTextBrowser *instructionText=new QTextBrowser(this);
    instructionText->setStyleSheet("background-color:#F3F3F3;");
//    instructionText->setMaximumHeight(300);
    rightFundLayout->addWidget(instructionText);
    instructionText->append(m_pGlobalVar->settings->value("Instruction").toString());

    circle=new QLabel(this);
    ui->statusBar->addWidget(circle);
    circle->setStyleSheet(CIRCLE_RED_SHEETSTYLE);
    QPushButton *indexButton;
    QLabel *indexLabel;
    QString styleSheet1;
    QString styleSheet2;
    if (w>2160){
        styleSheet1="QPushButton{font:bold;font-size:16px;font-family:微软雅黑;}";
        styleSheet2="QLabel{font:bold;font-size:16px;}";
    }
    else{
        styleSheet1="QPushButton{font:bold;font-size:13px;font-family:微软雅黑;}";
        styleSheet2="QLabel{font:bold;font-size:13px;}";
    }

    for(int i=1;i<=8;++i)
    {
//        QHBoxLayout *statusBarLayout=new QHBoxLayout(ui->statusBar);
//        ui->statusBar->setLayout(statusBarLayout);
        indexButton = new QPushButton(QString::asprintf("Button%d", i), this);
        indexButton->setStyleSheet(styleSheet1);
        indexLabel=new QLabel(QString::asprintf("Label%d", i), this);
        indexLabel->setStyleSheet(styleSheet2);
        ui->statusBar->addWidget(indexButton);
        ui->statusBar->addWidget(indexLabel);
    }
}
void MainWindow::initSettings()
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    if (m_pGlobalVar->isSayNews)
    {
        QIcon icon(":/new/png/png/report.png");
        ui->newsReport->setIcon(icon);
        ui->newsReport->setText("关闭语音播报");
    }
    else
    {
        QIcon icon(":/new/png/png/no_report.png");
        ui->newsReport->setIcon(icon);
        ui->newsReport->setText("打开语音播报");
    }
    newsData->setOpenExternalLinks(true);
    pDrawChart->hisTimeShareChart->setParent(this);

    pTableStock->stockTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    pTableStock->risingSpeedView->setContextMenuPolicy(Qt::CustomContextMenu);
    pTableStock->myStockView->setContextMenuPolicy(Qt::CustomContextMenu);
    setFocusPolicy(Qt::StrongFocus);


    pTableStock->myStockView->setFocusPolicy(Qt::ClickFocus);//  Qt::StrongFocus);

    pTableStock->myStockView->horizontalHeader()->setHighlightSections(false);

    connect(pTableStock->stockTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(ontableViewclicked(QModelIndex)));
    connect(pTableStock->blockView, SIGNAL(clicked(QModelIndex)), this, SLOT(ontableViewclicked(QModelIndex)));

    connect(pTableStock->risingSpeedView, SIGNAL(clicked(QModelIndex)), this, SLOT(ontableViewclicked(QModelIndex)));
    connect(pTableStock->myStockView, SIGNAL(clicked(QModelIndex)), this, SLOT(ontableViewclicked(QModelIndex)));

    pTableStock->stockTableView->horizontalHeader()->setStretchLastSection(true);
    pTableStock->blockView->horizontalHeader()->setStretchLastSection(true);
    pTableStock->risingSpeedView->horizontalHeader()->setStretchLastSection(true);
    pTableStock->myStockView->horizontalHeader()->setStretchLastSection(true);

    QWidget::setTabOrder(pTableStock->stockTableView, pTableStock->blockView);
    QWidget::setTabOrder(pTableStock->blockView,pTableStock->risingSpeedView);
    QWidget::setTabOrder(pTableStock->risingSpeedView, pTableStock->myStockView);
    QWidget::setTabOrder(pTableStock->myStockView,pTableStock->stockTableView);


    QScreen* screen = QGuiApplication::primaryScreen();
    
    searchSmallWindow=new QWidget(this);
    searchSmallWindow->setWindowFlag(Qt::Popup);
    int w=screen->size().width();
    int h=screen->size().height();
    searchSmallWindow->setGeometry(w-450, h-435, 400, 350);

    QVBoxLayout *search =new QVBoxLayout(searchSmallWindow);
    
    search->setContentsMargins(2, 2, 2, 2);
    searchSmallWindow->setLayout(search);
    search->addWidget(pSearchStock->searchCodeLine);
    search->addWidget(pSearchStock->matchCodeText);

    F10SmallWindow=new QWidget(this);
    QWidget *F10Title=new QWidget(this);
    F10SmallWindow->setWindowFlag(Qt::Popup);
    F10SmallWindow->move(200,100);
    F10SmallWindow->hide();
    QVBoxLayout *f10MainLayout=new QVBoxLayout(F10SmallWindow);
    F10SmallWindow->setLayout(f10MainLayout);
    QHBoxLayout *f10ContentLayout =new QHBoxLayout();
    QHBoxLayout *titleLayout=new QHBoxLayout(F10Title);

    QPushButton *close=new QPushButton(F10SmallWindow);
    connect(close,&QPushButton::clicked,this,[=](){F10SmallWindow->close();});
    F10Title->setLayout(titleLayout);
    F10Title->setMaximumHeight(TITLEHEIGHT);
//    f10Title->setStyleSheet("background-color:rgb(153, 204, 255);");
    fTitle->setStyleSheet("QLabel{font:bold 18px;font-family:微软雅黑}");

    f10MainLayout->addWidget(F10Title);
    f10MainLayout->addLayout(f10ContentLayout);
    f10MainLayout->setContentsMargins(0,0,0,0);

    QIcon myicon;
    myicon.addFile(tr(":/new/png/png/close.png"));
    close->setIcon(myicon);
    close->setIconSize(QSize(20,20));
    close->setMaximumSize(QSize(30,30));
    titleLayout->addWidget(fTitle);
    titleLayout->addWidget(close);

    QWidget *navigation=new QWidget(F10SmallWindow);
    f10ContentLayout->addWidget(navigation);
    f10ContentLayout->addWidget(pF10View->stockInfoView);
    QVBoxLayout *navi=new QVBoxLayout(navigation);
    navigation->setLayout(navi);
    QStringList F10InfoName={"热度概念排名","主要指标","经营分析","资产负债表","利润表","现金流量表"};
    for (int i=0;i<6;++i)
    {
        F10Info[i]=new QPushButton(F10InfoName[i],F10SmallWindow);
        navi->addWidget(F10Info[i]);
    }
}
void MainWindow::initBaseInfoLayout(QGridLayout *baseInfoLayout)
{
    stockCode = new QLabel(this);
    stockName = new QLabel(this);
    stockCode->setStyleSheet("QLabel{font:bold 18px;color:black}");
    stockName->setStyleSheet("QLabel{font:bold 26px;font-family:微软雅黑;color:red}");
    // baseInfoLayout->setVerticalSpacing(1);
    baseInfoLayout->addWidget(stockCode, 0, 0, 2, 1);
    baseInfoLayout->addWidget(stockName, 0, 1, 2, 3);

    //QString lName[]={"现价", "涨幅","换手", "成交额", "总股本", "总市值", "市净率", "上涨总数","上涨5/8",
    //                   "今开", "最高", "最低", "成交量(手)", "流通股", "", "", "下跌总数","下跌5/8"};
					   
    QString lName[]={"现价", "涨幅","换手", "成交额", "总股本", "总市值", "市净率", "今开", "最高", "最低", "成交量(手)", "流通股", "", ""};
 					   
    QLabel *lblName;
    /*for (int i=0;i<18;++i)
    {
        lblName=new QLabel(lName[i],this);
        baseInfoData[i]=new QLabel(this);
        lblName->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(47,79,79)}");
        if ((i >= 2 and i <= 6) or (i >= 12 and i <= 15))
            baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px;color:blue}"); //rgb(72,61,139)blue
        else if (i == 7 or i==8)
        {
            baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px;color:red}");
//            name->setMaximumWidth(90);
        }
        else if (i == 16 or i==17)
        {
            baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px;color:green}");
//            name->setMaximumWidth(90);
        }
        else
            baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px}");
        if (i < 9)
        {
            baseInfoLayout->addWidget(lblName,i+2,0);
            baseInfoLayout->addWidget(baseInfoData[i],i+2,1);
        }
        else if (i<18)
        {
            baseInfoLayout->addWidget(lblName,i-7,2);
            baseInfoLayout->addWidget(baseInfoData[i],i-7,3);
        }
    }*/
	for (int i=0;i<14;++i)
    {
        lblName=new QLabel(lName[i],this);
        baseInfoData[i]=new QLabel(this);
        lblName->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(47,79,79)}");
        if (i < 7)
        {
            if (i>1)
                baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px;color:blue}");
            else
                baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px}");
            baseInfoLayout->addWidget(lblName,i+2,0);
            baseInfoLayout->addWidget(baseInfoData[i],i+2,1);
        }
        else if (i<14)
        {
            if (i>9)
                baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px;color:blue}");
            else
                baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px}");
            baseInfoLayout->addWidget(lblName,i-5,2);
            baseInfoLayout->addWidget(baseInfoData[i],i-5,3);
        }
    }
    EPSLabel=new QLabel(this);
    EPSLabel->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(47,79,79)}");
    PELabel=new QLabel(this);
    PELabel->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(47,79,79)}");
    baseInfoLayout->addWidget(EPSLabel,7,2);
    baseInfoLayout->addWidget(PELabel,8,2);
}
void MainWindow::initBuySellLayout(QGridLayout *BuySellLayout)
{
    QString n[5]={"一","二", "三", "四", "五"};
    QLabel *buySellName[10];
    for (int i=0;i<5;++i)
    {
        buySellName[i]=new QLabel("卖"+n[4-i],this);
        buySellPrice[i]=new QLabel(this);
        buySellPrice[i]->setContextMenuPolicy(Qt::CustomContextMenu);
        buySellNum[i]=new QLabel(this);
        buySellNum[i]->setAlignment(Qt::AlignRight);
        buySellName[i]->setStyleSheet("QLabel{font:bold 16px;font-family:微软雅黑;color:rgb(47,79,79)}");
        buySellPrice[i]->setStyleSheet("QLabel{font:16px;font-family:微软雅黑}");
        buySellNum[i]->setStyleSheet("QLabel{font:bold 16px;color:rgb(255,165,0);font-family:微软雅黑}");
        BuySellLayout->addWidget(buySellName[i],i,0);
        BuySellLayout->addWidget(buySellPrice[i],i,1);
        BuySellLayout->addWidget(buySellNum[i],i,2);

        buySellName[i+5]=new QLabel("买"+n[i],this);
        buySellPrice[5+i]=new QLabel(this);
        buySellPrice[5+i]->setContextMenuPolicy(Qt::CustomContextMenu);
        buySellNum[5+i]=new QLabel(this);
        buySellNum[5+i]->setAlignment(Qt::AlignRight);
        buySellName[i+5]->setStyleSheet("QLabel{font:bold 16px;font-family:微软雅黑;color:rgb(47,79,79)}");
        buySellPrice[i+5]->setStyleSheet("QLabel{font:16px;font-family:微软雅黑}");
        buySellNum[i+5]->setStyleSheet("QLabel{font:bold 16px;color:rgb(255,165,0);font-family:微软雅黑}");
        BuySellLayout->addWidget(buySellName[i+5],i+6,0);
        BuySellLayout->addWidget(buySellPrice[5+i],i+6,1);
        BuySellLayout->addWidget(buySellNum[5+i],i+6,2);
    }
    buySellName[0]->setMinimumWidth(40);
    buySellPrice[0]->setMinimumWidth(45);
    buySellNum[0]->setMinimumWidth(40);
    QFrame *line=new QFrame(this);
    line->setStyleSheet("QFrame{/*background:yellow;*/min-height:2px;border-top:2px dotted gray}");
//    line->setFrameShape(QFrame::HLine);
//    line->setFrameShadow(QFrame::Sunken);
    BuySellLayout->addWidget(line,5,0,1,3);

    pTableStock->timeShareTickView->setMinimumWidth(300);
    BuySellLayout->addWidget(pTableStock->timeShareTickView,0,3,11,3);
}

void MainWindow::initFeelingLayout(QGridLayout *feelingLayout)
{
    QString lName[]={"上涨总数:", "实际涨停:","上涨5/8:", "实时量能:", "下跌总数:", "实际跌停:", "下跌5/8:", "预测量能:"};
    QLabel *label1=new QLabel("市场情绪:");
    label1->setStyleSheet("QLabel{font:bold 26px;font-family:微软雅黑;}");
    dateEdit1->setCalendarPopup(true);

    QDateTime curTime=m_pGlobalVar->curWorkDay();
    QDate t_= curTime.date();
    dateEdit1->setDate(t_);

    dateEdit1->setMaximumDate(t_);
    dateEdit1->setFocusPolicy(Qt::NoFocus);
    feelingLayout->addWidget(label1,0,0,2,2);
    feelingLayout->addWidget(dateEdit1,0,3,2,2);

    for (int i=0;i<8;++i)
    {
        QLabel *name=new QLabel(lName[i],this);
        name->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(47,79,79)}");
        feelingData[i]=new QLabel(this);
        feelingData[i]->setAlignment(Qt::AlignCenter);
        if (i < 4)
        {
            feelingLayout->addWidget(name,i+2,0);
            feelingLayout->addWidget(feelingData[i],i+2,1,1,2);
            feelingData[i]->setStyleSheet("QLabel{font:bold 20px;color:red}");
        }
        else
        {
            feelingLayout->addWidget(name,i-2,3);
            feelingLayout->addWidget(feelingData[i],i-2,4,1,2);
            feelingData[i]->setStyleSheet("QLabel{font:bold 20px;color:green}");
        }
    }
}

void MainWindow::initSignals()
{
    connect(pTableStock->stockTableView, &QTableView::clicked, this, [this](const QModelIndex &index){
        int row=index.row();
        if (row<0 or row >=m_pTableList->size())  //fixed
            return;

        if (m_pGlobalVar->WhichInterface==FUNDFLOW)
        {
            if(ifCanClick==CNCLICK)
            {
                m_pGlobalVar->curCode=m_pTableList->at(row).code;
                m_pGlobalVar->curName=m_pTableList->at(row).name;
            }
            else if(ifCanClick==FUNDFLOWCLICK)
            {
                m_pGlobalVar->curCode= pFundFlow->model->item(row,0)->text();
                m_pGlobalVar->curName= pFundFlow->model->item(row,1)->text();
            }
            else if(ifCanClick==NORMALCLICK)
            {
                pFundFlow->isClick=true;
                pFundFlow->getFundFlowChartData( pFundFlow->model->item(row,13)->text());
                pFundFlow->fundFlowChart->setWindowTitle( pFundFlow->model->item(row,0)->text()+" 资金流图表");
                pFundFlow->fundFlowChart->show();
                pFundFlow->fundFlowChart->update();
                pFundFlow->fundFlowChart->move(POSXCHART,POSYCHART);
                pFundFlow->fundFlowChart->move(POSXCHART+1,POSYCHART);
            }
        }
        else
        {
            m_pGlobalVar->curCode=m_pTableList->at(row).code;
            emit startThreadTimeShareChart(false);
            emit startThreadTimeShareTick(false);
        }
    });
    connect(pTableStock->risingSpeedView, &QTableView::clicked, this, [this](const QModelIndex &index){
        int curRow=index.row();
        if (curRow<0 or curRow >=m_pRisingSpeedList->size())  //fixed
            return;
        m_pGlobalVar->curCode= m_pRisingSpeedList->at(curRow).code;
//        m_pGlobalVar->curName=m_pGlobalVar->mRisingSpeedList.at(curRow).name;
        emit startThreadTimeShareChart(false);
        emit startThreadTimeShareTick(false);
    });
    connect(pTableStock->myStockView, &QTableView::clicked, this, [this](const QModelIndex &index){
        int curRow=index.row();
        if (curRow<0 or curRow >=m_pMyStockList->size())  //fixed
            return;
        m_pGlobalVar->curCode= m_pMyStockList->at(curRow).code;
        emit startThreadTimeShareChart(false);
        emit startThreadTimeShareTick(false);
    });
    // connect(pTableStock->blockView,&QTableView::clicked,this, [this](const QModelIndex &index){
    //     int row=index.row();
    //     pFundFlow->isClick=true;
    //     pFundFlow->getFundFlowChartData(m_pGlobalVar->mFundFlowList.at(row)[13]);
    //     pFundFlow->fundFlowChart->setWindowTitle(m_pGlobalVar->mFundFlowList.at(row)[0]+" 资金流图表");
    //     pFundFlow->fundFlowChart->show();
    //     pFundFlow->fundFlowChart->update();
    //     pFundFlow->fundFlowChart->move(859,150);
    //     pFundFlow->fundFlowChart->move(860,150);
    // });
    connect(ui->ZHMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->HKMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->USMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->USzMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->UKMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(pTableStock->stockTableView, &QTableView::doubleClicked, this, [this](const QModelIndex &index){
        if (m_pGlobalVar->WhichInterface==FUNDFLOW)
        {
            if (ifCanClick==NORMALCLICK)
            {
                pFundFlow->getBoardStock(m_pTableList, pFundFlow->FundFlowList.at(index.row())[0]);
                pTableStock->m_tableModel->setModelData(*m_pTableList,false,true,false);
                pTableStock->stockTableView->setModel(pTableStock->m_tableModel);
                for (int i=0;i<16;++i)
                    pTableStock->stockTableView->setColumnWidth(i,90);
                ifCanClick=CNCLICK;
            }
            else if(ifCanClick==CNCLICK or ifCanClick==FUNDFLOWCLICK)
            {
                m_pGlobalVar->isKState=true;
                bWantReqKline=true;
                lastTime=QDateTime::currentDateTime();
                bool bFirst;
                if (m_PreCode != m_pGlobalVar->curCode )
                {
                    bFirst=true;
                    m_PreCode=m_pGlobalVar->curCode;
                }
                else
                {
                    bFirst=false;
                }
                emit startThreadCandleChart(freq,adjustFlag,bFirst);
                emit startThreadTimeShareTick(false);
                emit startThreadTimeShareChart(false);
                toInterFace("k");
            }
            else
                QMessageBox::information(this,"提示","只能查看个股和东方板块",QMessageBox::Ok);
        }
        else
        {
            m_pGlobalVar->isKState=true;

            lastTime=QDateTime::currentDateTime();
            bWantReqKline=true;
            bool bFirst;
            if (m_PreCode != m_pGlobalVar->curCode )
            {
                bFirst=true;
                m_PreCode=m_pGlobalVar->curCode;
            }
            else
            {
                bFirst=false;
            }
            emit startThreadCandleChart(freq,adjustFlag,bFirst);
            toInterFace("k");
        }
    });
    connect(pTableStock->risingSpeedView, &QTableView::doubleClicked, this, [this](const QModelIndex &/*index*/){
        m_pGlobalVar->isKState=true;
//        int curRow=index.row();
//        m_pGlobalVar->curCode=m_pGlobalVar->mRisingSpeedList.at(curRow).code;
//        m_pGlobalVar->curName=m_pGlobalVar->mRisingSpeedList.at(curRow).name;

        lastTime=QDateTime::currentDateTime();
        bWantReqKline=true;
        bool bFirst;
        if (m_PreCode != m_pGlobalVar->curCode )
        {
            bFirst=true;
            m_PreCode=m_pGlobalVar->curCode;
        }
        else
        {
            bFirst=false;
        }
        emit startThreadCandleChart(freq,adjustFlag, bFirst);
        toInterFace("k");
    });
    connect(pTableStock->blockView, &QTableView::doubleClicked, this, [this](const QModelIndex &index){
        pTableStock->isFlashTable=false;
        m_pGlobalVar->isBoard=true;
        m_pGlobalVar->curBoard=m_pFundFlowList->at(index.row())[13];
        pSearchStock->getBoardData(m_pTableList);
        if (not m_pTableList->isEmpty())
            pTableStock->m_tableModel->setModelData(*m_pTableList,false,true,false);
        pTableStock->isFlashTable=true;

    });
    connect(pTableStock->myStockView, &QTableView::doubleClicked, this, [this](const QModelIndex &/*index*/){
        m_pGlobalVar->isKState=true;
        isTraversalMyStock=true;

//        int curRow=index.row();
//        m_pGlobalVar->curCode=m_pGlobalVar->mMyStockList.at(curRow).code;
//        m_pGlobalVar->curName=m_pGlobalVar->mMyStockList.at(curRow).name;
        lastTime=QDateTime::currentDateTime();
        bWantReqKline=true;
        bool bFirst;
        if (m_PreCode != m_pGlobalVar->curCode )
        {
            bFirst=true;
            m_PreCode=m_pGlobalVar->curCode;
        }
        else
        {
            bFirst=false;
        }
        emit startThreadCandleChart(freq,adjustFlag,bFirst);
        toInterFace("k");
    });

    // 信号发出，stockTableView进行排序
    connect(pTableStock->stockTableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, [this](int logicalIndex/*, Qt::SortOrder order*/) {
        //        bool is_asc;
        if (m_pGlobalVar->WhichInterface!=FUNDFLOW or(m_pGlobalVar->WhichInterface==FUNDFLOW and ifCanClick==CNCLICK))
        {
            if (m_pGlobalVar->curSortNum[0]!=logicalIndex)
            {
                m_pGlobalVar->is_asc[0] = false;
                m_pGlobalVar->curSortNum[0]=logicalIndex;
            }
            else
                m_pGlobalVar->is_asc[0] = not preSort[0];
            sortByColumn(m_pTableList,logicalIndex,m_pGlobalVar->is_asc[0]);
            preSort[0]=m_pGlobalVar->is_asc[0];
            pTableStock->m_tableModel->setModelData(*m_pTableList,false,true,false);
            // pTableStock->stockTableView->setModel(pTableStock->m_tableModel);
            pTableStock->stockTableView->setCurrentIndex(pTableStock->m_tableModel->index(0,0));
            pTableStock->stockTableView->scrollTo(pTableStock->m_tableModel->index(0,0));
        }
//        else if (m_pGlobalVar->WhichInterface==FUNDFLOW and ifCanClick==FUNDFLOWCLICK)
//        {
//            pFundFlow->model->sort(logicalIndex,Qt::DescendingOrder);
//            pTableStock->stockTableView->setModel( pFundFlow->model);
//            pTableStock->stockTableView->setCurrentIndex( pFundFlow->model->index(0,0));
//        }
    });

   connect(pTableStock->blockView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, [this](int logicalIndex/*, Qt::SortOrder order*/) {
         if (m_pGlobalVar->WhichInterface!=FUNDFLOW or(m_pGlobalVar->WhichInterface==FUNDFLOW and ifCanClick==CNCLICK))
        {
            if (m_pGlobalVar->curSortNum[1]!=logicalIndex)
            {
                m_pGlobalVar->is_asc[1] = false;
                m_pGlobalVar->curSortNum[1]=logicalIndex;
            }
            else
                m_pGlobalVar->is_asc[1] = not preSort[1];

            sortByStrColumn(m_pFundFlowList,logicalIndex,m_pGlobalVar->is_asc[1]);

            preSort[1]=m_pGlobalVar->is_asc[1];
            pTableStock->m_fundFlowModel->setModelData(*m_pFundFlowList,false);
            pTableStock->blockView->setCurrentIndex(pTableStock->m_fundFlowModel->index(0,0));
            pTableStock->blockView->scrollTo(pTableStock->m_fundFlowModel->index(0,0));
        }

    });

    connect(pTableStock->risingSpeedView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, [this](int logicalIndex/*, Qt::SortOrder order*/) {
                     if (m_pGlobalVar->WhichInterface!=FUNDFLOW or(m_pGlobalVar->WhichInterface==FUNDFLOW and ifCanClick==CNCLICK))
                    {
                        if (m_pGlobalVar->curSortNum[2]!=logicalIndex)
                        {
                            m_pGlobalVar->is_asc[2] = false;
                            m_pGlobalVar->curSortNum[2]=logicalIndex;
                        }
                        else
                            m_pGlobalVar->is_asc[2] = not preSort[2];
                        sortByColumn(m_pRisingSpeedList,logicalIndex,m_pGlobalVar->is_asc[2]);
                        preSort[2]=m_pGlobalVar->is_asc[2];
                        pTableStock->m_risingSpeedModel->setModelData(*m_pRisingSpeedList,false,true,false);
                        pTableStock->risingSpeedView->setCurrentIndex(pTableStock->m_risingSpeedModel->index(0,0));
                        pTableStock->risingSpeedView->scrollTo(pTableStock->m_risingSpeedModel->index(0,0));
                    }

    });

   connect(pTableStock->myStockView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, [this](int logicalIndex/*, Qt::SortOrder order*/) {
         if (m_pGlobalVar->WhichInterface!=FUNDFLOW or(m_pGlobalVar->WhichInterface==FUNDFLOW and ifCanClick==CNCLICK))
        {
            if (m_pGlobalVar->curSortNum[3]!=logicalIndex)
            {
                m_pGlobalVar->is_asc[3] = false;
                m_pGlobalVar->curSortNum[3]=logicalIndex;
            }
            else
                m_pGlobalVar->is_asc[3] = not preSort[3];

            sortByColumn(m_pMyStockList,logicalIndex,m_pGlobalVar->is_asc[3]);
            preSort[3]=m_pGlobalVar->is_asc[3];
            pTableStock->m_myStockModel->setModelData(*m_pMyStockList,false,true,true);
            pTableStock->myStockView->setCurrentIndex(pTableStock->m_myStockModel->index(0,0));
            pTableStock->myStockView->scrollTo(pTableStock->m_myStockModel->index(0,0));
        }
    });
    // ***进行排序-- end ------

    connect(pTableStock->stockTableView,&QTableView::customContextMenuRequested,this,[=](QPoint){
        if (m_pGlobalVar->WhichInterface==CNMARKET or (m_pGlobalVar->WhichInterface==FUNDFLOW and (ifCanClick==FUNDFLOWCLICK or ifCanClick==CNCLICK)))
            addRightMenu(1);
    });
    connect(pTableStock->risingSpeedView,&QTableView::customContextMenuRequested,this,[=](QPoint){
        addRightMenu(2);
    });
    connect(pTableStock->myStockView,&QTableView::customContextMenuRequested,this,[=](QPoint){
        QMenu *menu=new QMenu(this);
        QAction *act=new QAction("删除自选",menu);
        menu->setAttribute(Qt::WA_DeleteOnClose);
        QIcon icon(":/new/png/png/del.jpg");
        act->setIcon(icon);
        menu->addAction(act);
        menu->popup(QCursor::pos());
        connect(act,&QAction::triggered,this,&MainWindow::delMyStock);
    });

    QPushButton *qButton;
    QString IndexCode[]={"1.000001","399001","399006","100.HSI","100.N225","100.KS11","1.000688","100.TWII","100.SENSEX","100.DJIA",
                           "100.SPX","100.NDX","100.SX5E","100.GDAXI","100.RTS","100.FTSE","100.FCHI","100.AS51","104.CN00Y","103.YM00Y",
                           "133.USDCNH","122.XAU"};

    for (int i = 2; i<=16; i+=2)
    {
        qButton = (QPushButton *)(ui->statusBar->children().at(i));
          connect(qButton,&QPushButton::clicked,this,[=](){

            if (not pDrawChart->hisTimeShareChart->isHidden())

                pDrawChart->hisTimeShareChart->close();
            int n;
            if (isAsia)
            {
                if ((i==8 or i==10 or i==12) and not changeInTurn)
                    n=i/2+2;
                else
                    n=i/2-1;
            }
            else
            {
                if ((i==8 or i==10 or i==12) and not changeInTurn)
                    n=i/2+11;
                else
                    n=i/2+8;
            }
            if (i==14)
            {
                if (changeInTurn)
                    n=19;
                else
                    n=18;
            }
            else if (i==16)
            {
                if (changeInTurn)
                    n=21;
                else
                    n=20;
            }

            if (n<0)
                n=0;
            if (n>21)
                n=21;

            m_pGlobalVar->curCode=IndexCode[n];
            m_pGlobalVar->curName=qButton->text();
           // resetKParameter();

            lastTime=QDateTime::currentDateTime();
            bWantReqKline=true;
            bool bFirst;
            if (m_PreCode != m_pGlobalVar->curCode )
            {
                bFirst=true;
                m_PreCode=m_pGlobalVar->curCode;
            }
            else
            {
                bFirst=false;
            }
            emit startThreadCandleChart(freq,adjustFlag,bFirst);
            emit startThreadTimeShareChart(false);
            emit startThreadTimeShareTick(false);
            if ( !m_pGlobalVar->isKState){
                toInterFace("k");
            }
        });
    }
    connect(ui->newsReport,&QAction::triggered,this,[=](){

        m_pGlobalVar->isSayNews=not m_pGlobalVar->isSayNews;
        if (m_pGlobalVar->isSayNews)
        {
            QIcon icon(":/new/png/png/report.png");
            ui->newsReport->setIcon(icon);
            ui->newsReport->setText("关闭语音播报");
            if (threadNewsReport->tts->state() == QTextToSpeech::Paused)
            {
              threadNewsReport->tts->resume();
            }
            else
               threadNewsReport->SpeechNow();
        }
        else
        {
            if (threadNewsReport->tts->state() == QTextToSpeech::Speaking)
            {
                 threadNewsReport->tts->pause();
            }
            QIcon icon(":/new/png/png/no_report.png");
            ui->newsReport->setIcon(icon);
            ui->newsReport->setText("打开语音播报");
        }
    });
    
    connect(ui->fundFlow,&QAction::triggered,this,&MainWindow::dealWithFundFlow);
    connect(pSearchStock,SIGNAL(showSearch()),this,SLOT(showSearchResult()));
    connect(ui->DLAllStockK,&QAction::triggered,this,[=](){
        pRequestsToCsv->downloadAllStockK(m_pTableListCopy);
    });
    connect(ui->DLStockInfo,&QAction::triggered,this,[=](){
        pRequestsToCsv->downStockIndexPlateInfo();
    });
    connect(F10Info[0],&QPushButton::clicked,this,[=](){
        toInterFace("f3");
    });
    connect(ui->actionF3,&QAction::triggered,this,[=](){
        toInterFace("f3");
    });
    connect(F10Info[1],&QPushButton::clicked,this,[=](){
        toInterFace("f10");
    });
    connect(ui->actionF10,&QAction::triggered,this,[=](){
        toInterFace("f10");
    });
    connect(F10Info[2],&QPushButton::clicked,this,[=](){
        F10SmallWindow->setFixedSize(1470,700);
        pF10View->dealWithBusinessAnalysis();
    });
    connect(F10Info[3],&QPushButton::clicked,this,[=](){
        F10SmallWindow->setFixedSize(1330,700);
        pF10View->dealWithAssetLiability();
    });
    connect(F10Info[4],&QPushButton::clicked,this,[=](){
        F10SmallWindow->setFixedSize(1430,700);
        pF10View->dealWithIncome();
    });
    connect(F10Info[5],&QPushButton::clicked,this,[=](){
        F10SmallWindow->setFixedSize(1430,700);
        pF10View->dealWithCashFlow();
    });
    connect(pF10View->stockInfoView,&QTableView::doubleClicked,this,[=](const QModelIndex &index){
        m_pGlobalVar->curBoard=pF10View->model->item(index.row(),3)->text();
        m_pGlobalVar->isBoard=true;
        pSearchStock->getBoardData(m_pTableList);
        if (m_pTableList->isEmpty())
            return;
        pTableStock->m_tableModel->setModelData(*m_pTableList,false,true,false);
        pTableStock->stockTableView->setModel(pTableStock->m_tableModel);
        pTableStock->stockTableView->setCurrentIndex(pTableStock->m_tableModel->index(0,0));
        toInterFace("main");
        pTableStock->risingSpeedView->show();
        pTableStock->myStockView->show();
    });
    connect(ui->pickStock,&QAction::triggered,this,[=](){
        pPickStock->PickStockInterface();
    });
    connect(pPickStock,&JSPickStock::updateTableList,this,[=](){
        pTableStock->setTableView();
    });
    connect(ui->formula,&QAction::triggered,this,[=](){

        if (!formulaMutex.tryLock())
            return;

        QDialog *formulaDes=new QDialog(this);

        connect(formulaDes,&QDialog::finished,this,[=](){
             this->formulaMutex.unlock();
        });

        formulaDes->setWindowTitle("编写公式说明");
        formulaDes->setGeometry(450, 200, 1000, 700);
        formulaDes->setAttribute(Qt::WA_DeleteOnClose);// 对话框关闭时删除
        QTextBrowser *des=new QTextBrowser(formulaDes);
        des->setMinimumHeight(380);
        des->setStyleSheet("QTextBrowser{font:bold 22px;font:bold}");
        QVBoxLayout *mainLayout =new QVBoxLayout(formulaDes);
        mainLayout->setContentsMargins(2,0,2,2);
        mainLayout->setSpacing(0);
        formulaDes->setLayout(mainLayout);
        mainLayout->addWidget(des);
        QFile file(m_pGlobalVar->currentPath+"/list/formulades.txt");
        if (file.open(QFile::ReadOnly))
        {
            QTextCodec *codec = QTextCodec::codecForName("utf-8");
            des->setText(codec->toUnicode(file.readAll()));
            int post=0;
            QString s=des->toPlainText();
            QTextCharFormat fmt;
            fmt.setForeground(QColor("red"));
            while((post=s.indexOf("(",post))!=-1)
            {
                QString t=s.mid(post-1,1);
                if (t>='A' && t<='Z')
                {
                    QTextCursor cursor = des->textCursor();
                    int p=s.indexOf(")",post)+1;
                    cursor.setPosition(post-1,QTextCursor::MoveAnchor);
                    cursor.setPosition(p,QTextCursor::KeepAnchor);
                    cursor.mergeCharFormat(fmt);
                }
                post+=1;
            }
        }
//        des->setCursor();
        QTextBrowser *d=new QTextBrowser(formulaDes);
        mainLayout->addWidget(d);
        d->insertHtml("<img src=:/new/png/png/des.jpg/>");
        formulaDes->show();
        file.close();
    });
    connect(ui->setVacation,&QAction::triggered,this,[=](){
        if (m_pGlobalVar->getVacation())
            QMessageBox::information(this,"提示", "设置成功。一年只需设置一次！", QMessageBox::Ok);
        else
            QMessageBox::information(this,"提示", "设置失败,请重试", QMessageBox::Ok);
        // if (m_pGlobalVar->settings->value("isSetVacation").toString()==QDateTime::currentDateTime().toString("yyyy"))
        //     ui->setVacation->setEnabled(false);
    });
    for (int i=0;i<9;++i)
    {
        connect( pFundFlow->checkBox[i],&QCheckBox::clicked,this,[=](){
            if ( pFundFlow->checkBox[i]->isChecked())
                pFundFlow->isShow[i]=true;
            else
                pFundFlow->isShow[i]=false;
            pFundFlow->tableChart->update();
        });
        //1: 1分钟  5: 5分钟 15: 15分钟  30: 30分钟 60: 60分钟
        //101:日 102:周 103:月
        //获取股票数据
        connect(periodAdjust[i],&QRadioButton::clicked,this,[=](){
            if (i==0)
                 freq="1";// freq="7";
            else if (i==1)
                freq="5";
            else if (i==2)
                freq="30";
            else if (i==3)
                freq="101";
            else if (i==4)
                freq="102";
            else if (i==5)
                freq="103";
            else if (i==6)
                adjustFlag="0";//股票除权
            else if (i==7)
                adjustFlag="1";//前复权
            else if (i==8)
                adjustFlag="2"; //后复权
            if (m_pGlobalVar->isKState )
            {
                bool bFirst;
                if (m_PreCode != m_pGlobalVar->curCode or qStrPreFreq!=freq or qStrPreAdjustFlag!=adjustFlag)
                {
                  bFirst=true;
                  m_PreCode=m_pGlobalVar->curCode;
                  qStrPreFreq=freq;
                  qStrPreAdjustFlag=adjustFlag;
                }
                else //if (qStrFreq=freq or qStrPreAdjustFlag=adjustFlag)
                {
                  bFirst=false;
                }
                resetKParameter();
                lastTime=QDateTime::currentDateTime();
                bWantReqKline=true;
                emit startThreadCandleChart(freq,adjustFlag,bFirst);
            }
        });
    }
    for (int i=0;i<10;++i)
    {
        connect(fundFlow[i],&QPushButton::clicked,this,&MainWindow::toFundFlow);
        connect(buySellPrice[i],&QLabel::customContextMenuRequested,this,&MainWindow::fastTrade);
    }
    connect(ui->login,&QAction::triggered,this,[=](){
        if (QMTProcess->state()==QProcess::NotRunning)
        {
           // QString str = "D:\\Program Files\\Finance\\国金证券QMT交易端\\bin.x64\\XtMiniQmt.exe";
            QString str = "XtMiniQmt.exe";
            QString tagDir = "\"" + str + "\"";
            QMTProcess->startDetached(tagDir);
        }
        else
            QMessageBox::information(this,"提示", "交易已经启动", QMessageBox::Ok);
    });
	
	  connect(dateEdit1,SIGNAL(dateChanged(QDate)),this,SLOT(updateFeeling(QDate)));
}

void MainWindow::ontableViewclicked(const QModelIndex &index)
{
   QTableView *tableView  = qobject_cast<QTableView*>(sender());
   if (tableView )
   {
      SelectedtableView=tableView;
   }
}
void MainWindow::tableViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    if (QTableView *tableView = qobject_cast<QTableView*>(sender())) {
        SelectedtableView=tableView;
    }
}

void MainWindow::ChangeKline()
{
    QAction* pAction=qobject_cast<QAction*>(sender());
    if (pAction==nullptr)
        return;
    int  type = pAction->data().value<int>();

    switch(type)
    {
        case 1:
          freq="1";
          break;
        case 2:
          freq="5";
          break;
        case 3:
          freq="15";
          break;
        case 4:
            freq="30";
            break;
        case 5:
            freq="60";
            break;
        case 6:
            freq="101";
            break;

        case 7:
            freq="102";
            break;

        case 8:
            freq="103";
            break;
        case 9:
            freq="104";
            break;

        case 10:
            freq="105";
            break;
    }
    bool bFirst=false;
    if (qStrPreFreq!=freq)
    {
        qStrPreFreq=freq;
        bFirst=true;
    }
    resetKParameter();
    lastTime=QDateTime::currentDateTime();
    bWantReqKline=true;
    emit startThreadCandleChart(freq,adjustFlag,bFirst);
}

void MainWindow::SelectTechAction()
{
     if (not m_pGlobalVar->isKState)
        return;
     
    QAction *action = (QAction*)sender();
     if (action==nullptr)
        return;
    QString actionText = action->text();
        
    if (actionText.contains("MACD"))
    {
        nTech=STT_MACD;
    }
    else if (actionText.contains("KDJ"))
    {
        nTech=STT_KDJ;
    }
    else if (actionText.contains("成交量"))
    {
        nTech=STT_VOLUME;
    }
   
     pDrawChart->candleChart->update();
    
}
    
void MainWindow::saveCode()
{
    QString strRate= QString::number(m_pGlobalVar->speechrate);
    m_pGlobalVar->settings->setValue("speechrate",strRate);

    if (m_pGlobalVar->curCode.length()!=5 and m_pGlobalVar->curCode.left(1)!="1")
    {
        m_pGlobalVar->settings->setValue("curCode",m_pGlobalVar->curCode);
//        m_pGlobalVar->settings->setValue("curName",m_pGlobalVar->curName);
        m_pGlobalVar->settings->sync();
    }
}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == pTableStock->stockTableView->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *event1 = static_cast<QWheelEvent *>(event);
        int para=event1->angleDelta().y();
        int tempStep=pTableStock->stockTableView->verticalScrollBar()->value();
        int curIndex=pTableStock->stockTableView->currentIndex().row();
        if (curIndex==-1) curIndex=0; //fixed
        int row=pTableStock->stockTableView->height()/22+1;
        if (para<0)
        {
            pTableStock->stockTableView->verticalScrollBar()->setSliderPosition(tempStep+row);
            if (curIndex>m_pTableList->count()-row)
                pTableStock->stockTableView->setCurrentIndex(pTableStock->m_tableModel->index(0,0));
            else
                pTableStock->stockTableView->setCurrentIndex(pTableStock->m_tableModel->index(curIndex+row,0));
        }
        else
        {
            pTableStock->stockTableView->verticalScrollBar()->setSliderPosition(tempStep-row);
            if (curIndex>=row)
                pTableStock->stockTableView->setCurrentIndex(pTableStock->m_tableModel->index(curIndex-row,0));
            else
                pTableStock->stockTableView->setCurrentIndex(pTableStock->m_tableModel->index(m_pTableList->count()-1,0));

        }
//        qDebug()<<pTableStock->stockTableView->height();
        return true;
    }
    else if (obj==pTableStock->blockView->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *event1 = static_cast<QWheelEvent *>(event);
        int para=event1->angleDelta().y();
        int tempStep=pTableStock->blockView->verticalScrollBar()->value();
        int curIndex=pTableStock->blockView->currentIndex().row();
         if (curIndex==-1) curIndex=0; //fixed
        int row=pTableStock->blockView->height()/22;
        if (para<0)
        {
            pTableStock->blockView->verticalScrollBar()->setSliderPosition(tempStep+row);
            if (curIndex>m_pFundFlowList->count()-row)
                pTableStock->blockView->setCurrentIndex(pTableStock->m_fundFlowModel->index(0,0));
            else
                pTableStock->blockView->setCurrentIndex(pTableStock->m_fundFlowModel->index(curIndex+row,0));
        }
        else
        {
            pTableStock->blockView->verticalScrollBar()->setSliderPosition(tempStep-row);
            if (curIndex>=row)
                pTableStock->blockView->setCurrentIndex(pTableStock->m_fundFlowModel->index(curIndex-row,0));
            else
                pTableStock->blockView->setCurrentIndex(pTableStock->m_fundFlowModel->index(m_pFundFlowList->count()-1,0));
        }
        //        qDebug()<<pTableStock->stockTableView->height();
        return true;
    }
    else if ((obj==pTableStock->myStockView or
                obj==pTableStock->stockTableView or
                obj==pTableStock->risingSpeedView or
                obj==pTableStock->blockView
                ) and event->type()==QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        QKeySequence key=keyEvent->key();
        if ((keyEvent->key() >= 48 and keyEvent->key() <= 57) or (keyEvent->key() >= 65 and keyEvent->key() <= 90))
        {
            searchSmallWindow->show();
            pSearchStock->searchCodeLine->setText(keyEvent->text());
            pSearchStock->searchCodeLine->setFocus();
            pSearchStock->matchCodeText->moveCursor(QTextCursor::Start);
            return true;
        }
        else if (key==Qt::Key_Down or key==Qt::Key_PageDown)
        {
            KeyMoveUpOrDown(1);
        }
        else if (key==Qt::Key_Up or key==Qt::Key_PageUp)
        {
            KeyMoveUpOrDown(-1);
        }
        else if ( key==Qt::Key_Tab)
        {
          //  SelectedtableView=qobject_cast<QTableView*>(sender());
            if (obj==pTableStock->stockTableView ){
              SelectedtableView=pTableStock->blockView;
              SelectedtableView->setFocus();
              return true;
            }
            else if (obj==pTableStock->blockView){
                SelectedtableView=pTableStock->risingSpeedView;
                SelectedtableView->setFocus();
                return true;
            }
            else if (obj==pTableStock->risingSpeedView){
                SelectedtableView=pTableStock->myStockView;
                SelectedtableView->setFocus();
                return true;
            }
            else if (obj==pTableStock->myStockView)
            {
                SelectedtableView=pTableStock->stockTableView;
                SelectedtableView->setFocus();
                return true;
            }
        }

    }
    else if (obj==pDrawChart->timeShareChart)
    {
        if (event->type() == QEvent::Paint )
        {
            QPainter painter(pDrawChart->timeShareChart);
            pDrawChart->drawTimeShareChart(m_pTimeShareChartList,&painter);
            return true;
        }
        else if(event->type()==QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            QPoint mPos = mouseEvent->pos();
            //        float price=m_pGlobalVar->timeShareHighLowPoint[0]-(m_pGlobalVar->timeShareHighLowPoint[0]-m_pGlobalVar->timeShareHighLowPoint[1])*(mouseEvent->pos().ry()-TOPHEIGHTEDGE)/(pDrawChart->timeShareChart->height()*12/15-2*TOPHEIGHTEDGE);
            //        timeSharePrice->setText(QString::number((1+price/100)*m_pGlobalVar->preClose,'f',2)+"("+QString::number(price,'f',2)+")%");
            int n=int( mPos.rx()*m_pGlobalVar->trendsTotal/(pDrawChart->TimeChartWidth()-2*WIDTHEDGE))-1;
            if (n<0 or n>m_pTimeShareChartList->size()-1 or mPos.ry()<TOPHEIGHTEDGE or mPos.ry()>pDrawChart->TimeChartBottom())
            {
                pDrawChart->lbltimeSharePrice->hide();
                pDrawChart->lbltimeShareVol->hide();
                pDrawChart->lbltimeShareTime->hide();
                pDrawChart->vlblLine.hide();
                pDrawChart->hlblLine.hide();
                return false;
            }
            pDrawChart->vlblLine.setStyleSheet("QLabel{border:2px dotted white;}");
            pDrawChart->vlblLine.resize(1,pDrawChart->TimeChartBottom());
            pDrawChart->hlblLine.setStyleSheet("QLabel{border:2px dotted white;}");
            pDrawChart->hlblLine.resize(pDrawChart->TimeChartWidth()-2*WIDTHEDGE,1);
            float price=m_pTimeShareChartList->at(n).price;
            float per =(price-m_pGlobalVar->preClose)*100.0f/m_pGlobalVar->preClose;
            if ( per>0)
                pDrawChart->lbltimeSharePrice->setStyleSheet("color:red;font:bold;font-size:18px");
            else if (per<0)
                pDrawChart->lbltimeSharePrice->setStyleSheet("color:rgb(0,255,0);font:bold;font-size:18px");
            else
                pDrawChart->lbltimeSharePrice->setStyleSheet("color:yellow;font:bold;font-size:18px");
            //float price=(1+p/100)*m_pGlobalVar->preClose;
            int x=(pDrawChart->TimeChartWidth()-2*WIDTHEDGE)*n/m_pGlobalVar->trendsTotal+WIDTHEDGE;
            int y=(m_pGlobalVar->timeShareHighLowPoint[0]-price)*(pDrawChart->TimeChartHeight())/(m_pGlobalVar->timeShareHighLowPoint[0]-m_pGlobalVar->timeShareHighLowPoint[1])+TOPHEIGHTEDGE;
            //        qDebug()<<x<<y;
            pDrawChart->lbltimeSharePrice->setText(QString::number(price,'f',2)+"("+QString::number(per)+"%)");
            pDrawChart->lbltimeShareVol->setText(format_conversion(m_pTimeShareChartList->at(n).vol));
            pDrawChart->lbltimeShareTime->setText(m_pTimeShareChartList->at(n).time.right(5));
            pDrawChart->lbltimeSharePrice->adjustSize();
            pDrawChart->lbltimeShareVol->adjustSize();
            pDrawChart->lbltimeSharePrice->move( mPos.rx()-( mPos.x()-WIDTHEDGE)*pDrawChart->lbltimeSharePrice->width()/(pDrawChart->TimeChartWidth()-2*WIDTHEDGE),95);
            pDrawChart->lbltimeShareVol->move( mPos.rx()-( mPos.x()-WIDTHEDGE)*pDrawChart->lbltimeShareVol->width()/(pDrawChart->TimeChartWidth()-2*WIDTHEDGE),250);
            pDrawChart->lbltimeShareTime->move( mPos.rx()-( mPos.x()-WIDTHEDGE)*pDrawChart->lbltimeShareTime->width()/(pDrawChart->TimeChartWidth()-2*WIDTHEDGE),220);
            pDrawChart->vlblLine.move(x,0);
            pDrawChart->hlblLine.move(0,y);
            pDrawChart->lbltimeSharePrice->show();
            pDrawChart->lbltimeShareVol->show();
            pDrawChart->lbltimeShareTime->show();
            pDrawChart->vlblLine.show();
            pDrawChart->hlblLine.show();
             return true;
        }
        else if (event->type()==QEvent::Leave)
        {
            pDrawChart->lbltimeSharePrice->hide();
            pDrawChart->lbltimeShareVol->hide();
            pDrawChart->lbltimeShareTime->hide();
            pDrawChart->vlblLine.hide();
            pDrawChart->hlblLine.hide();
             return true;
        }

    }
    else if (obj== pFundFlow->tableChart)
    {
        if (event->type() == QEvent::Paint)
        {
            QPainter painter( pFundFlow->tableChart);
            pFundFlow->drawIntervalHighLowChart(&painter);
            return true;
        }       
    }
    else if (obj== pFundFlow->fundFlowChart)
    {
        if (event->type() == QEvent::Paint)
        {
            QPainter painter( pFundFlow->fundFlowChart);
            painter.setRenderHint(QPainter::Antialiasing);
            pFundFlow->drawFundFlowChart(&painter);
            return true;// fixed 2024.10.7
        }
        else if(event->type()==QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            int maxNums= pFundFlow->fundFlowKList.count();
            QPoint mPos = mouseEvent->pos();
            int maxHNums= pFundFlow->fundFlowHKList.count();
            int aveHW= pFundFlow->fundFlowChart->width()/2/maxHNums;
            if (mPos.rx()>10 and
                mPos.rx()< pFundFlow->fundFlowChart->width()/2-10 and
                mPos.ry()>0 and mPos.ry()<( pFundFlow->fundFlowChart->height()-150)/2)
            {
                int n=0;
                if (maxNums!=0)
                {
                    int aveW= pFundFlow->fundFlowChart->width()/2/maxNums;
                    n=(mPos.rx()-10)/aveW;
                }
                if ((maxNums-1)>=n )
                {
                    pFundFlow->vKLine->show();
                    pFundFlow->vKLine->move(mPos.rx(),0);
                    pFundFlow->time->setText( pFundFlow->fundFlowKList.at(n)[0]);
                    if ( pFundFlow->fundFlowKList.at(n)[1].toFloat()<0)
                        pFundFlow->textFund[0]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
                    else
                        pFundFlow->textFund[0]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");

                    pFundFlow->textFund[0]->setText(format_conversion( pFundFlow->fundFlowKList.at(n)[1].toFloat()));
                    for (int i=1;i<5;++i)
                    {
                        if ( pFundFlow->fundFlowKList.at(n)[6-i].toFloat()<0)
                            pFundFlow->textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
                        else
                            pFundFlow->textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");
                        pFundFlow->textFund[i]->setText(format_conversion( pFundFlow->fundFlowKList.at(n)[6-i].toFloat()));
                    }
                }
                else
                    pFundFlow->vKLine->hide();
            }
            else if (mPos.rx()>5 and
                       mPos.rx()< pFundFlow->fundFlowChart->width()/2-15 and
                       mPos.ry()>( pFundFlow->fundFlowChart->height()+70)/2 and
                       mPos.ry()< pFundFlow->fundFlowChart->height()-40)
            {
                int n=(mPos.rx()-5)/aveHW;
                if (n>=maxHNums )
                    return false;
                pFundFlow->vKLine->show();
                pFundFlow->vKLine->move(mPos.rx(),435);
                pFundFlow->time->setText( pFundFlow->fundFlowHKList.at(n)[0]);
                if ( pFundFlow->fundFlowHKList.at(n)[1].toFloat()<0)
                    pFundFlow->textFund[0]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
                else
                    pFundFlow->textFund[0]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");
                pFundFlow->textFund[0]->setText(format_conversion( pFundFlow->fundFlowHKList.at(n)[1].toFloat()));
                for (int i=1;i<5;++i)
                {
                    if ( pFundFlow->fundFlowHKList.at(n)[6-i].toFloat()<0)
                        pFundFlow->textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
                    else
                        pFundFlow->textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");
                    pFundFlow->textFund[i]->setText(format_conversion( pFundFlow->fundFlowHKList.at(n)[6-i].toFloat()));
                }
            }
            else if (mPos.rx()> pFundFlow->fundFlowChart->width()/2 and
                       mPos.rx()< pFundFlow->fundFlowChart->width()-15 and
                       mPos.ry()>( pFundFlow->fundFlowChart->height()+70)/2 and
                       mPos.ry()< pFundFlow->fundFlowChart->height()-205)
            {
                pFundFlow->time->setText("一周5日资金流入");
                for (int i=0;i<5;++i)
                {
                    if ( pFundFlow->fiveTotal[i]<0)
                        pFundFlow->textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
                    else
                        pFundFlow->textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");

                    pFundFlow->textFund[i]->setText(format_conversion( pFundFlow->fiveTotal[i]));
                }
            }
            else if (mPos.rx()> pFundFlow->fundFlowChart->width()/2 and
                       mPos.rx()< pFundFlow->fundFlowChart->width()-15 and
                      mPos.ry()> pFundFlow->fundFlowChart->height()-205 and
                       mPos.ry()< pFundFlow->fundFlowChart->height()-40)
            {
                pFundFlow->time->setText("一月20日资金流入");
                for (int i=0;i<5;++i)
                {
                    if ( pFundFlow->twentyTotal[i]<0)
                        pFundFlow->textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
                    else
                        pFundFlow->textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");
                    pFundFlow->textFund[i]->setText(format_conversion( pFundFlow->twentyTotal[i]));
                }
            }
            else if ((mPos.rx()-760)*(mPos.rx()-760)+
                           (mPos.ry()-175)*(mPos.ry()-175)<=125*125)
            {
                float degree=90-atan2(175-mPos.ry(),760-mPos.rx())*180/PI;
                if (degree<0)
                    degree+=360;
                if (degree< pFundFlow->degree[0])
                    pFundFlow->whichPiece=1;
                else if (degree< pFundFlow->degree[1])
                    pFundFlow->whichPiece=2;
                else if (degree< pFundFlow->degree[2])
                    pFundFlow->whichPiece=3;
                else if (degree< pFundFlow->degree[3])
                    pFundFlow->whichPiece=4;
                else if (degree< pFundFlow->degree[4])
                    pFundFlow->whichPiece=5;
                else if (degree< pFundFlow->degree[5])
                    pFundFlow->whichPiece=6;
                else if (degree< pFundFlow->degree[6])
                    pFundFlow->whichPiece=7;
                else
                    pFundFlow->whichPiece=8;
                pFundFlow->fundFlowChart->update();
            }
            else
            {
                pFundFlow->whichPiece=-1;
                pFundFlow->vKLine->hide();
                pFundFlow->fundFlowChart->update();
            }
            pFundFlow->fundFlowChart->move(POSXCHART,POSYCHART);
            pFundFlow->fundFlowChart->move(POSXCHART+1,POSYCHART);
        }
        else if (event->type()==QEvent::Leave)
        {
            pFundFlow->vKLine->hide();
            pFundFlow->fundFlowChart->move(POSXCHART,POSYCHART);
            pFundFlow->fundFlowChart->move(POSXCHART+1,POSYCHART);
        }
        else if (event->type()==QEvent::KeyPress)
        {
            QKeyEvent *event1 = static_cast<QKeyEvent *>(event);
            if (event1->key()==Qt::Key_Escape)
                pFundFlow->fundFlowChart->close();
        }
        return true;
    }
    else if (obj==pDrawChart->hisTimeShareChartTitle)
    {
        if (event->type()==QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            pDrawChart->isMoveHisTimeShareChart=true;
            p=mouseEvent->pos();
        }
        else if (event->type()==QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            if (pDrawChart->isMoveHisTimeShareChart)
                pDrawChart->hisTimeShareChart->move((mouseEvent->scenePosition()-p).rx(),(mouseEvent->scenePosition()-p).ry());
        }
        else if (event->type()==QEvent::MouseButtonRelease)
        {
            pDrawChart->isMoveHisTimeShareChart=false;
        }
        return true;
    }
    else if (obj==pDrawChart->hisTimeShareChartView)
    {
        if (event->type() == QEvent::Paint)
        {
            QPainter painter(pDrawChart->hisTimeShareChartView);
            pDrawChart->drawHisTimeShare(m_pHisTimeShareChartList,&painter);
            return true;// fixed 2024.10.7
        }
        else if (event->type()==QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            QPoint mPos = mouseEvent->pos();
            int nHisTimeSize = m_pHisTimeShareChartList->size();
            int n=int(mPos.rx()* nHisTimeSize/(pDrawChart->HisTimeChartWidth()));
            if (n<0 or n> nHisTimeSize -1 or
                mPos.ry()<TOPHEIGHTEDGE or
                mPos.ry()>pDrawChart->hisTimeShareChartView->height()-BOTTOMHEIGHTEDGE)
            {
                pDrawChart->lblhisTimeSharePrice->hide();
                pDrawChart->lblhisTimeShareVol->hide();
                pDrawChart->lblhisTimeShareTime->hide();
                pDrawChart->vlblHisLine.hide();
                pDrawChart->hlblHisLine.hide();
                return false;
            }
            pDrawChart->vlblHisLine.setStyleSheet("QLabel{border:2px dotted white;}");
            pDrawChart->vlblHisLine.resize(1,pDrawChart->HisTimeChartBottom());
            pDrawChart->hlblHisLine.setStyleSheet("QLabel{border:2px dotted white;}");
            pDrawChart->hlblHisLine.resize(pDrawChart->HisTimeChartWidth(),1);
            float price=m_pHisTimeShareChartList->at(n).price;

            if (price>m_pGlobalVar->hisPreClose)
                pDrawChart->lblhisTimeSharePrice->setStyleSheet("color:red;font:bold;font-size:18px");
            else if (price<m_pGlobalVar->hisPreClose)
                pDrawChart->lblhisTimeSharePrice->setStyleSheet("color:rgb(0,255,0);font:bold;font-size:18px");
            else
                pDrawChart->lblhisTimeSharePrice->setStyleSheet("color:yellow;font:bold;font-size:18px");
            float p=(price-m_pGlobalVar->hisPreClose)*100/m_pGlobalVar->hisPreClose;
            int x=pDrawChart->HisTimeChartWidth()*n/m_pHisTimeShareChartList->size()+WIDTHEDGE;
            int y=pDrawChart->YTransferHisTimeChart(price);//pDrawChart->hisTimeShareHighLowPoint[0]-price)*(pDrawChart->hisTimeShareChartView->height()*12/15-2*TOPHEIGHTEDGE)/(pDrawChart->hisTimeShareHighLowPoint[0]-pDrawChart->hisTimeShareHighLowPoint[1])+TOPHEIGHTEDGE;
//            qDebug()<<x<<y;
            pDrawChart->lblhisTimeSharePrice->setText(QString::number(price,'f',2)+"("+QString::number(p,'f',2)+"%)");
            pDrawChart->lblhisTimeShareVol->setText(format_conversion(m_pHisTimeShareChartList->at(n).vol));
            pDrawChart->lblhisTimeShareTime->setText(m_pHisTimeShareChartList->at(n).time.right(5));
            pDrawChart->lblhisTimeSharePrice->adjustSize();
            pDrawChart->lblhisTimeShareVol->adjustSize();
            pDrawChart->lblhisTimeSharePrice->move(mPos.rx()-(mPos.x()-WIDTHEDGE)*pDrawChart->lblhisTimeSharePrice->width()/(pDrawChart->HisTimeChartWidth()),100);
            pDrawChart->lblhisTimeShareVol->move(mPos.rx()-(mPos.x()-WIDTHEDGE)*pDrawChart->lblhisTimeShareVol->width()/(pDrawChart->HisTimeChartWidth()),340);
            pDrawChart->lblhisTimeShareTime->move(mPos.rx()-(mPos.x()-WIDTHEDGE)*pDrawChart->lblhisTimeShareTime->width()/(pDrawChart->HisTimeChartWidth()),310);
            pDrawChart->vlblHisLine.move(x,0);
            pDrawChart->hlblHisLine.move(0,y);
            pDrawChart->lblhisTimeSharePrice->show();
            pDrawChart->lblhisTimeShareVol->show();
            pDrawChart->lblhisTimeShareTime->show();
            pDrawChart->vlblHisLine.show();
            pDrawChart->hlblHisLine.show();
            pDrawChart->hisTimeShareChartView->update();
        }
        else if (event->type() == QEvent::Wheel)
        {
            QWheelEvent *ev = static_cast<QWheelEvent *>(event);
            if (ev->angleDelta().y()<0)
            {
                hisTimeShareN+=1;
                if (hisTimeShareN>(int)m_pKline->size()-1)
                {
                    hisTimeShareN=(int)m_pKline->size()-1;
                    return true;
                }
            }
            else
            {
                hisTimeShareN-=1;
                if (hisTimeShareN<0)
                {
                    hisTimeShareN=0;
                    return true;
                }
            }
           // QString date=m_pKline->at(hisTimeShareN).date;
            QString date=getKlineDate(m_pKline->at(hisTimeShareN).date);
            pDrawChart->lblrowTime->setText(date);
            pFundFlow->getTimeShareMin(m_pHisTimeShareChartList,m_pGlobalVar->getStockSymbol(),date);
            pDrawChart->title->setText(m_pGlobalVar->curName.left(m_pGlobalVar->curName.indexOf("("))+" "+date+"分时图");
            pDrawChart->hisTimeShareChartView->update();
        }
        else if (event->type()==QEvent::Leave)
        {
            pDrawChart->lblhisTimeSharePrice->hide();
            pDrawChart->lblhisTimeShareVol->hide();
            pDrawChart->lblhisTimeShareTime->hide();
            pDrawChart->vlblHisLine.hide();
            pDrawChart->hlblHisLine.hide();
        }
        return true;
    }
    else if (obj==pDrawChart->candleChart && (not bWantReqKline )) //and freq=="1" ) || (freq>"1") ))
    {
        if (event->type()==QEvent::MouseMove)
        {
            isFlashBaseInfo=false;
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            flashOldCandleInfo(mouseEvent);    
            return true;
        }
        else if (event->type()==QEvent::MouseButtonDblClick)
        {
            if (m_pGlobalVar->WhichInterface==HKMARKET or m_pGlobalVar->WhichInterface==USMARKET or
                m_pGlobalVar->curCode.left(2)=="10" or m_pGlobalVar->WhichInterface==UKMARKET or
                m_pGlobalVar->curCode.left(2)=="13" or m_pGlobalVar->curCode.left(2)=="12")
            {
               // QMessageBox::information(this,"提示", "只能查看A股", QMessageBox::Ok);
                return false; //只能查看A股分时图
            }
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            if (mouseEvent->pos().y()<KWIDTHEDGE)
                return false;
            int nKdCount= (int)m_pKline->size();

            int m = pDrawChart->XTransferCandleChart(mouseEvent->pos().x()-KWIDTHEDGE);
            if (nKdCount<m_pGlobalVar->KRange)
                hisTimeShareN=m;
            else
                hisTimeShareN=nKdCount-m_pGlobalVar->offsetLocal+m;
            if (hisTimeShareN>nKdCount-1 or hisTimeShareN<0)
                return true;
            //QString date=m_pKline->at(hisTimeShareN).time;
            QString date=getKlineDate(m_pKline->at(hisTimeShareN).date);
            pDrawChart->lblrowTime->setText(date);
            if (freq=="1" or freq=="7" or freq=="5" or freq=="15" or freq=="30" or freq=="60")
            {
                QStringList list=date.split(" ");
                date=list[0];
            }

            pFundFlow->getTimeShareMin(m_pHisTimeShareChartList,m_pGlobalVar->getStockSymbol(),date);
            pDrawChart->hisTimeShareChart->show();
            pDrawChart->title->setText(m_pGlobalVar->curName.left(m_pGlobalVar->curName.indexOf("("))+" "+date+"分时图");
            pDrawChart->hisTimeShareChartView->update();
             return true;
        }
        else if (event->type()==QEvent::Paint)
        {
            QPainter painter(pDrawChart->candleChart);
            pDrawChart->drawCandleChart(freq,nTech,bNewCalc,m_pKline,&painter);
            bNewCalc= false;
            return true; // fixed 2024.10.7
        }
        else if (event->type() == QEvent::Wheel)
        {
            QWheelEvent *ev = static_cast<QWheelEvent *>(event);
            if (pDrawChart->hisTimeShareChart->isHidden()){
                downUpLookStock(ev);
                 return true;
            }
            else
            {
                if (ev->angleDelta().y()<0)
                {
                    hisTimeShareN+=1;
                    int nKdCount= (int)m_pKline->size();
                    if (hisTimeShareN>nKdCount-1)
                    {
                        hisTimeShareN=nKdCount-1;
                       // pDrawChart->hisTimeShareChartView->update();
                        return true;
                    }
                }
                else
                {
                    hisTimeShareN-=1;
                    if (hisTimeShareN<0)
                    {
                        hisTimeShareN=0;
                        //return true;
                    }
                }
                //QString date=m_pKline->at(hisTimeShareN).time;
                QString qStrDate=getKlineDate(m_pKline->at(hisTimeShareN).date);
                pDrawChart->lblrowTime->setText(qStrDate);

                pFundFlow->getTimeShareMin(m_pHisTimeShareChartList,m_pGlobalVar->getStockSymbol(),qStrDate);
                if (m_pGlobalVar->curName.indexOf("(")==-1)
                    pDrawChart->title->setText(m_pGlobalVar->curName+" "+qStrDate+"分时图");
                else
                    pDrawChart->title->setText(m_pGlobalVar->curName.left(m_pGlobalVar->curName.indexOf("("))+" "+qStrDate+"分时图");
                pDrawChart->hisTimeShareChartView->update();
                 return true;
            }
        }
        else if (event->type()==QEvent::Leave)
        {
            pDrawChart->lblcolPrice->hide();
            pDrawChart->lblrowTime->hide();
            pDrawChart->vlblKLine->hide();
            pDrawChart->hlblKLine->hide();
            isFlashBaseInfo=true;
            reFlashBuySellBaseInfo();
             return true;
        }
        else if (event->type()==QEvent::ContextMenu)
        {
            QMenu *menu=new QMenu(pDrawChart->candleChart);
            menu->setAttribute(Qt::WA_DeleteOnClose);
            QAction *moveOne=new QAction("左右移动一格",menu);
            QAction *moveFast=new QAction("左右快速移动",menu);
            QIcon icon(":/new/png/png/step.jpeg");
            moveOne->setIcon(icon);
            QIcon icon1(":/new/png/png/fast.jpg");
            moveFast->setIcon(icon1);
            menu->addAction(moveFast);
            menu->addAction(moveOne);


            // 添加分隔线
            menu->addSeparator();
            QIcon icon11(":/new/png/png/kline.png");

            menu->addAction(icon11,"MACD",this,SLOT(SelectTechAction()));
            menu->addAction(icon11,"KDJ",this,SLOT(SelectTechAction()));        
            menu->addAction(icon11,"成交量",this,SLOT(SelectTechAction()));
            
            menu->popup(QCursor::pos());
            connect(moveOne,&QAction::triggered,this,[=](){
                pDrawChart->moveUnit=1;
            });
            connect(moveFast,&QAction::triggered,this,[=](){
                pDrawChart->moveUnit=m_pGlobalVar->KRange*0.2;
            });
             return true;
        }

    }
    else if (obj==newsData->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *event1 = static_cast<QWheelEvent *>(event);
        int para=event1->angleDelta().y();
//        int maxStep=newsData->verticalScrollBar()->maximum();
        int tempStep=newsData->verticalScrollBar()->value();
//        qDebug()<<para<<tempStep;
        if (para<0/* and tempStep+6<maxStep*/)
            newsData->verticalScrollBar()->setSliderPosition(tempStep+136);
        else if (para>0/* and tempStep>6*/)
            newsData->verticalScrollBar()->setSliderPosition(tempStep-136);
      //  return true;
    }
    else if (obj==pTableStock->timeShareTickView->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *ev = static_cast<QWheelEvent *>(event);
        int para=ev->angleDelta().y();
        int tempStep=pTableStock->timeShareTickView->verticalScrollBar()->value();
        if (para<0)
            pTableStock->timeShareTickView->verticalScrollBar()->setSliderPosition(tempStep+13);
        else
            pTableStock->timeShareTickView->verticalScrollBar()->setSliderPosition(tempStep-13);
        //return true;
    }
    else if (obj==pSearchStock->searchCodeLine and event->type() ==QKeyEvent::KeyPress)
    {
        QKeyEvent *event1 = static_cast<QKeyEvent *>(event);
        if (event1->key()==Qt::Key_Down)
        {
            m_pGlobalVar->curBlock+=1;
            if (m_pGlobalVar->curBlock>pSearchStock->matchCodeText->document()->blockCount()-1)
                m_pGlobalVar->curBlock=0;
        }
        else if (event1->key()==Qt::Key_Up)
        {
            m_pGlobalVar->curBlock-=1;
            if (m_pGlobalVar->curBlock<0)
                m_pGlobalVar->curBlock=pSearchStock->matchCodeText->document()->blockCount()-1;
        }
        QTextBlockFormat fmt=QTextBlockFormat();
        QTextCharFormat charFmt=QTextCharFormat();
        fmt.setBackground(QColor("white"));
        pSearchStock->matchCodeText->textCursor().setBlockFormat(fmt);
        pSearchStock->matchCodeText->textCursor().deletePreviousChar();
        pSearchStock->matchCodeText->textCursor().deletePreviousChar();
        pSearchStock->matchCodeText->textCursor().deletePreviousChar();
        pSearchStock->matchCodeText->textCursor().deletePreviousChar();
//        qDebug()<<m_pGlobalVar->curBlock;
        QTextBlock block=pSearchStock->matchCodeText->document()->findBlockByNumber(m_pGlobalVar->curBlock);
        pSearchStock->matchCodeText->setTextCursor(QTextCursor(block));
        fmt=pSearchStock->matchCodeText->textCursor().blockFormat();

        fmt.setBackground(QColor(0, 199, 255));
        pSearchStock->matchCodeText->textCursor().setBlockFormat(fmt);
        charFmt.setForeground(QColor("red"));
        pSearchStock->matchCodeText->mergeCurrentCharFormat(charFmt);
        pSearchStock->matchCodeText->textCursor().insertText(">>> ");
    }
    else
    {
        if (event->type()==QEvent::MouseButtonPress or event->type()==QEvent::Leave or event->type()==QEvent::Leave)
        {
          for (int i=0;i<10;++i)
          {
            if (obj==buySellPrice[i] and (m_pGlobalVar->WhichInterface==CNMARKET or m_pGlobalVar->WhichInterface==FUNDFLOW) and m_pGlobalVar->curCode.left(1)!="1")
            {
                if (event->type()==QEvent::MouseButtonPress)
                    tradePrice=buySellPrice[i]->text().toFloat();
                else if (event->type()==QEvent::MouseMove and !buySellPrice[i]->text().isEmpty())
                    buySellPrice[i]->setStyleSheet("border-width:1px;border-style:solid;border-color:rgb(255,170,0);font:14px;font-family:微软雅黑;color:blue");
                else if (event->type()==QEvent::Leave)
                {
                    buySellPrice[i]->setStyleSheet("border-width: 0px;font:16px;font-family:微软雅黑");
                    float price=buySellPrice[i]->text().toFloat();
                    if (price>m_pGlobalVar->preClose)
                        buySellPrice[i]->setPalette(m_pGlobalVar->pRed);
                    else if (price<m_pGlobalVar->preClose)
                        buySellPrice[i]->setPalette(m_pGlobalVar->pGreen);
                    else
                        buySellPrice[i]->setPalette(m_pGlobalVar->pBlack);
                }
            }
          }
        }
        if (event->type()==QEvent::MouseButtonDblClick)
        {
            for (int i=0;i<50;++i)
            {
            // if (obj==pDrawChart->annLabel[i] and event->type()==QEvent::MouseButtonDblClick)
              if (obj==pDrawChart->annLabel[i])
              {
                QString date=pDrawChart->annLabel[i]->toolTip().left(11);
                int curItem=0;
                QStringList l;
                bool f=true;
                int nCount =(int)m_pGlobalVar->annoucementList.count()-1;
                for(int j = 0; j < nCount; j++)
                {
                    if (f and date==m_pGlobalVar->annoucementList.at(j)[2].left(11))
                    {
                        curItem=j;
                        f=false;
                    }
                    l<<m_pGlobalVar->annoucementList.at(j)[2]+m_pGlobalVar->annoucementList.at(j)[1]+
                            "\n"+m_pGlobalVar->annoucementList.at(j)[0];
                }
                pDrawChart->model->setStringList(l);
                pDrawChart->annTitle->setModel(pDrawChart->model);
                QModelIndex qindex = pDrawChart->model->index(curItem,0);
                pDrawChart->annTitle->setCurrentIndex(qindex);
                pDrawChart->annoucementWindow->setWindowTitle(m_pGlobalVar->curCode+" "+
                            m_pGlobalVar->curName.left(m_pGlobalVar->curName.indexOf("(")));
                pDrawChart->annClicked(qindex);
                pDrawChart->annoucementWindow->show();
                return true;
              }
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->pos().ry()<250 and event->button()==Qt::RightButton){
        if (m_pGlobalVar->WhichInterface==CNMARKET or (m_pGlobalVar->WhichInterface==FUNDFLOW and m_pGlobalVar->isKState))
            addRightMenu(3);
    }
    else
        QMainWindow::mousePressEvent(event);
}

void MainWindow::onTabFocusChanged() {
    QTableView *tableView = qobject_cast<QTableView*>(sender());
    if (tableView) {
         SelectedtableView=tableView;
    }
}

void MainWindow::KeyMoveUpOrDown(short incIdx)
{
    if (SelectedtableView!=nullptr )
    {
        if (SelectedtableView == pTableStock->stockTableView) {
            int row=pTableStock->stockTableView->currentIndex().row()+incIdx;
            if (row<0 or row >=m_pTableList->size())  //fixed
                return;

            m_pGlobalVar->curCode=m_pTableList->at(row).code;


        } else if (SelectedtableView== pTableStock->blockView) {
            int curRow=pTableStock->blockView->currentIndex().row()+incIdx;
            if (curRow<0 or curRow >=m_pFundFlowList->count())  //fixed
                return;

            QAbstractItemModel *model =SelectedtableView->model();
            if (!model)
                return;
            int column_count = model->columnCount();
            m_pGlobalVar->curBoard =model->data(model->index(curRow, column_count-1)).toString();


            return ;

        }
        else if (SelectedtableView== pTableStock->risingSpeedView) {
            int curRow=pTableStock->risingSpeedView->currentIndex().row()+incIdx;
            if (curRow<0 or curRow >=m_pRisingSpeedList->size())  //fixed
                return;
            QAbstractItemModel *model =SelectedtableView->model();
            if (!model)
                return;
              m_pGlobalVar->curCode =model->data(model->index(curRow, 0)).toString();

        } else if (SelectedtableView == pTableStock->myStockView) {
            int curRow=pTableStock->myStockView->currentIndex().row()+incIdx;
            if (curRow>m_pMyStockList->count()-1)
                curRow=0;
            QAbstractItemModel *model =SelectedtableView->model();
            if (!model)
                return;

            m_pGlobalVar->curCode =model->data(model->index(curRow, 0)).toString();
        }
        emit startThreadTimeShareChart(false);
        emit startThreadTimeShareTick(false);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{

    QKeySequence key=event->key();

    if (key==Qt::Key_Escape)
    {
        if(event->isAutoRepeat()){
            return;
         }

        if (m_pGlobalVar->isKState)
        {
            m_PreCode="";
            qStrPreFreq="";
            qStrPreAdjustFlag="";
            m_pKline->Clear();

            if (not pDrawChart->annoucementWindow->isHidden())
                pDrawChart->annoucementWindow->close();
            else if (not pDrawChart->hisTimeShareChart->isHidden())
                pDrawChart->hisTimeShareChart->close();
            else
            {
                m_pGlobalVar->isKState=false;
                isTraversalMyStock=false;
                if (m_pGlobalVar->WhichInterface==CNMARKET)
                    middleWindow->show();
                else if(m_pGlobalVar->WhichInterface==FUNDFLOW)
                {
                    rightFundWindow->show();
                    rightBaseWindow->hide();
                }

                pDrawChart->candleChart->hide();
                pTableStock->stockTableView->show();
                //resetKParameter();
            }

            if (SelectedtableView!=nullptr)
            {
                SelectedtableView->setFocus();
            }
        }
        else if (m_pGlobalVar->WhichInterface==CNMARKET or m_pGlobalVar->WhichInterface==FUNDFLOW)
        {
            pFundFlow->fundFlowChart->close();
            if(m_pGlobalVar->isBoard)
            {
                m_pGlobalVar->WhichInterface=CNMARKET; //  市场标志
                isAsia=true;
                ifCanClick=CNCLICK;
                middleWindow->show();
                ui->USzMarket->setChecked(false);
                m_pGlobalVar->curCode=m_pGlobalVar->settings->value("curCode").toString();
                m_pGlobalVar->curName=m_pGlobalVar->settings->value("curName").toString();

                m_pGlobalVar->isBoard=false;
                pTableStock->stockTableView->clearSpans();
                resetKParameter();
                toInterFace("main");
                emit startThreadTable(true);
                emit startThreadTimeShareChart(false);
                emit startThreadTimeShareTick(false);
                emit startThreadIndex(false);
            }
        }
    }
    else if ( key==Qt::Key_Tab)
    {
        QTableView *tableView = qobject_cast<QTableView*>(sender());
       if (tableView)
       {
         SelectedtableView=tableView;
         SelectedtableView->setFocus();
       }
    }
    else if (key==Qt::Key_PageDown)
    {
        if (isThreadRunning  )
            return;
        isThreadRunning=true;
        pDrawChart->hisTimeShareChart->close();
        pDrawChart->annoucementWindow->close();
        if (m_pGlobalVar->isKState)
        {     
            resetKParameter();
            if (ifCanClick==FUNDFLOWCLICK)
            {
                int curRow=pTableStock->stockTableView->currentIndex().row()+1;
                if (curRow> pFundFlow->model->rowCount()-1)
                    curRow=0;
                m_pGlobalVar->curCode= pFundFlow->model->item(curRow,0)->text();
                pTableStock->stockTableView->setCurrentIndex( pFundFlow->model->index(curRow,0));
            }
            else
            {
                if (isTraversalMyStock)
                {
                    int curRow=pTableStock->myStockView->currentIndex().row()+1;
                    if (curRow>m_pMyStockList->count()-1)
                        curRow=0;
                    m_pGlobalVar->curCode=m_pMyStockList->at(curRow).code;
                    pTableStock->myStockView->setCurrentIndex(pTableStock->m_myStockModel->index(curRow,0));
                }
                else
                {
                    int curRow=pTableStock->stockTableView->currentIndex().row()+1;
                    if (curRow>m_pTableList->count()-1)
                        curRow=0;
                    m_pGlobalVar->curCode=m_pTableList->at(curRow).code;
                    pTableStock->stockTableView->setCurrentIndex(pTableStock->m_tableModel->index(curRow,0));
                }
            }

            lastTime=QDateTime::currentDateTime();
            bWantReqKline=true;
            emit startThreadCandleChart(freq,adjustFlag,true);
            emit startThreadTimeShareChart(false);
            emit startThreadTimeShareTick(false);
            pFundFlow->initAllNews();
           // pDrawChart->candleChart->update();
        }

        isThreadRunning=false;
    }
    else if (key==Qt::Key_PageUp)
    {
        if (isThreadRunning  )
            return;
        isThreadRunning=true;
        pDrawChart->hisTimeShareChart->close();
        pDrawChart->annoucementWindow->close();
        if (m_pGlobalVar->isKState)
        {
            resetKParameter();
            if (ifCanClick==FUNDFLOWCLICK)
            {
                int curRow=pTableStock->stockTableView->currentIndex().row()-1;
                if (curRow<0)
                    curRow= pFundFlow->model->rowCount()-1;
                m_pGlobalVar->curCode= pFundFlow->model->item(curRow,0)->text();
                pTableStock->stockTableView->setCurrentIndex( pFundFlow->model->index(curRow,0));
            }
            else
            {
                if (isTraversalMyStock)
                {
                    int curRow=pTableStock->myStockView->currentIndex().row()-1;
                    if (curRow<0)
                        curRow=m_pMyStockList->count()-1;
                    m_pGlobalVar->curCode=m_pMyStockList->at(curRow).code;
                    pTableStock->myStockView->setCurrentIndex(pTableStock->m_myStockModel->index(curRow,0));
                }
                else
                {
                    int curRow=pTableStock->stockTableView->currentIndex().row()-1;
                    if (curRow<0)
                        curRow=m_pTableList->count()-1;
                    m_pGlobalVar->curCode=m_pTableList->at(curRow).code;
                    pTableStock->stockTableView->setCurrentIndex(pTableStock->m_tableModel->index(curRow,0));
                }
            }

            lastTime=QDateTime::currentDateTime();
            bWantReqKline=true;
            emit startThreadCandleChart(freq,adjustFlag,true);
            emit startThreadTimeShareChart(false);
            emit startThreadTimeShareTick(false);
            pFundFlow->initAllNews();
            //pDrawChart->candleChart->update();
        }

        isThreadRunning=false;
    }
    else if (key==Qt::Key_Enter or key==Qt::Key_Return)
    {

        if (m_pGlobalVar->isKState or m_pGlobalVar->WhichInterface==FUNDFLOW  )
            return;
        if ( SelectedtableView!=nullptr and SelectedtableView==pTableStock->blockView )
        {
          pTableStock->isFlashTable=false;
          m_pGlobalVar->isBoard=true;
          //m_pGlobalVar->curBoard=m_pGlobalVar->mFundFlowList.at(curRow)[13];
          pSearchStock->getBoardData(m_pTableList);
          if (not m_pTableList->isEmpty())
              pTableStock->m_tableModel->setModelData(*m_pTableList,false,true,false);
           pTableStock->isFlashTable=true;
          return;
        }
        m_pGlobalVar->isKState=true;

        lastTime=QDateTime::currentDateTime();
        bWantReqKline=true;
        emit startThreadCandleChart(freq,adjustFlag,true);
        toInterFace("k"); // fixed 2024.10.7
    }
    else if ((key >= 48 and key <= 57) or (key >= 65 and key <= 90))
    {
//        qDebug()<<event->text();
//        searchCode+=event->text();
        //键盘精灵，只有A股可以输入选择
        //if (m_pGlobalVar->WhichInterface==HKMARKET or m_pGlobalVar->WhichInterface==USMARKET or m_pGlobalVar->WhichInterface==UKMARKET)
        //    return;

        searchSmallWindow->show();
        pSearchStock->searchCodeLine->setText(event->text());
        pSearchStock->searchCodeLine->setFocus();
        pSearchStock->matchCodeText->moveCursor(QTextCursor::Start);
    }
    else if (key==Qt::Key_Up)
    {
        if (m_pGlobalVar->isKState)
        {
            int value=m_pGlobalVar->KRange;
            m_pGlobalVar->KRange= value *0.8f;

            if (m_pGlobalVar->KRange<50)
            {
                m_pGlobalVar->KRange=50;
              //  return; // fixed 2024.10.8
            }
            m_pGlobalVar->offsetLocal = m_pGlobalVar->offsetLocal - value + m_pGlobalVar->KRange;
            if ( m_pGlobalVar->offsetLocal<0)
                m_pGlobalVar->offsetLocal=0;
//            if (m_pGlobalVar->offsetLocal<m_pGlobalVar->KRange)
//                m_pGlobalVar->offsetLocal=m_pGlobalVar->KRange;

            pDrawChart->candleChart->update();
        }

    }
    else if (key==Qt::Key_Down)
    {

        if (m_pGlobalVar->isKState)
        {

            m_pGlobalVar->offsetLocal=m_pGlobalVar->offsetLocal+m_pGlobalVar->KRange*0.1f;

            m_pGlobalVar->KRange=m_pGlobalVar->KRange*1.2f;
            int nKdCount =(int)m_pKline->size();
            if (nKdCount<m_pGlobalVar->offsetLocal)
                m_pGlobalVar->offsetLocal=nKdCount;

            if (m_pGlobalVar->KRange>m_pGlobalVar->offsetLocal)
                m_pGlobalVar->KRange=m_pGlobalVar->offsetLocal;

            //if (preCode!=m_pGlobalVar->curCode)
            //{
            //    preCode=m_pGlobalVar->curCode;
           //     emit startThreadCandleChart(freq,adjustFlag,false);
           // }
            pDrawChart->candleChart->update();
        }

    }
    else if (key==Qt::Key_Left)
    {
        if (m_pGlobalVar->isKState)
        {
            m_pGlobalVar->offsetLocal=m_pGlobalVar->offsetLocal+pDrawChart->moveUnit;
             int nKdCount= (int)m_pKline->size();
            if (nKdCount<m_pGlobalVar->offsetLocal)
                m_pGlobalVar->offsetLocal=nKdCount;
           // if (preCode!=m_pGlobalVar->curCode)
           // {
           //     preCode=m_pGlobalVar->curCode;
           //     emit startThreadCandleChart(freq,adjustFlag,false);
           // }
            pDrawChart->candleChart->update();
        }
    }
    else if (key==Qt::Key_Right)
    {
       if (m_pGlobalVar->isKState)
        {
            m_pGlobalVar->offsetLocal=m_pGlobalVar->offsetLocal-pDrawChart->moveUnit;
            if (m_pGlobalVar->offsetLocal<m_pGlobalVar->KRange)
                m_pGlobalVar->offsetLocal=m_pGlobalVar->KRange;
            //m_pGlobalVar->offsetEnd=0;
            pDrawChart->candleChart->update();
//            m_pGlobalVar->offsetEnd=OFFSET;
        }
    }
    else if (key==Qt::Key_F3 and !event->isAutoRepeat())
        toInterFace("f3");
    else if (key==Qt::Key_F10  and !event->isAutoRepeat())
        toInterFace("f10");
    else if (key==Qt::Key_Delete and pTableStock->myStockView->hasFocus()  and !event->isAutoRepeat())
        delMyStock();
    QMainWindow::keyPressEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
  
    if (not m_pGlobalVar->isKState)
    {
         downUpLookStock(event);
    }
   else
     QMainWindow::wheelEvent(event);
}

void MainWindow::setMarket()
{
    timeCount=-3;
    QObject* obj = sender();
    if (obj->objectName()=="ZHMarket")
    {
        m_pGlobalVar->WhichInterface=CNMARKET; //  市场标志
        isAsia=true;
        ifCanClick=CNCLICK;
        middleWindow->show();
//        ui->HKMarket->setChecked(false);
//        ui->USMarket->setChecked(false);
//        ui->ZHMarket->setChecked(true);
        ui->USzMarket->setChecked(false);
        m_pGlobalVar->curCode=m_pGlobalVar->settings->value("curCode").toString();
        m_pGlobalVar->curName=m_pGlobalVar->settings->value("curName").toString();
    }
    else if (obj->objectName()=="HKMarket")
    {
        saveCode();
        ifCanClick=OTHERCLICK;
        m_pGlobalVar->WhichInterface=HKMARKET;
        m_pGlobalVar->curSortNum[0]=0;
        isAsia=true;
        middleWindow->hide();
//        ui->HKMarket->setChecked(true);
//        ui->USMarket->setChecked(false);
//        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(false);
        m_pGlobalVar->curCode="00700";
        m_pGlobalVar->curName="腾讯控股";
    }
    else if (obj->objectName()=="USMarket")
    {
        saveCode();
        ifCanClick=OTHERCLICK;
        m_pGlobalVar->WhichInterface=USMARKET;
        m_pGlobalVar->curSortNum[0]=6;
        isAsia=false;
        m_pGlobalVar->isUsZhStock=false;
        middleWindow->hide();
//        ui->HKMarket->setChecked(false);
//        ui->USMarket->setChecked(true);
//        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(false);
        m_pGlobalVar->curCode="105.NVDA";
        m_pGlobalVar->curName="英伟达";
    }
    else if (obj->objectName()=="USzMarket")
    {
        saveCode();
        ifCanClick=OTHERCLICK;
        m_pGlobalVar->WhichInterface=USMARKET;
        m_pGlobalVar->curSortNum[0]=6;
        isAsia=false;
        m_pGlobalVar->isUsZhStock=true;
        middleWindow->hide();
//        ui->HKMarket->setChecked(false);
        ui->USMarket->setChecked(true);
//        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(true);
        m_pGlobalVar->curCode="106.BABA";
        m_pGlobalVar->curName="阿里巴巴";
    }
    else if (obj->objectName()=="UKMarket")
    {
        saveCode();
        ifCanClick=OTHERCLICK;
        m_pGlobalVar->WhichInterface=UKMARKET;
        m_pGlobalVar->curSortNum[0]=6;
        isAsia=false;
        middleWindow->hide();
        //        ui->HKMarket->setChecked(true);
        //        ui->USMarket->setChecked(false);
        //        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(false);
        m_pGlobalVar->curCode="155.SHEL";
        m_pGlobalVar->curName="壳牌";
    }
    m_pGlobalVar->isBoard=false;
    pTableStock->stockTableView->clearSpans();
    if (m_pGlobalVar->isKState)
    {
        m_PreCode="";
        qStrPreFreq="";
        qStrPreAdjustFlag="";
        m_pKline->Clear();
    }
    resetKParameter();
    toInterFace("main");
    emit startThreadTable(true);
    emit startThreadTimeShareChart(false);
    emit startThreadTimeShareTick(false);
    emit startThreadIndex(false);
}
void MainWindow::addRightMenu(int num)
{
    QMenu *menu=new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    
    QAction *act=new QAction("加入自选",menu);
    QIcon icon(":/new/png/png/join.jpg");
    act->setIcon(icon);
    menu->addAction(act);
    
     if (m_pGlobalVar->isKState and num==3)
     {
        // 添加分隔线
        menu->addSeparator();
        QAction *act1=new QAction("15分钟K线",menu);
        QIcon icon1(":/new/png/png/kline.png");
        act1->setIcon(icon1);
        menu->addAction(act1);
        QAction *act2=new QAction("60分钟K线",menu);
        act2->setIcon(icon1);
        menu->addAction(act2);
        menu->addSeparator();
        QAction *act3=new QAction("季K线",menu);
        act3->setIcon(icon1);
        menu->addAction(act3);
        QAction *act4=new QAction("年K线",menu);
        act4->setIcon(icon1);
        menu->addAction(act4);

        act1->setData( QVariant::fromValue(3) ) ;
        act2->setData( QVariant::fromValue(5) ) ;
        act3->setData( QVariant::fromValue(9) ) ;
        act4->setData( QVariant::fromValue(10) ) ;

        connect(act1, &QAction::triggered, this, &MainWindow::ChangeKline);
        connect(act2, &QAction::triggered, this, &MainWindow::ChangeKline);
        connect(act3, &QAction::triggered, this, &MainWindow::ChangeKline);
        connect(act4, &QAction::triggered, this, &MainWindow::ChangeKline);
     }
    
    menu->popup(QCursor::pos());
    StockInfo info;
    QString code;
    if (num==1)
    {
        if (ifCanClick==1)
            info=m_pTableList->at(pTableStock->stockTableView->currentIndex().row());
        else if (ifCanClick==FUNDFLOWCLICK)
        {
            code= pFundFlow->model->item(pTableStock->stockTableView->currentIndex().row(),0)->text();
            info=findStock(m_pTableListCopy,code);
        }
        else
            return;
    }
    else if(num==2)
        info=m_pRisingSpeedList->at(pTableStock->risingSpeedView->currentIndex().row());
    else if(num==3)
    {
        code=m_pGlobalVar->curCode;
        if (code.left(1)=="1" or code.left(3)=="399")
            return;
        info=findStock(m_pTableListCopy,code);
    }
    connect(act,&QAction::triggered,this,[info, this](){
        for (int i=0;i<m_pMyStockList->count();++i)
        {
            if (info.code==m_pMyStockList->at(i).code)
                return;
        }
        m_pMyStockList->append(info);
        m_pMyStockCode->append(info.code);
        int curIndex=pTableStock->myStockView->currentIndex().row();
         if (curIndex==-1) curIndex=0; //fixed
        pTableStock->m_myStockModel->setModelData(*m_pMyStockList,false,false,true);
        pTableStock->myStockView->setCurrentIndex(pTableStock->m_myStockModel->index(curIndex,0));
        pTableStock->m_risingSpeedModel->setModelData(*m_pRisingSpeedList,false,true,false);
        if (ifCanClick==1)
            pTableStock->m_tableModel->setModelData(*m_pTableList,false,true,false);
        else
            pFundFlow->model->item(pTableStock->stockTableView->currentIndex().row(),1)->setForeground(QColor(255,140,0));
        m_pGlobalVar->settings->setValue("myStock",*m_pMyStockCode);
    });
}

void MainWindow::showSearchResult()
{
    if (m_pGlobalVar->isBoard)
    {
      //  resetKParameter();
        searchSmallWindow->hide();
        m_pGlobalVar->WhichInterface=CNMARKET;
        pTableStock->m_tableModel->setModelData(*m_pTableList,false,true,false);
        pTableStock->stockTableView->setModel(pTableStock->m_tableModel);
        pTableStock->stockTableView->setCurrentIndex(pTableStock->m_tableModel->index(0,0));
        emit startThreadTable(false);
        toInterFace("main");
        //pTableStock->risingSpeedView->show();
        //pTableStock->myStockView->show();
        middleWindow->show();
    }
    else
    {
        m_pGlobalVar->isKState=true;
        resetKParameter();

        searchSmallWindow->hide();
        lastTime=QDateTime::currentDateTime();
        bWantReqKline=true;
        bool bFirst;
        if (m_PreCode != m_pGlobalVar->curCode)
        {
            bFirst=true;
            m_PreCode = m_pGlobalVar->curCode;
        }
        else
        {
            bFirst=false;
        }
        emit startThreadCandleChart(freq,adjustFlag,  bFirst);
        emit startThreadTimeShareChart(false);
        emit startThreadTimeShareTick(false);
        toInterFace("k");
    }
}

void MainWindow::resetKParameter()
{
  //  m_pGlobalVar->offsetEnd=m_pGlobalVar->settings->value("offsetEnd").toInt();
    m_pGlobalVar->KRange=KRANGE;
    m_pGlobalVar->offsetLocal=m_pGlobalVar->KRange;
    isFlashBaseInfo=true;
}

void MainWindow::dealWithFundFlow()
{
    int n=periodBox->currentIndex();
    if (n==1 or n==4)
    {
        QMessageBox::information(this,"提示","东方只能查看1日、5日和10日",QMessageBox::Ok);
        return;
    }
    dateEdit->setDate(m_pGlobalVar->curRecentWorkDay(0).date());
    tradedetailBox->setItemText(0,m_pGlobalVar->curRecentWorkDay(1).toString("yyyy-MM-dd"));

    m_pGlobalVar->isKState=false;
    ifCanClick=NORMALCLICK;
    m_pGlobalVar->WhichInterface=FUNDFLOW;
    toInterFace("fund");
    int days[]={1,3,5,10,20};
    pFundFlow->getEastPlateFundFlow(days[n]);
    pTableStock->stockTableView->setModel( pFundFlow->model);
    pTableStock->stockTableView->setColumnWidth(0,140);
    pTableStock->stockTableView->setColumnWidth(5,120);
    pTableStock->stockTableView->setColumnWidth(12,160);
    for (int i=1;i<5;++i)
        pTableStock->stockTableView->setColumnWidth(i,100);
    for (int i=6;i<12;++i)
        pTableStock->stockTableView->setColumnWidth(i,100);
}
void MainWindow::fastTrade()
{
#if 0
    if ((m_pGlobalVar->WhichInterface!=CNMARKET and m_pGlobalVar->WhichInterface!=FUNDFLOW) or m_pGlobalVar->curCode.left(1)=="1" or tradePrice==0)
        return;
    QMenu *menu=new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    QAction *actB=new QAction("闪电买入",menu);
    QIcon icon(":/new/png/png/buy.jpg");
    actB->setIcon(icon);
    menu->addAction(actB);
    QAction *actS=new QAction("闪电卖出",menu);
    QIcon icon1(":/new/png/png/sell.jpg");
    actS->setIcon(icon1);
    menu->addAction(actS);
    menu->popup(QCursor::pos());
    howPosition=0;
    QStringList proportionNums={"全仓","1/2","1/3","1/5","1/10"};
    float rate[5]={1.0,2.0,3.0,5.0,10.0};
    connect(actB,&QAction::triggered,this,[=](){
        PyGILState_STATE state=PyGILState_Ensure();
//        PyObject* pModule = PyImport_ImportModule("qmt");
//        if(!pModule)
//            qDebug()<<"import failure";
        PyObject* pFunTrade = PyObject_GetAttrString(pModule,"getAsset");
//        if(!pFunTrade)
//            qDebug()<<"get function failed";
        PyObject* args = PyTuple_New(1);
        PyTuple_SetItem(args,0,Py_BuildValue("s",account.toStdString().c_str()));
        PyObject* outcome=PyObject_CallFunction(pFunTrade,"O",args);
        float cash=QString(PyUnicode_AsUTF8(outcome)).toFloat();
        PyGILState_Release(state);

        QDialog *fastBuy=new QDialog();
        fastBuy->setAttribute(Qt::WA_DeleteOnClose);
        fastBuy->setWindowFlags(fastBuy->windowFlags() | Qt::WindowStaysOnTopHint);
        fastBuy->setWindowTitle(m_pGlobalVar->curName.split("(")[0]+" "+"闪电买入");
        fastBuy->setGeometry(670, 200, 260, 300);

        QLabel *infoName[5];
        QString name[5]={"证券代码:","报价方式:","买入价格:","最大可买:","买入数量:"};
        QVBoxLayout *mainLayout=new QVBoxLayout(fastBuy);
        QGridLayout *tradeInfo=new QGridLayout();
        QHBoxLayout *group=new QHBoxLayout();
        QHBoxLayout *buttom=new QHBoxLayout();

//        mainLayout->setContentsMargins(15, 5, 15, 20);
        mainLayout->addLayout(tradeInfo);
        mainLayout->addLayout(group);
        mainLayout->addSpacing(15);
        mainLayout->addLayout(buttom);
        for (int i=0;i<5;++i)
        {
            infoName[i]=new QLabel(name[i],fastBuy);
            infoName[i]->setStyleSheet("QLabel{font:16px;font-family:微软雅黑}");
            tradeInfo->addWidget(infoName[i],i,0);
        }
        QLabel *code=new QLabel(m_pGlobalVar->curCode,fastBuy);
        QLabel *fix=new QLabel("限价委托",fastBuy);
        QDoubleSpinBox *price=new QDoubleSpinBox(fastBuy);
        QLabel *unit=new QLabel("元",fastBuy);
        QLabel *unit1=new QLabel("股",fastBuy);
        QLabel *unit2=new QLabel("股",fastBuy);

        QLabel *maxNumbers=new QLabel(m_pGlobalVar->curCode,fastBuy);
        QSpinBox *buyNums=new QSpinBox(fastBuy);
        buyNums->setRange(0,1000000);
        buyNums->setSingleStep(100);
        int maxNums=floor(cash/tradePrice/100)*100;
        maxNumbers->setText(QString::number(maxNums));
        buyNums->setValue(maxNums);

        connect(price,&QDoubleSpinBox::valueChanged,this,[=](){
            int n=0;
            float m=price->text().toFloat();
            if (m)
                n=floor(cash/m/100);
            maxNumbers->setText(QString::number(n*100));
            buyNums->setValue(n/(howPosition+1)*100);
        });

        price->setSingleStep(0.01);
        price->setRange(0,10000);
        price->setValue(tradePrice);
        tradeInfo->addWidget(code,0,2);
        tradeInfo->addWidget(fix,1,2);
        tradeInfo->addWidget(price,2,2);
        tradeInfo->addWidget(unit,2,3);
        tradeInfo->addWidget(maxNumbers,3,2);
        tradeInfo->addWidget(unit1,3,3);
        tradeInfo->addWidget(buyNums,4,2);
        tradeInfo->addWidget(unit2,4,3);
        QButtonGroup *proportion=new QButtonGroup(fastBuy);
        QRadioButton *proportionName[5];
        for (int i=0;i<5;++i)
        {
            proportionName[i]=new QRadioButton(proportionNums[i],fastBuy);
            proportion->addButton(proportionName[i]);
            group->addWidget(proportionName[i]);
            connect(proportionName[i],&QRadioButton::clicked,this,[=]()mutable{
                int n=floor(cash/price->text().toFloat()/100);
                howPosition=i;
                buyNums->setValue(int(n/rate[i]+0.5)*100);
            });
        }
        proportionName[0]->setChecked(true);
        QPushButton *buy=new QPushButton("买入",fastBuy);
        QPushButton *close=new QPushButton("取消",fastBuy);
        if (maxNums==0)
            buy->setEnabled(false);
        connect(buy,&QPushButton::clicked,this,[=](){
            PyGILState_STATE state=PyGILState_Ensure();
            // PyObject* pModule = PyImport_ImportModule("qmt");
            // if(!pModule)
            //     qDebug()<<"import failure";
            PyObject* pFunTrade = PyObject_GetAttrString(pModule,"qmtBuy");
            // if(!pFunTrade)
            //     qDebug()<<"get function failed";

            PyObject* args = PyTuple_New(4);
            QString code=m_pGlobalVar->curCode;
            if (code.mid(0,1)=='3' or code.mid(0,1)=='0')
                code=code+".SZ";
            else
                code=code+".SH";
            PyTuple_SetItem(args,0,Py_BuildValue("s",account.toStdString().c_str()));
            PyTuple_SetItem(args,1,Py_BuildValue("s",code.toStdString().c_str()));
            PyTuple_SetItem(args,2,Py_BuildValue("i",buyNums->value()));
            PyTuple_SetItem(args,3,Py_BuildValue("f",price->value()));

            PyObject* outcome=PyObject_CallFunction(pFunTrade, "O",args);
            fastBuy->close();
            QMessageBox::information(this,"提示", QString(PyUnicode_AsUTF8(outcome)), QMessageBox::Ok);
            PyGILState_Release(state);
        });
        connect(close,&QPushButton::clicked,this,[=](){
            fastBuy->close();
        });

        buttom->addWidget(buy);
        buttom->addWidget(close);
        mainLayout->addSpacing(15);
        fastBuy->show();
    });
    connect(actS,&QAction::triggered,this,[=](){
        PyGILState_STATE state=PyGILState_Ensure();
//        PyObject* pModule = PyImport_ImportModule("qmt");
//        if(!pModule)
//            qDebug()<<"import failure";
        PyObject* pFunTrade = PyObject_GetAttrString(pModule,"getPositions");
//        if(!pFunTrade)
//            qDebug()<<"get function failed";
        PyObject* args = PyTuple_New(2);
        QString stockCode=m_pGlobalVar->curCode;
        if (stockCode.mid(0,1)=='3' or stockCode.mid(0,1)=='0')
            stockCode=stockCode+".SZ";
        else
            stockCode=stockCode+".SH";
        PyTuple_SetItem(args,0,Py_BuildValue("s",account.toStdString().c_str()));
        PyTuple_SetItem(args,1,Py_BuildValue("s",stockCode.toStdString().c_str()));
        PyObject* outcome=PyObject_CallFunction(pFunTrade, "O",args);
        QStringList l=QString(PyUnicode_AsUTF8(outcome)).split(";");
        PyGILState_Release(state);
//        if (l[0]=="-1" or l[1]=="-1")
//            return;

        QDialog *fastSell=new QDialog();
        fastSell->setAttribute(Qt::WA_DeleteOnClose);
        fastSell->setWindowFlags(fastSell->windowFlags() | Qt::WindowStaysOnTopHint);
        fastSell->setWindowTitle(m_pGlobalVar->curName.split("(")[0]+" "+"闪电卖出");
        fastSell->setGeometry(670, 200, 260, 300);

        QLabel *infoName[5];
        QString name[5]={"证券代码:","报价方式:","卖出价格:","最大可卖:","卖出数量:"};
        QVBoxLayout *mainLayout=new QVBoxLayout(fastSell);
        QGridLayout *tradeInfo=new QGridLayout();
        QHBoxLayout *group=new QHBoxLayout();
        QHBoxLayout *buttom=new QHBoxLayout();

        mainLayout->addLayout(tradeInfo);
        mainLayout->addLayout(group);
        mainLayout->addSpacing(15);
        mainLayout->addLayout(buttom);
        for (int i=0;i<5;++i)
        {
            infoName[i]=new QLabel(name[i],fastSell);
            infoName[i]->setStyleSheet("QLabel{font:16px;font-family:微软雅黑}");
            tradeInfo->addWidget(infoName[i],i,0);
        }
        QLabel *code=new QLabel(m_pGlobalVar->curCode,fastSell);
        QLabel *fix=new QLabel("限价委托",fastSell);
        QDoubleSpinBox *price=new QDoubleSpinBox(fastSell);
        QLabel *unit=new QLabel("元",fastSell);
        QLabel *unit1=new QLabel("股",fastSell);
        QLabel *unit2=new QLabel("股",fastSell);

        QLabel *numbers=new QLabel(m_pGlobalVar->curCode,fastSell);
        QSpinBox *sellNums=new QSpinBox(fastSell);
        sellNums->setRange(0,1000000);
        int maxNums=l[1].toInt();
        numbers->setText(QString::number(maxNums));
        sellNums->setValue(maxNums);

        price->setSingleStep(0.01);
        price->setRange(0,10000);
        price->setValue(tradePrice);
        tradeInfo->addWidget(code,0,2);
        tradeInfo->addWidget(fix,1,2);
        tradeInfo->addWidget(price,2,2);
        tradeInfo->addWidget(unit,2,3);
        tradeInfo->addWidget(numbers,3,2);
        tradeInfo->addWidget(unit1,3,3);
        tradeInfo->addWidget(sellNums,4,2);
        tradeInfo->addWidget(unit2,4,3);
        QButtonGroup *proportion=new QButtonGroup(fastSell);
        QRadioButton *proportionName[5];

        for (int i=0;i<5;++i)
        {
            proportionName[i]=new QRadioButton(proportionNums[i],fastSell);
            proportion->addButton(proportionName[i]);
            group->addWidget(proportionName[i]);
            connect(proportionName[i],&QRadioButton::clicked,this,[=](){
                float t=maxNums%100*100;
                sellNums->setValue(int(maxNums/100/rate[i]+0.5)*100+t);
            });
        }
        proportionName[0]->setChecked(true);
        QPushButton *sell=new QPushButton("卖出",fastSell);
        QPushButton *close=new QPushButton("取消",fastSell);
        if (maxNums==0)
            sell->setEnabled(false);
        connect(sell,&QPushButton::clicked,this,[=](){
            PyGILState_STATE state=PyGILState_Ensure();
            // PyObject* pModule = PyImport_ImportModule("qmt");
            // if(!pModule)
            //     qDebug()<<"import failure";
            PyObject* pFunTrade = PyObject_GetAttrString(pModule,"qmtSell");
            // if(!pFunTrade)
            //     qDebug()<<"get function failed";

            PyObject* args = PyTuple_New(4);
            QString code=m_pGlobalVar->curCode;
            if (code.mid(0,1)=='3' or code.mid(0,1)=='0')
                code=code+".SZ";
            else
                code=code+".SH";
            PyTuple_SetItem(args,0,Py_BuildValue("s",account.toStdString().c_str()));
            PyTuple_SetItem(args,1,Py_BuildValue("s",code.toStdString().c_str()));
            PyTuple_SetItem(args,2,Py_BuildValue("i",sellNums->value()));
            PyTuple_SetItem(args,3,Py_BuildValue("f",price->value()));

            PyObject* outcome=PyObject_CallFunction(pFunTrade, "O",args);
            fastSell->close();
            QMessageBox::information(this,"提示", QString(PyUnicode_AsUTF8(outcome)), QMessageBox::Ok);
            PyGILState_Release(state);
        });
        connect(close,&QPushButton::clicked,this,[=](){
            fastSell->close();
        });

        buttom->addWidget(sell);
        buttom->addWidget(close);
        mainLayout->addSpacing(15);
        fastSell->show();
    });
#endif
}
void MainWindow::delMyStock()
{
    int curIndex=pTableStock->myStockView->currentIndex().row();
    if (curIndex==-1)
        return;
    m_pMyStockList->removeAt(curIndex);
    m_pMyStockCode->removeAt(curIndex);
    if (curIndex==pTableStock->m_myStockModel->rowCount()-1)
        curIndex-=1;
    pTableStock->m_myStockModel->setModelData(*m_pMyStockList,false,false,true);
    pTableStock->myStockView->setCurrentIndex(pTableStock->m_myStockModel->index(curIndex,0));
    pTableStock->m_risingSpeedModel->setModelData(*m_pRisingSpeedList,false,true,false);
    pTableStock->m_tableModel->setModelData(*m_pTableList,false,true,false);
    m_pGlobalVar->settings->setValue("myStock",*m_pMyStockCode);
}

void MainWindow::updateFeeling(QDate date)
{    
    if (not m_pTimeShareChartList->isEmpty())
    {
        QString qStrTime =  m_pTimeShareChartList->at(0).time;
        QDateTime tt = QDateTime::fromString(qStrTime, "yyyy-MM-dd hh:mm");
        QDate tt_ = tt.date();
        if (tt_ ==date)
        {
          feelingData[0]->setText(QString::number(m_pGlobalVar->upNums[0]));
          feelingData[1]->setText(QString::number(m_pGlobalVar->upNums[1]));
          feelingData[2]->setText(QString::number(m_pGlobalVar->upNums[2])+"/"+QString::number(m_pGlobalVar->upNums[3]));
          feelingData[4]->setText(QString::number(m_pGlobalVar->downNums[0]));
          feelingData[5]->setText(QString::number(m_pGlobalVar->downNums[1]));
          feelingData[6]->setText(QString::number(m_pGlobalVar->downNums[2])+"/"+QString::number(m_pGlobalVar->downNums[3]));
          feelingData[3]->setText(format_conversion(threadIndex->totalAmount));
          return;
        }
    }
    QString d=date.toString("yyyy-MM-dd");
    QFile file(m_pGlobalVar->currentPath+"/list/feeling.csv");
    if (!file.open(QFile::ReadOnly))
    {
        feelingData[0]->setText("--");
        feelingData[1]->setText("--");
        feelingData[2]->setText("--/--");
        feelingData[3]->setText("--");
        feelingData[4]->setText("--");
        feelingData[5]->setText("--");
        feelingData[6]->setText("--/--");
       // QMessageBox::information(this,"提示","没有此日期数据",QMessageBox::Ok);
        return;
    }
    QTextStream in(&file);
    QString s = in.readAll();
    file.close();
    QList l=s.split("\n");
    int nCount = l.size();
    for (int i=0;i<nCount;++i)
    {
        QList v=l[i].split(",");
        if (v.size()>=11 and  d==v[0])
        {
          feelingData[0]->setText(v[2]);
          feelingData[1]->setText(v[4]);
          feelingData[2]->setText(v[6]+"/"+v[8]);
          feelingData[3]->setText(format_conversion(v[10].toDouble()));
          feelingData[4]->setText(v[3]);
          feelingData[5]->setText(v[5]);
          feelingData[6]->setText(v[7]+"/"+v[9]);
          return;
        }
    }
    if (date!=QDate::currentDate()){
        feelingData[0]->setText("--");
        feelingData[1]->setText("--");
        feelingData[2]->setText("--/--");
        feelingData[3]->setText("--");
        feelingData[4]->setText("--");
        feelingData[5]->setText("--");
        feelingData[6]->setText("--/--");
      //  QMessageBox::information(this,"提示","没有此日期数据",QMessageBox::Ok);
    }
}

void MainWindow::tradingTimeRunThread()
{
    QDateTime curTime=QDateTime::currentDateTime();
//    if (not ui->DLAllStockK->isEnabled() and curTime.time().toString("hh:mm")>"15:00")
//        ui->DLAllStockK->setEnabled(true);

    if (timeCount%2==0 ) // and m_pGlobalVar->WhichInterface==CNMARKET and m_pGlobalVar->isZhMarketDay(curTime))
    {
        QDate date = curTime.date();
        int weekDay = date.dayOfWeek();
        if ((weekDay!=6 and weekDay!=7 and (m_pGlobalVar->WhichInterface==CNMARKET or m_pGlobalVar->WhichInterface==HKMARKET or m_pGlobalVar->WhichInterface==FUNDFLOW)) or
            ( weekDay!=7 and (m_pGlobalVar->WhichInterface==USMARKET or m_pGlobalVar->WhichInterface==UKMARKET))
            )
        {
           // qDebug()<<"m_pGlobalVar->WhichInterface=" << m_pGlobalVar->WhichInterface <<" " <<m_pGlobalVar->curCode;
            QStringList list=m_pGlobalVar->curCode.split(".");
            QString market ;
            if (list.size()!=1)
                market =list[0];
            else
                market =m_pGlobalVar->curCode.left(3);
            if (m_pGlobalVar->isTradeTime(curTime)  or
                  market=="100" or market=="133" or market=="122" or market=="104" or market=="103" or market=="90")
            {
                  emit startThreadTimeShareTick(false);
            }
        }
    }

    if (timeCount%6==1 and m_pGlobalVar->WhichInterface==CNMARKET)
    {
        //if (m_pGlobalVar->isZhMarketDay(curTime))
        if (m_pGlobalVar->isTradeTime(curTime))
        {
            circle->setStyleSheet(CIRCLE_GREEN_SHEETSTYLE);
            if (m_pGlobalVar->isBoard)
                pSearchStock->getBoardData(m_pTableList);
            emit startThreadTable(false);
            emit startThreadTimeShareChart(false);
        }
        else
            circle->setStyleSheet(CIRCLE_RED_SHEETSTYLE);
    }
    else if (timeCount%10==4)
    {
        if (isWorkDay(curTime) or ((m_pGlobalVar->isTradeTime(curTime)) and  m_pGlobalVar->WhichInterface==CNMARKET ))
            emit startThreadIndex(false);
        else
            reFlashIndex();

        if (m_pGlobalVar->isKState==true){ // fixed add 2024.10.9

            //ReqKtimeCount++;
            bool bReq=false;

            if (freq=="1"){
                int seconds = lastTime.secsTo(curTime);
                if (seconds>=59){
                     bReq= true;
                }
            }
            else if (freq=="5"){
                int seconds = lastTime.secsTo(curTime);
                if (seconds>=295){
                    bReq= true;
                }
            }
            else if (freq=="15"){
                int seconds = lastTime.secsTo(curTime);
                if (seconds>=995){
                    bReq= true;
                }
            }
            else if (freq=="30"){
                int seconds = lastTime.secsTo(curTime);
                if (seconds>=1800){
                    bReq= true;
                }
            }
            else if (freq=="60"){
                int seconds = lastTime.secsTo(curTime);
                if (seconds>=3600){
                    bReq= true;
                }
            }

            QDate date = curTime.date();
            int weekDay = date.dayOfWeek();
            if ( bReq and  (( weekDay!=6 and weekDay!=7 and (m_pGlobalVar->WhichInterface==CNMARKET or m_pGlobalVar->WhichInterface==HKMARKET or m_pGlobalVar->WhichInterface==FUNDFLOW)) or
                                                    ( weekDay!=7 and (m_pGlobalVar->WhichInterface==USMARKET or m_pGlobalVar->WhichInterface==UKMARKET)))
                )
            {
              //  qDebug()<<"m_pGlobalVar->WhichInterface=" << m_pGlobalVar->WhichInterface <<" " <<m_pGlobalVar->curCode;
                QStringList list=m_pGlobalVar->curCode.split(".");
                QString market ;
                if (list.size()!=1)
                    market =list[0];
                else
                    market =m_pGlobalVar->curCode.left(3);

                if ( not bWantReqKline and ( m_pGlobalVar->isTradeTime(curTime) or
                                       market=="100" or market=="133" or market=="122" or market=="104" or market=="103" or market=="90"))
                {

                    bWantReqKline=true;
                    lastTime=QDateTime::currentDateTime();
                    emit startThreadCandleChart(freq,adjustFlag,false);
                }
            }
        }

        if (m_pGlobalVar->WhichInterface==USMARKET)
        {
            if (m_pGlobalVar->isUSMarketDay(curTime))
            {
                circle->setStyleSheet(CIRCLE_GREEN_SHEETSTYLE);
                emit startThreadTable(false);
                emit startThreadTimeShareTick(false);
                emit startThreadTimeShareChart(false);
            }
            else
                circle->setStyleSheet(CIRCLE_RED_SHEETSTYLE);
        }
        else if (m_pGlobalVar->WhichInterface==HKMARKET)
        {
            //if (m_pGlobalVar->isHKMarketDay(curTime))
            if (m_pGlobalVar->isTradeTime(curTime))
            {
                circle->setStyleSheet(CIRCLE_GREEN_SHEETSTYLE);
                emit startThreadTable(false);
                emit startThreadTimeShareTick(false);
                emit startThreadTimeShareChart(false);
            }
            else
                circle->setStyleSheet(CIRCLE_RED_SHEETSTYLE);
        }
        else if (m_pGlobalVar->WhichInterface==UKMARKET)
        {
            if (m_pGlobalVar->isUKMarketDay(curTime))
            {
                circle->setStyleSheet(CIRCLE_GREEN_SHEETSTYLE);
                emit startThreadTable(false);
                emit startThreadTimeShareTick(false);
                emit startThreadTimeShareChart(false);
            }
            else
                circle->setStyleSheet(CIRCLE_RED_SHEETSTYLE);
        }
        else if (m_pGlobalVar->WhichInterface==FUNDFLOW)
            circle->setStyleSheet(CIRCLE_RED_SHEETSTYLE);
    }
    else if (timeCount==30)
    {
        emit startThreadGetNews();
        QString curDate=curTime.date().toString("yyyy-MM-dd");

        //接口要求相隔1小时以上
        static bool bFirst=true;
        int qdtDelta = dldlastTime.secsTo(curTime);
        if ( (curDate>downloadDate and (bFirst or (not bFirst and  qdtDelta>60*60)))  and isWorkDay(curTime) and curTime.time().toString("hh:mm")>="08:50")
        {
            QDateTime curNow=m_pGlobalVar->curWorkDay();
            QDate t_= curNow.date();
            dateEdit1->setDate(t_);

            QStringList vacation=m_pGlobalVar->settings->value("Vacation_ZH").toStringList();
            QString cur_date=curTime.toString("MMdd");

            if (not vacation.contains(cur_date))
            {
                bFirst=false;
                bool bResult=pRequestsToCsv->downStockIndexPlateInfo();
                if (bResult){
                  m_pGlobalVar->settings->setValue("curTime",curDate);
                  downloadDate=curDate;
                }

                dldlastTime=QDateTime::currentDateTime();
                //try init                                                
                QString qStrDate =m_pGlobalVar->settings->value("isInitDone").toString();

                cur_date= curNow.toString("yyyy-MM-dd");
                qDebug() <<"tradingTimeRunThread "<<  cur_date <<"," <<qStrDate ;
                if (qStrDate !=cur_date ) // cur_date)
                {
                  if (!m_pGlobalVar->isBoard) {
                    qDebug() <<"***init**** ";
                    emit startThreadTable(true);
                  }
                  else
                    emit startThreadTable(false);
                  m_pGlobalVar->settings->setValue("isInitDone", cur_date);
                }
                emit startThreadTimeShareChart(false);
                emit startThreadTimeShareTick(true);
                emit startThreadIndex(true);
            }
        }
        if (curDate>feelingDate and curTime.time().toString("hh:mm")>"15:00")
        {
            QFile file(m_pGlobalVar->currentPath+"/list/feeling.csv");
            if (file.open(QFile::ReadOnly))
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
                if (str!=curDate)
                {
                    file.close();
                    if (file.open(QFile::Append))
                    {
                        QStringList dataList;
                        dataList<<curDate<<"";
                        for (int i=0;i<4;++i)
                            dataList<<QString::number(m_pGlobalVar->upNums[i])<<QString::number(m_pGlobalVar->downNums[i]);
                        dataList<<QString::number(threadIndex->totalAmount,'f',2)<<"";
                        file.write(dataList.join(",").toLocal8Bit()+"\n");
                    }
                    feelingDate =curDate;
                    m_pGlobalVar->settings->setValue("feelingTime",curDate);
                }
                file.close();
            }
        }
        timeCount=0;
    }
    timeCount+=1;
}

void MainWindow::reFlashIndex()
{
    changeInTurn=not changeInTurn;
//    qDebug()<<ui->statusBar->children();
    int n;
    QPushButton *qBtn;
    QLabel *lbl;
    for (int i = 2; i<=12; i+=2)
    {
        qBtn = (QPushButton *)(ui->statusBar->children().at(i));
        lbl = (QLabel *)(ui->statusBar->children().at(i+1));
        if (isAsia)
        {
            if ((i==8 or i==10 or i==12) and not changeInTurn)
                n=i/2+2;
            else
                n=i/2-1;
        }
        else
        {
            if ((i==8 or i==10 or i==12) and not changeInTurn)
                n=i/2+11;
            else
                n=i/2+8;
        }
        if (n>=m_pIndexList->size())
            continue;
        qBtn->setText(m_pIndexList->at(n).name);
        if (m_pIndexList->at(n).pctChg.toFloat()>0)
            lbl->setPalette(m_pGlobalVar->pRed);
        else if (m_pIndexList->at(n).pctChg.toFloat()<0)
            lbl->setPalette(m_pGlobalVar->pGreen);
        else
            lbl->setPalette(m_pGlobalVar->pBlack);
        lbl->setText(m_pIndexList->at(n).close+" "+m_pIndexList->at(n).pctChg+"%");
    }
    int m=20;
    n=19;
    if (changeInTurn)
    {
        n=18;
        m=21;
    }
    if (n>=m_pIndexList->size())
        return;
    qBtn = (QPushButton *)(ui->statusBar->children().at(14));
    qBtn->setText(m_pIndexList->at(n).name);
    lbl = (QLabel *)(ui->statusBar->children().at(15));
    if (m_pIndexList->at(n).pctChg.toFloat()>0)
        lbl->setPalette(m_pGlobalVar->pRed);
    else if (m_pIndexList->at(n).pctChg.toFloat()<0)
        lbl->setPalette(m_pGlobalVar->pGreen);
    else
        lbl->setPalette(m_pGlobalVar->pBlack);
    lbl->setText(m_pIndexList->at(n).close+" "+m_pIndexList->at(n).pctChg+"%");
    qBtn= (QPushButton *)(ui->statusBar->children().at(16));
    qBtn->setText(m_pIndexList->at(m).name);
    lbl = (QLabel *)(ui->statusBar->children().at(18));
    if (m_pIndexList->at(m).pctChg.toFloat()>0)
        lbl->setPalette(m_pGlobalVar->pRed);
    else if (m_pIndexList->at(m).pctChg.toFloat()<0)
        lbl->setPalette(m_pGlobalVar->pGreen);
    else
        lbl->setPalette(m_pGlobalVar->pBlack);
    lbl->setText(m_pIndexList->at(m).close+" "+m_pIndexList->at(m).pctChg+"%");
}
void MainWindow::reFlashBuySellBaseInfo()  //显示5档挂盘买卖价
{
    QString str;
    int nums,d=m_pGlobalVar->setRound();
    float price;
    for (int i=0;i<10;++i)
    {
        price=m_pGlobalVar->buySellPrice[i];
        if (price==0)
            str="";
        else
        {
            str=QString::number(price,'f',d);
            if (price>m_pGlobalVar->preClose)
                buySellPrice[i]->setPalette(m_pGlobalVar->pRed);
            else if (price<m_pGlobalVar->preClose)
                buySellPrice[i]->setPalette(m_pGlobalVar->pGreen);
            else
                buySellPrice[i]->setPalette(m_pGlobalVar->pBlack);
        }
        buySellPrice[i]->setText(str);
        nums=m_pGlobalVar->buySellNum[i];
        if (nums==0)
            str="";
        else
        {
            if (nums>1000000)
                str=format_conversion(nums);
            else
                str=QString::number(nums);
        }
        buySellNum[i]->setText(str);
    }
    if (!isFlashBaseInfo){
        if (m_pGlobalVar->isKState)
            pDrawChart->candleChart->update();
        return;
    }
    if (m_pGlobalVar->baseInfoData[1]>0)
    {
        baseInfoData[0]->setPalette(m_pGlobalVar->pRed);
        baseInfoData[1]->setPalette(m_pGlobalVar->pRed);
    }
    else if(m_pGlobalVar->baseInfoData[1]<0)
    {
        baseInfoData[0]->setPalette(m_pGlobalVar->pGreen);
        baseInfoData[1]->setPalette(m_pGlobalVar->pGreen);
    }
    else
    {
        baseInfoData[0]->setPalette(m_pGlobalVar->pBlack);
        baseInfoData[1]->setPalette(m_pGlobalVar->pBlack);
    }

    baseInfoData[0]->setText(FormatNumber(m_pGlobalVar->baseInfoData[0],d,""));
   // for (int i=1;i<3;++i){
    //qDebug()<<m_pGlobalVar->baseInfoData[1] <<" "<<m_pGlobalVar->preClose;
        baseInfoData[1]->setText(FormatNumber(m_pGlobalVar->baseInfoData[1],2,"%"));
        baseInfoData[2]->setText(FormatNumber(m_pGlobalVar->baseInfoData[2],2,"%"));
    //}
    for (int i=3;i<7;++i){
        baseInfoData[i]->setText(format_conversion(m_pGlobalVar->baseInfoData[i]));
    }
    float v,pct=0;
    for (int i=7;i<10;++i)
    {
        v=m_pGlobalVar->baseInfoData[i];
        if (m_pGlobalVar->preClose==0 or v==0)
            pct=0;
        else
            pct=(v-m_pGlobalVar->preClose)*100.0f/m_pGlobalVar->preClose;

        if (pct!=0)
          baseInfoData[i]->setText(QString::number(v,'f',d)+"("+format_conversion(pct)+"%)");
        else
           baseInfoData[i]->setText("--");

        if (pct>0)
            baseInfoData[i]->setPalette(m_pGlobalVar->pRed);
        else if(pct<0)
            baseInfoData[i]->setPalette(m_pGlobalVar->pGreen);
        else
            baseInfoData[i]->setPalette(m_pGlobalVar->pBlack);
    }
    for (int i=10;i<14;++i) {
          if(i==12)
            baseInfoData[i]->setText(FormatNumber(m_pGlobalVar->baseInfoData[i],3,""));
          else
            baseInfoData[i]->setText(format_conversion(m_pGlobalVar->baseInfoData[i]));
    }
    stockCode->setText(m_pGlobalVar->curCode);
    stockName->setText(m_pGlobalVar->curName);
    EPSLabel->setText(m_pGlobalVar->EPSReportDate);
    PELabel->setText(m_pGlobalVar->PEName);
    if (m_pGlobalVar->isKState)
     pDrawChart->candleChart->update();
}
    
void MainWindow::flashOldCandleInfo(QMouseEvent *mouseEvent)
{
    int nKdCount= (int)m_pKline->size();
    if (nKdCount==0)
    {
        return;
    }
    float aveWidth =  pDrawChart->CandleChartWidth() / m_pGlobalVar->KRange;
    int w = aveWidth / 4;
    if ((w % 2) == 0){
        w++;
    }
    QPoint mPoint =mouseEvent->pos();
    int x= mPoint.x();
    int nIdx = pDrawChart->XTransferCandleChart(x+w);
    int begin = nKdCount -m_pGlobalVar->offsetLocal;
    if ( begin <0){
        begin =0;
    }
    nIdx+=begin;

    if (nIdx>(nKdCount -1) or nIdx<0){
        nIdx= nKdCount-1;
    }
   
    pDrawChart->Cross_Cursor_Idx= nIdx;
    std::vector<CL::Kline>::iterator ptr = m_pKline->begin();
    float temp=(ptr+nIdx)->pctChg;
    if (temp>0)
    {
        baseInfoData[0]->setPalette(m_pGlobalVar->pRed);
        baseInfoData[1]->setPalette(m_pGlobalVar->pRed);
    }
    else if(temp<0)
    {
        baseInfoData[0]->setPalette(m_pGlobalVar->pGreen);
        baseInfoData[1]->setPalette(m_pGlobalVar->pGreen);
    }
    else
    {
        baseInfoData[0]->setPalette(m_pGlobalVar->pBlack);
        baseInfoData[1]->setPalette(m_pGlobalVar->pBlack);
    }
    baseInfoData[0]->setText(QString::number((ptr+nIdx)->close));
    if ((ptr+nIdx)->pctChg!=0)
      baseInfoData[1]->setText(FormatNumber((ptr+nIdx)->pctChg,2,"%"));
    else{
      if (nIdx>0 && (ptr+nIdx)->pctChg==0 && freq!="1"){
            temp = ((ptr+nIdx)->close-(ptr+nIdx-1)->close)*100.0f/(ptr+nIdx-1)->close;
            baseInfoData[1]->setText(FormatNumber( temp,2,"%"));
       }
       else
         baseInfoData[1]->setText("--");
    }
    baseInfoData[2]->setPalette(m_pGlobalVar->pBlue);
    baseInfoData[3]->setPalette(m_pGlobalVar->pBlue);
    baseInfoData[12]->setPalette(m_pGlobalVar->pBlue);
    baseInfoData[2]->setText(FormatNumber((ptr+nIdx)->turn,2,"%"));

    baseInfoData[3]->setText(format_conversion((ptr+nIdx)->amount));

    baseInfoData[12]->setText(format_conversion((ptr+nIdx)->vol));

    QString  strType="";

    if (m_pGlobalVar->curCode.contains("."))
        strType =m_pGlobalVar->curCode.split(".")[0];
    if (strType =="104"){
      baseInfoData[13]->setText("--");

    }
    else
    {   
      if (m_pGlobalVar->WhichInterface==CNMARKET)
        baseInfoData[13]->setText(format_conversion((ptr+nIdx)->vol*10000/(ptr+nIdx)->turn));
      else
        baseInfoData[13]->setText(format_conversion((ptr+nIdx)->vol*100/(ptr+nIdx)->turn));
    }
    if (nIdx==0)
        temp=(ptr+nIdx)->close/(1+(ptr+nIdx)->pctChg/100);
    else
        temp=m_pKline->at(nIdx-1).close;

    QList<float> t={(ptr+nIdx)->open,(ptr+nIdx)->high,
                   (ptr+nIdx)->low,temp};
    for (int i=0;i<3;++i)
    {
        if (t[i]>t[3])
            baseInfoData[i+9]->setPalette(m_pGlobalVar->pRed);
        else if (t[i]<t[3])
            baseInfoData[i+9]->setPalette(m_pGlobalVar->pGreen);
        else
            baseInfoData[i+9]->setPalette(m_pGlobalVar->pBlack);
        baseInfoData[i+9]->setText(QString::number(t[i])+"("+QString::number((t[i]-temp)*100/temp,'f',2)+"%)");
    }
    pDrawChart->lblrowTime->show();

    if (mPoint.ry()>pDrawChart->CandleChartBottom())
    {
        float vol=pDrawChart->candleHighLowPoint[2]-pDrawChart->candleHighLowPoint[2]*(mPoint.ry()-pDrawChart->CandleChartHeight())/(pDrawChart->candleChart->height()-pDrawChart->CandleChartHeight());
        pDrawChart->lblcolPrice->setText(format_conversion(int(vol)));
    }
    else
    {
        if (mPoint.ry()>=KTOPHEIGHTEDGE)
        {
            float price=  pDrawChart->YTransferCandleChart(mPoint.y()); // pDrawChart->candleHighLowPoint[0]-(pDrawChart->candleHighLowPoint[0]-pDrawChart->candleHighLowPoint[1])*(mPoint.ry()-KTOPHEIGHTEDGE)/(pDrawChart->candleChart->height()-pDrawChart->CandleChartHeight());
            pDrawChart->lblcolPrice->setText(FormatNumber(price,2,""));
        }
    }
    pDrawChart->lblcolPrice->adjustSize();

    QString qStrDate=getKlineDate((ptr+nIdx)->date);
    pDrawChart->lblrowTime->setText(qStrDate);

    if (mPoint.ry()>=KTOPHEIGHTEDGE)
    {
        pDrawChart->lblcolPrice->show();
        pDrawChart->lblcolPrice->move(pDrawChart->CandleChartWidth(),mPoint.ry()-pDrawChart->lblcolPrice->height());
        pDrawChart->hlblKLine->move(0,mPoint.ry());
    }
    pDrawChart->lblrowTime->move(mPoint.rx()-(mPoint.x()-KWIDTHEDGE)*pDrawChart->lblrowTime->width()/(pDrawChart->CandleChartWidth()),pDrawChart->CandleChartBottom());
    pDrawChart->vlblKLine->setStyleSheet("QLabel{border:2px dotted white;}");
    pDrawChart->vlblKLine->resize(1,pDrawChart->candleChart->height());
    pDrawChart->hlblKLine->setStyleSheet("QLabel{border:2px dotted white;}");
    pDrawChart->hlblKLine->resize(pDrawChart->candleChart->width(),1);
    //int posX=(2*m+1)*(pDrawChart->candleChart->width()-2*KWIDTHEDGE-KRIGHTWIDTHEDGE)/(2*m_pGlobalVar->KRange);
    //pDrawChart->vlblKLine->move(posX+KWIDTHEDGE,0);
    pDrawChart->vlblKLine->move(x,0);
    pDrawChart->vlblKLine->show();
    pDrawChart->hlblKLine->show();
    
    pDrawChart->candleChart->update();
}

QString MainWindow::getKlineDate(time_t date)
{
    QDateTime  dateTime = QDateTime::fromSecsSinceEpoch(date);
    QString qStrDate;
     if (freq == "1" or freq == "5" or freq == "15" or freq == "30" or freq == "60")
        qStrDate =dateTime.toString("yyyy-MM-dd HH:mm");
    else
        qStrDate =dateTime.toString("yyyy-MM-dd");

    return qStrDate;
}

void MainWindow::toInterFace(QString which)
{
    if (which=="main")
    {
        m_pGlobalVar->isKState=false;
        isTraversalMyStock=false;
        rightFundWindow->hide();
        pDrawChart->candleChart->hide();
        rightBaseWindow->show();
        pTableStock->stockTableView->show();
        int row=pTableStock->stockTableView->currentIndex().row();
        if (row==-1)
            row=0;
        pTableStock->stockTableView->setModel(pTableStock->m_tableModel);
        pTableStock->m_tableModel->setModelData(*m_pTableList,true,true,false);
        pTableStock->stockTableView->setCurrentIndex(pTableStock->m_tableModel->index(row,0));
    }
    else if (which=="k")
    {
        m_pGlobalVar->isKState=true;
        pTableStock->stockTableView->hide();
        middleWindow->hide();
        rightFundWindow->hide();
        pDrawChart->candleChart->show();
        rightBaseWindow->show();
        rightBaseWindow->setFocus();
        pFundFlow->initAllNews();
    }
    else if (which=="fund")
    {
        m_pGlobalVar->WhichInterface=FUNDFLOW;
        m_pGlobalVar->isKState=false;
        isTraversalMyStock=false;
        rightBaseWindow->hide();
        pDrawChart->candleChart->hide();
        middleWindow->hide();
        rightFundWindow->show();
        pTableStock->stockTableView->show();
    }
    else if(which=="f3")
    {
        if (m_pGlobalVar->WhichInterface==HKMARKET or m_pGlobalVar->WhichInterface==USMARKET or m_pGlobalVar->WhichInterface==UKMARKET)
        {
            QMessageBox::information(this,"提示", "只能查看A股", QMessageBox::Ok);
            return;
        }
        if (m_pGlobalVar->curCode.left(1)=="1" or m_pGlobalVar->curCode.left(3)=="399" or m_pGlobalVar->curCode.left(3)=="899")
        {
            QMessageBox::information(this,"提示", "只能查看个股", QMessageBox::Ok);
            return;
        }
        F10SmallWindow->setFixedSize(675,500);
        fTitle->setText(m_pGlobalVar->curCode+" "+m_pGlobalVar->curName.left(m_pGlobalVar->curName.indexOf("(")));
        F10SmallWindow->show();
        pF10View->dealWithHotRank();
    }
    else if(which=="f10")
    {
        if (m_pGlobalVar->curCode.left(1)=="1" or m_pGlobalVar->curCode.left(3)=="399" or m_pGlobalVar->curCode.left(3)=="899")
        {
            QMessageBox::information(this,"提示", "只能查看个股", QMessageBox::Ok);
            return;
        }
        if (m_pGlobalVar->WhichInterface==HKMARKET or m_pGlobalVar->WhichInterface==USMARKET or m_pGlobalVar->WhichInterface==UKMARKET)
        {
            QMessageBox::information(this,"提示", "只能查看A股", QMessageBox::Ok);
            return;
        }
        F10SmallWindow->setFixedSize(1275,700);
        fTitle->setText(m_pGlobalVar->curCode+" "+m_pGlobalVar->curName.left(m_pGlobalVar->curName.indexOf("(")));
        F10SmallWindow->show();
        pF10View->dealWithMainIndex();
    }
    else
        QMessageBox::information(this,"提示", "界面转换错误", QMessageBox::Ok);
}
void MainWindow::toFundFlow()
{
    pTableStock->stockTableView->clearSpans();
    if (sender()==fundFlow[0])
        dealWithFundFlow();
    else if((sender()==fundFlow[1]))
    {
        ifCanClick=OTHERCLICK;
        pFundFlow->getRoyalFlushFundFlow();
        pTableStock->stockTableView->setModel( pFundFlow->model);
    }
    else if((sender()==fundFlow[2]))
    {
        ifCanClick=OTHERCLICK;
        pFundFlow->getIntervalHighLow();
        pTableStock->stockTableView->setSpan(2,8,29,8);
        pTableStock->stockTableView->setIndexWidget( pFundFlow->model->index(2, 8), pFundFlow->tableChart);
        pTableStock->stockTableView->setModel( pFundFlow->model);

    }
    else if((sender()==fundFlow[3]))
    {
        ifCanClick=FUNDFLOWCLICK;
        pFundFlow->getStockHot(m_pMyStockCode,m_pTableListCopy);
        pTableStock->stockTableView->setModel( pFundFlow->model);
    }
    else if((sender()==fundFlow[4]))
    {
        ifCanClick=FUNDFLOWCLICK;
        pFundFlow->getNotNormalStock(m_pMyStockCode);
        pTableStock->stockTableView->setModel( pFundFlow->model);
    }
    else if((sender()==fundFlow[5]))
    {
        ifCanClick=FUNDFLOWCLICK;
        pFundFlow->getStockPoolStrong(m_pMyStockCode,dateEdit->date().toString("yyyyMMdd"));
        pTableStock->stockTableView->setModel( pFundFlow->model);
        for (int i=0;i<15;++i)
            pTableStock->stockTableView->setColumnWidth(i,90);
    }
    else if((sender()==fundFlow[6]))
    {
        ifCanClick=FUNDFLOWCLICK;
        QString days[]={"1","3","5","10","M","Q","Y"};
        pFundFlow->getNorthFundFlow(m_pMyStockCode,days[northBox->currentIndex()]);
        pTableStock->stockTableView->setModel( pFundFlow->model);
        for (int i=0;i<14;++i)
            pTableStock->stockTableView->setColumnWidth(i,90);
        pTableStock->stockTableView->setColumnWidth(6,120);
        pTableStock->stockTableView->setColumnWidth(7,120);
        pTableStock->stockTableView->setColumnWidth(10,120);
        pTableStock->stockTableView->setColumnWidth(11,120);
        pTableStock->stockTableView->setColumnWidth(12,120);
    }
    else if((sender()==fundFlow[7]))
    {
        ifCanClick=FUNDFLOWCLICK;
        int nums[]={1,3,5,10,30};
        int pages[]={1,1,1,2,4};
        pFundFlow->getDragonTigerList(m_pMyStockCode,nums[tradedetailBox->currentIndex()],pages[tradedetailBox->currentIndex()]);
        pTableStock->stockTableView->setModel( pFundFlow->model);
        pTableStock->stockTableView->setColumnWidth(0,60);
        pTableStock->stockTableView->setColumnWidth(1,75);
        pTableStock->stockTableView->setColumnWidth(2,210);
        for (int i=3;i<9;++i)
            pTableStock->stockTableView->setColumnWidth(i,75);
        pTableStock->stockTableView->setColumnWidth(9,90);
        pTableStock->stockTableView->setColumnWidth(10,100);
        pTableStock->stockTableView->setColumnWidth(11,100);
        pTableStock->stockTableView->setColumnWidth(12,65);
        pTableStock->stockTableView->setColumnWidth(13,85);
        pTableStock->stockTableView->setColumnWidth(14,300);
    }
    else if((sender()==fundFlow[8]))
    {
        ifCanClick=FUNDFLOWCLICK;
        QString nums[]={"01","02","03","04"};
        int pages[]={2,3,4,7};
        pFundFlow->countSingleStockBoard(m_pMyStockCode,nums[singleStockBoard->currentIndex()],pages[singleStockBoard->currentIndex()]);
        pTableStock->stockTableView->setModel( pFundFlow->model);
        pTableStock->stockTableView->setColumnWidth(0,65);
        pTableStock->stockTableView->setColumnWidth(1,80);
        pTableStock->stockTableView->setColumnWidth(2,100);
        pTableStock->stockTableView->setColumnWidth(3,65);
        pTableStock->stockTableView->setColumnWidth(4,65);
        pTableStock->stockTableView->setColumnWidth(5,65);
        for (int i=6;i<19;++i)
            pTableStock->stockTableView->setColumnWidth(i,90);
    }
    else if((sender()==fundFlow[9]))
    {
        ifCanClick=OTHERCLICK;
        QString ft[]={"all","gp","hh","zq","zs","qdii","lof","fof"};
        QString sc[]={"zzf", "6yzf", "6yzf", "6yzf", "6yzf", "6yzf", "6yzf", "6yzf"};
        pFundFlow->openFundRank(ft[openFundBox->currentIndex()],sc[openFundBox->currentIndex()]);
        pTableStock->stockTableView->setModel( pFundFlow->model);
        pTableStock->stockTableView->setColumnWidth(0,65);
        pTableStock->stockTableView->setColumnWidth(1,280);
        pTableStock->stockTableView->setColumnWidth(2,100);
        for (int i=3;i<16;++i)
            pTableStock->stockTableView->setColumnWidth(i,90);
    }
}
void MainWindow::downUpLookStock(QWheelEvent *event)
{
    if (isThreadRunning )
        return;
    isThreadRunning=true;
    pDrawChart->hisTimeShareChart->close();
    pDrawChart->annoucementWindow->close();
    if (m_pGlobalVar->isKState)
    {
        resetKParameter();
        int curRow=pTableStock->stockTableView->currentIndex().row();
        int counts=0;
        if (ifCanClick==FUNDFLOWCLICK)
            counts= pFundFlow->model->rowCount()-1;
        else
        {
            if (isTraversalMyStock)
            {
                counts=m_pMyStockList->count()-1;
                curRow=pTableStock->myStockView->currentIndex().row();
            }
            else
            {
                counts=m_pTableList->count()-1;
            }

        }
        if (event->angleDelta().y()<0)
        {
            curRow++;
            if (curRow>counts)
                curRow=0;
        }
        else
        {
            curRow--;
            if (curRow<0)
                curRow=counts;
        }
        if (ifCanClick==FUNDFLOWCLICK)
        {
            m_pGlobalVar->curCode= pFundFlow->model->item(curRow,0)->text();
            pTableStock->stockTableView->setCurrentIndex( pFundFlow->model->index(curRow,0));
        }
        else
        {
            if (isTraversalMyStock)
            {
                m_pGlobalVar->curCode=m_pMyStockList->at(curRow).code;
                pTableStock->myStockView->setCurrentIndex(pTableStock->m_myStockModel->index(curRow,0));
            }
            else
            {
                m_pGlobalVar->curCode=m_pTableList->at(curRow).code;
                pTableStock->stockTableView->setCurrentIndex(pTableStock->m_tableModel->index(curRow,0));
            }
        }
        \
        lastTime=QDateTime::currentDateTime();
        bWantReqKline=true;
        emit startThreadCandleChart(freq,adjustFlag,true);
        emit startThreadTimeShareChart(true);
        emit startThreadTimeShareTick(true);
        pFundFlow->initAllNews();
    }
    isThreadRunning=false;
}

void MainWindow::EastWebTrade()
{
    if (!this->TradeMutex.tryLock()) {
		return;
	}
}

void MainWindow::SetSpeechRate()
{
    ttscfgDlg ttscfgDlg(m_pGlobalVar,this);
    QObject::connect(&ttscfgDlg, &ttscfgDlg::RateChanged, this, &MainWindow::handleRateChanged);
    QObject::connect(&ttscfgDlg, &ttscfgDlg::VolumeChanged, this, &MainWindow::handleVolumeChanged);
    QObject::connect(&ttscfgDlg, &ttscfgDlg::PitchChanged, this, &MainWindow::handlePitchChanged);
    ttscfgDlg.exec();   
}

void MainWindow::handleRateChanged(float value)
{
    threadNewsReport->tts->setRate(value);
}

//取值范围通常在0.0（静音）到1.0（最大音量）
// ::SendMessage((HWND)(this->winId()), WM_APPCOMMAND, 0, MAKELPARAM(0,FAPPCOMMAND_KEY<<12|APPCOMMAND_VOLUME_UP));
// ::SendMessage((HWND)(this->winId()), WM_APPCOMMAND, 0, MAKELPARAM(0,FAPPCOMMAND_KEY<<12|APPCOMMAND_VOLUME_DOWN));
// ::SendMessage((HWND)(this->winId()), WM_APPCOMMAND, 0, MAKELPARAM(0,FAPPCOMMAND_KEY<<12|APPCOMMAND_VOLUME_MUTE));
void MainWindow::handleVolumeChanged(float value)
{
    // 获取默认音频设备
   // ..QAudioDevice device =  QMediaDevices::defaultAudioOutput();
   //..// QAudioDevice::Mode mode = device.mode();// 只是获取音量，不进行音频播放
    
    
    //QAudioDeviceInfo info = QAudioDeviceInfo::defaultAudioDevice();
    // 获取输入设备
    //QList<QAudioDevice> inputDevices = QMediaDevices::audioInputs();
    // 获取输出设备
    //QList<QAudioDevice> outputDevices = QMediaDevices::audioOutputs();
    
    // 检查设备是否支持获取音量
   // if (info.isValid()) {
     //   qreal volume = info.defaultOutputVolume(); // 获取默认的音量大小
    //}
    float vol = threadNewsReport->tts->volume();
   // if (value ==-1.0f)
     //   ::SendMessage((HWND)(this->winId()), WM_APPCOMMAND, 0, MAKELPARAM(0,FAPPCOMMAND_KEY<<12|APPCOMMAND_VOLUME_MUTE));
    if (value <vol){       
        ::SendMessage((HWND)(this->winId()), WM_APPCOMMAND, 0, MAKELPARAM(0,FAPPCOMMAND_KEY<<12|APPCOMMAND_VOLUME_DOWN));
    }
    else if (value > vol){
        ::SendMessage((HWND)(this->winId()), WM_APPCOMMAND, 0, MAKELPARAM(0,FAPPCOMMAND_KEY<<12|APPCOMMAND_VOLUME_UP));
    }
        
    threadNewsReport->tts->setVolume(value);
}

void MainWindow::handlePitchChanged(float value)
{
  threadNewsReport->tts->setPitch(value);
}    
