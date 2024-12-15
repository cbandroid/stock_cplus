#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <windows.h>
//#include <cstddef>
//#include <stdio.h>
#include <QMainWindow>
#include <qgridlayout.h>
#include "tablestock.h"
#include "fundflow.h"
#include "drawchart.h"
#include "searchstock.h"
#include "requeststocsv.h"
#include "f10view.h"
#include <qmutex.h>
#include <qlabel.h>
#include <QTimer>
#include <QThread>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QRadioButton>
#include <QButtonGroup>
#include <QWheelEvent>
#include <QActionGroup>
#include <QTextBlock>
#include <QComboBox>
#include <QDateEdit>
#include <QMessageBox>
#include <QListWidgetItem>
// #include <complex>
#include <QProcess>
#include "stockinfo.h"


#undef slots
//#include <Python.h>
#define slots Q_SLOTS

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class GlobalVar;
class JSPickStock;
class ThreadTable;
class ThreadIndex;
class ThreadTimeShareTick;
class ThreadTimeShareChart ;
class ThreadNewsReport;
class ThreadCandleChart;
class CTechnique;
class CKData;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
     explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void tradingTimeRunThread();
    void reFlashIndex();
    void reFlashBuySellBaseInfo();
    void setMarket();
    void addRightMenu(int num);
    void showSearchResult();
    void resetKParameter();
    void dealWithFundFlow();
    void fastTrade();
    void delMyStock();
	void updateFeeling(QDate date);
    void handleRateChanged(float value);
    void handleVolumeChanged(float value);
    void handlePitchChanged(float value);
    void ChangeKline();

    void SelectTechAction();
    void tableViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void ontableViewclicked(const QModelIndex &index);
    void onTabFocusChanged();

signals:
    void startThreadTable(bool first);
    void startThreadIndex(bool reset);
    void startThreadTimeShareTick(bool reset);
    void startThreadTimeShareChart(bool reset);
    void startThreadGetNews();
    void startThreadCandleChart(QString freq,QString adjustFlag,bool isFirst);

private:
    Ui::MainWindow *ui;
    GlobalVar *m_pGlobalVar;
    QProcess *QMTProcess;
    TableStock *pTableStock;
    DrawChart *pDrawChart;
    SearchStock *pSearchStock;
    RequestsToCsv *pRequestsToCsv;
    F10View *pF10View;
    FundFlow *pFundFlow;
    JSPickStock *pPickStock;
    ThreadTable *threadTable;
    ThreadIndex *threadIndex;
    ThreadTimeShareTick *threadTimeShareTick;
    ThreadTimeShareChart *threadTimeShareChart;
    ThreadNewsReport *threadNewsReport;
    ThreadCandleChart *threadCandleChart;
    bool isThreadRunning=false;
    bool isFlashBaseInfo=true;
   // PyObject* pModule;
    QDateTime lastTime;
    QThread *thread[6];
    QTimer *tim;
    QLabel *circle;
    QLabel *baseInfoData[18];
    QLabel *buySellPrice[10];
    QLabel *buySellNum[10];
    QLabel *feelingData[8];
    QLabel *stockCode;
    QLabel *stockName;
    QTextBrowser *newsData;
    QWidget *middleWindow;
    QWidget *rightBaseWindow;
    QWidget *rightFundWindow;
    QWidget *searchSmallWindow;
    QWidget *F10SmallWindow;
    QPointF p;
    int hisTimeShareN;
    QString account;
    int ReqKtimeCount;
    int timeCount=-3;
    QString downloadDate;
    QString feelingDate;
    bool changeInTurn=true;
    bool isAsia=true;
    bool isTraversalMyStock=false;
    int ifCanClick;
    QString freq="1"; 
    QString adjustFlag="0";
    QRadioButton *periodAdjust[9];
    QPushButton *F10Info[6];
    QPushButton *fundFlow[10];
    QString m_PreCode="";
    QString qStrPreFreq="";
    QString qStrPreAdjustFlag="";

    QLabel *EPSLabel;
    QLabel *PELabel;
    QLabel *fTitle=new QLabel("标题栏",this);
    QComboBox *periodBox=new QComboBox(this);
    QDateEdit *dateEdit=new QDateEdit(this);
    QDateEdit *dateEdit1=new QDateEdit(this);
    QComboBox *northBox=new QComboBox(this);
    QComboBox *tradedetailBox=new QComboBox(this);
    QComboBox *singleStockBoard=new QComboBox(this);
    QComboBox *openFundBox=new QComboBox(this);
    bool preSort[4]={false,false,false,false};
    float tradePrice;
    int howPosition=0;
    //bool timeShareRunning=false;
    bool AleadyInCandleChartPainter;
    void initGlobalVar();
    void initThread();
    void initInterface();
    void initSettings();
    void initBaseInfoLayout(QGridLayout *baseInfoLayout);
    void initBuySellLayout(QGridLayout *BuySellLayout);
    void initFeelingLayout(QGridLayout *feelingLayout);
    void initSignals();
    void saveCode();
    void flashOldCandleInfo(QMouseEvent *mouseEvent);
    void toInterFace(QString which);
    void toFundFlow();
    void downUpLookStock(QWheelEvent *event);
    void EastWebTrade();
    void SetSpeechRate();
    void KeyMoveUpOrDown(short incIdx);
    QString getKlineDate(time_t date);
    bool bWantReqKline;
    bool bNewCalc;
    unsigned short nTech;
    CTechnique *pTech;
    QMutex TradeMutex;
    QMutex formulaMutex;
protected:
    ModelTableStock *m_pTableModel;
    ModelTableStock *m_pRisingSpeedModel;
    ModelTableStock *m_pMyStockModel;
    ModelTimeShare *m_pTimeShareTickModel;
    QList<StockInfo> *m_pTableList;
    QList<StockInfo> *m_pTableListCopy;
    QList<StockInfo> *m_pRisingSpeedList;
    QList<StockInfo> *m_pMyStockList;
    QList<QStringList> *m_pFundFlowList;

     QStringList *m_pMyStockCode;
     QList<IndexInfo> *m_pIndexList;
     QList<timeShareTickInfo> *m_pTimeShareTickList;
     QList<timeShareChartInfo> *m_pTimeShareChartList;
     QList<timeShareChartInfo> *m_pHisTimeShareChartList;

    CKData *m_pKline;
    CLCore *m_pCLCore;

    QTableView *SelectedtableView;
    QDateTime dldlastTime;

    bool eventFilter(QObject *obj, QEvent *event) override;//事件过滤器
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event)  override;
    void wheelEvent(QWheelEvent *event)  override;
};
#endif // MAINWINDOW_H
