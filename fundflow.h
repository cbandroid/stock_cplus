
#ifndef FUNDFLOW_H
#define FUNDFLOW_H

#include <qlabel.h>
#include <QTableView>
#include <QStandardItemModel>
//#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QFile>
#include <QTextCodec>
//#include <QStringRef>
#include <QJSEngine>
#include <QNetworkCookie>
#include <QDialog>
#include <QPainter>
//#include <complex>
#include <QCheckBox>
#include "stockinfo.h"

class GlobalVar;

class FundFlow : public QTableView
{
public:
    FundFlow(GlobalVar *&pGlobalVar);
//    QNetworkAccessManager *naManager=new QNetworkAccessManager(this);
    QList<QStringList> FundFlowList;
    QStandardItemModel *model=new QStandardItemModel(this);
    QWidget *tableChart=new QWidget(this);
    QDialog *fundFlowChart=new QDialog(this);
    QList<QStringList> fundFlowKList;
    QList<QStringList> fundFlowHKList;
    float maxMinKChart[2];
    float maxMinHKChart[2];
    bool isClick=false;
    float pieData[8]={0.0};
    int whichPiece=-1;
    float pointX[8];
    float pointY[8];
    float degree[8]={0.0};
    float fiveTotal[5]={0.0};
    float twentyTotal[5]={0.0};
    bool isShow[9]={true,true,true,true,true,true,true,false,false}; //9->6 fixed 2024.10.7
    QCheckBox *checkBox[9]; //9->6 fixed 2024.10.7
    QLabel *backGround=new QLabel(fundFlowChart);
    QLabel *vKLine=new QLabel(fundFlowChart);
    QLabel *time=new QLabel(fundFlowChart);
    QLabel *textFund[5];
    void getEastPlateFundFlow(int days);
    void getFundFlowChartData(QString code);
    void drawFundFlowChart(QPainter *painter);
    void getData(int days,const QByteArray &allData);
    void getBoardStock(QList<StockInfo> *&pTableList,QString name);
    void getIntervalHighLow();
    void drawIntervalHighLowChart(QPainter *painter);
    void getStockPoolStrong( QStringList *pMyStockCode,QString date);

    void getNorthFundFlow(QStringList *pMyStockCode,QString days);
    void getDragonTigerList(QStringList *pMyStockCode,int nums,int pages);
    void countSingleStockBoard(QStringList *pMyStockCode,QString nums,int pages);
    void getStockHot(QStringList *pMyStockCode,QList<StockInfo> *pTableListCopy);
    void openFundRank(QString ft,QString sc);
    void getRoyalFlushFundFlow();
    void getNotNormalStock(QStringList *pMyStockCode);
    void getHisTimeShareTick();
    void getTimeShareMin(QList<timeShareChartInfo> *&pHisTimeShareChartList,QString code,QString date);
    void initAllNews();
    void getAnnoucement();
    void getEastAnnoucement();
    void getNews();
    void getEastNews();
    void getGreatEvent();
    bool isGetNews=false;
protected:
    GlobalVar *m_pGlobalVar;
    static void handleReadyRead();
};

#endif // FUNDFLOW_H
