
#ifndef DRAWCHART_H
#define DRAWCHART_H

#include <qdialog.h>
#include <qlabel.h>
#include <qtextbrowser.h>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QListView>
#include <QStringListModel>
#include "stockinfo.h"

class CTechnique;
class CKData;
class GlobalVar;

class DrawChart : public QWidget
{
    Q_OBJECT
public:
    explicit DrawChart( GlobalVar *&pGlobalVar,QWidget *parent = nullptr);
    QWidget *timeShareChart=new QWidget(this);
    QWidget *candleChart=new QWidget(this);
    QWidget *hisTimeShareChart=new QWidget(this);
    QWidget *hisTimeShareChartTitle=new QWidget(hisTimeShareChart);
    QWidget *hisTimeShareChartView=new QWidget(hisTimeShareChart);
    QLabel *title=new QLabel("标题栏",hisTimeShareChartTitle);
    float candleHighLowPoint[5];

    bool isMoveHisTimeShareChart=false;
    int moveUnit=1;
    int Cross_Cursor_Idx;
    QLabel *lblcolPrice;
    QLabel *lblrowTime;
    QLabel *vlblKLine=new QLabel(candleChart);
    QLabel *hlblKLine=new QLabel(candleChart);
    QLabel *lbltimeSharePrice;
    QLabel *lbltimeShareVol;
    QLabel *lbltimeShareTime;
    QLabel vlblLine=QLabel(timeShareChart);
    QLabel hlblLine=QLabel(timeShareChart);
    QLabel *lblhisTimeSharePrice;
    QLabel *lblhisTimeShareVol;
    QLabel *lblhisTimeShareTime;
    QLabel vlblHisLine=QLabel(hisTimeShareChartView);
    QLabel hlblHisLine=QLabel(hisTimeShareChartView);
    QLabel *annLabel[50];
    QDialog *annoucementWindow=new QDialog(candleChart);
    QListView *annTitle=new QListView(annoucementWindow);
    QStringListModel *model=new QStringListModel(annoucementWindow);
    QTextBrowser *annText=new QTextBrowser(annoucementWindow);

    void drawTimeShareChart(QList<timeShareChartInfo>*pTimeShareChartList,QPainter *painter);
    void drawHisTimeShare(QList<timeShareChartInfo> *pHisTimeShareChartList,QPainter *painter);
    void drawCandleChart(QString freq,ushort nTech,bool bNewCalc,CKData *pKline, QPainter *painter);
    float YTransferCandleChart(float y);
    float YTransferCandleChart(int y);
    float YTransferHisTimeChart(float y);
    inline float YTransferTimeChart(float y);

    int TimeChartWidth() const { return   m_rectTimeChart.width(); }
    int TimeChartHeight() const { return   m_rectTimeChart.height(); }
    int TimeChartBottom() const { return   m_rectTimeChart.bottom(); }

    int HisTimeChartWidth() const { return  m_rectHisTimeChart.width() ; }
    int HisTimeChartBottom() const { return  m_rectHisTimeChart.bottom() ; }

    int CandleChartWidth() const { return  m_rectCandleChart.width() ; }
    int CandleChartHeight() const { return  m_rectCandleChart.height() ; }
    int CandleChartBottom() const { return  m_rectCandleChart.bottom(); }
    int XTransferCandleChart(int x);

    void appendAnnNews(int end);
    QString autoWordWrap(QString str,int width);
    void DrawTech(int nTech,int nIndexParam, QPainter *painter);
protected:
    GlobalVar *m_pGlobalVar;
    QString oldSymbol;
    ushort nOldSubTech;
    ushort nOldMainTech;
    float dMin, dMax;
    float close_mean, close_var,cumlow_sum;
    int nTkDays;
    int nUpTkDays;
    int nDownTkDays;
    CKData *m_pKline;
    QRect subRect;
    QRect m_rectCandleChart;
    QRect m_rectHisTimeChart;
    QRect m_rectTimeChart;
    float hisTimeShareHighLowPoint[3];
    bool BuySellTrend(int begin,int end); //股价分析
private:

    void calcHighLowPoint(int begin,int end);
    void calcTSHighLowPoint(QList<timeShareChartInfo> *pHisTimeShareChartList,int begin,int end);
    void DrawTechMA(int nKdCount,int nStart,int nEnd, QPainter *painter,float aveWidth, float &fMin,float &fMax);
    void DrawTechBOLL(int nKdCount,int nStart,int nEnd, QPainter *painter,float aveWidth, float &fMin,float &fMax);
    void DrawTechMACD(int nKdCount,int nStart,int nEnd, QPainter *painter,float aveWidth);
    void DrawTechKDJ(int nKdCount,int nStart,int nEnd, QPainter *painter,float aveWidth);

    void DrawTechVol(int nKdCount,int nStart,int nEnd, QPainter *painter,float aveWidth);

public slots:
    void annClicked(const QModelIndex index)  ;

};

#endif // DRAWCHART_H
