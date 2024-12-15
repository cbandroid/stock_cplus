#include <float.h>
#include "qboxlayout.h"
#include "qpushbutton.h"
#include "qtextcodec.h"
#include "utilityex.h"
#include "globalvar.h"
#include "tech/MA.h"
#include "tech/MACD.h"
#include "tech/KDJ.h"
#include "tech/BOLL.h"
#include "tech/VOLUME.h"
#include "drawchart.h"
#include "NetworkManager.h"

#define FABSMY(x) ((x)<0?-(x):(x))

DrawChart::DrawChart(GlobalVar *&pGlobalVar,QWidget *parent)
    : QWidget{parent}
{
//    hisTimeShareChart->setWindowFlags(hisTimeShareChart->windowFlags() | Qt::WindowStaysOnTopHint);
//    hisTimeShareChart->setWindowFlag(Qt::Popup);
    m_pGlobalVar =pGlobalVar;
    oldSymbol ="";
    Cross_Cursor_Idx=-1;
    nOldSubTech= STT_INVALID;
    nOldMainTech= STT_INVALID;
    pGlobalVar->pMainTech=nullptr;
    pGlobalVar->pSubTech = nullptr;

    QHBoxLayout *titleLayout=new QHBoxLayout(hisTimeShareChartTitle);
    hisTimeShareChartTitle->setLayout(titleLayout);
    hisTimeShareChartTitle->setMaximumHeight(TITLEHEIGHT);
    hisTimeShareChartTitle->setStyleSheet("background-color:rgb(153, 204, 255);");
    title->setStyleSheet("QLabel{font:bold 16px;font-family:微软雅黑;color:rgb(255, 0, 127)}");
    QVBoxLayout *main=new QVBoxLayout();
    main->setSpacing(0);
    main->setContentsMargins(0,0,0,0);
    hisTimeShareChart->setLayout(main);
    main->addWidget(hisTimeShareChartTitle);
    main->addWidget(hisTimeShareChartView);
    QPushButton *close=new QPushButton(hisTimeShareChartTitle);
    QIcon myicon;
    myicon.addFile(tr(":/new/png/png/close.png"));
    close->setIcon(myicon);
    close->setIconSize(QSize(20,20));
    close->setMaximumSize(QSize(30,30));
    titleLayout->addWidget(title);
    titleLayout->addWidget(close);
    hisTimeShareChart->setGeometry(200, 100, 600, 450);
//    hisTimeShareChartView->setMouseTracking(true);
//    hisTimeShareChartTitle->setMouseTracking(true);
    hisTimeShareChart->hide();

    lblcolPrice=new QLabel(candleChart);
    lblrowTime=new QLabel(candleChart);
    lblcolPrice->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    lblrowTime->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    //    colPrice->resize(72,15);
    lblcolPrice->setStyleSheet("color:yellow;font:bold;font-size:18px");
    lblcolPrice->setAlignment(Qt::AlignCenter);
    lblrowTime->resize(95+65,15);
    lblrowTime->setStyleSheet("color:yellow;font:bold;font-size:18px");
    lblrowTime->setAlignment(Qt::AlignCenter);
    hlblKLine->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    vlblKLine->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    lbltimeSharePrice=new QLabel(timeShareChart);
    lbltimeShareVol=new QLabel(timeShareChart);
    lbltimeShareTime=new QLabel(timeShareChart);
    lbltimeSharePrice->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    lbltimeShareVol->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    lbltimeShareTime->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    hlblLine.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    vlblLine.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    timeShareChart->setStyleSheet("color:white;font:bold;font-size:14px");
    lbltimeSharePrice->setAlignment(Qt::AlignCenter);
    lbltimeShareVol->setStyleSheet("color:white;font:bold;font-size:18px");
    lbltimeShareVol->setAlignment(Qt::AlignCenter);
    lbltimeShareTime->setStyleSheet("color:white;font:bold;font-size:18px");
    lbltimeShareTime->setAlignment(Qt::AlignCenter);
    lbltimeShareTime->resize(50,18);
    lblhisTimeSharePrice=new QLabel(hisTimeShareChartView);
    lblhisTimeShareVol=new QLabel(hisTimeShareChartView);
    lblhisTimeShareTime=new QLabel(hisTimeShareChartView);
    lblhisTimeSharePrice->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    lblhisTimeShareTime->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    lblhisTimeShareVol->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    vlblHisLine.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    hlblHisLine.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    hisTimeShareChart->setStyleSheet("color:white;font:bold;font-size:14px");
    lblhisTimeSharePrice->setAlignment(Qt::AlignCenter);
    lblhisTimeShareVol->setStyleSheet("color:white;font:bold;font-size:18px");
    lblhisTimeShareVol->setAlignment(Qt::AlignCenter);
    lblhisTimeShareTime->setStyleSheet("color:white;font:bold;font-size:18px");
    lblhisTimeShareTime->setAlignment(Qt::AlignCenter);
    lblhisTimeShareTime->resize(50,18);

    annoucementWindow->setWindowFlag(Qt::Popup);
    annoucementWindow->resize(1000,600);
    annTitle->setMaximumWidth(300);
    annTitle->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QHBoxLayout *annLayout=new QHBoxLayout(annoucementWindow);
    annoucementWindow->setLayout(annLayout);
//    annTitle->setStyleSheet("QListView::item{height:60px}");
    QFont list_font;
    list_font.setPointSize(14);
    annTitle->setFont(list_font);
    annTitle->setStyleSheet("QListView::item::selected{border:1px solid blue;color:red}");
    annTitle->setWordWrap(true);
    annTitle->setAlternatingRowColors(true);
    annTitle->setSpacing(6);
    annText->setOpenExternalLinks(true);
    annText->setStyleSheet("QTextBrowser{font:20px}");
    annLayout->addWidget(annTitle);
    annLayout->addWidget(annText);

//    annoucementWindow
    QPixmap *pixmap = new QPixmap(":/new/png/png/annoucement.png");
    for (int i=0;i<50;++i)
    {
        annLabel[i]=new QLabel(candleChart);
        annLabel[i]->resize(TIPWIDTH,TIPWIDTH);
        pixmap->scaled(annLabel[i]->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        annLabel[i]->setScaledContents(true);
        annLabel[i]->setPixmap(*pixmap);
        annLabel[i]->setStyleSheet("QToolTip{border:2px solid darkkhaki;padding:5px;border-radius:3px;opacity:200;}");
        annLabel[i]->hide();
    }
    connect(close,&QPushButton::clicked,this,[=](){hisTimeShareChart->close();});
    connect(annTitle, SIGNAL(clicked(QModelIndex)),this, SLOT(annClicked(QModelIndex)));
}

inline float DrawChart::YTransferTimeChart(float y)
{
    return (((m_rectTimeChart.top() - m_rectTimeChart.bottom())*(y - m_pGlobalVar->timeShareHighLowPoint[1])) / (m_pGlobalVar->timeShareHighLowPoint[0] - m_pGlobalVar->timeShareHighLowPoint[1]) + m_rectTimeChart.bottom());
}

//原来以涨跌幅值的形式画分时图
void DrawChart::drawTimeShareChart(QList<timeShareChartInfo> *pTimeShareChartList,QPainter *painter)
{
    // QPainter *painter=new QPainter(timeShareChart);
    if (pTimeShareChartList->empty())
        return;
    int trendsTotal= m_pGlobalVar->trendsTotal;
    float fPreclose = m_pGlobalVar->preClose;
    painter->setPen(Qt::gray);
    painter->setBrush(Qt::black);
    int timeShareChartWidth=timeShareChart->width();
    int timeShareChartHeight=timeShareChart->height();
    int priceH=timeShareChartHeight*12/15;
    painter->drawRect(0,0,timeShareChartWidth-1,timeShareChartHeight-1);
    painter->drawLine(0,priceH,timeShareChartWidth,priceH);
    painter->drawLine(0,0,timeShareChartWidth,0);
    painter->drawLine(0,timeShareChartHeight-BOTTOMHEIGHTEDGE,timeShareChartWidth,timeShareChartHeight-BOTTOMHEIGHTEDGE);

    m_rectTimeChart =QRect(0, TOPHEIGHTEDGE, timeShareChartWidth ,  priceH - TOPHEIGHTEDGE);

    float high=m_pGlobalVar->timeShareHighLowPoint[0];
    float low=m_pGlobalVar->timeShareHighLowPoint[1];
    float stopH=m_pGlobalVar->timeShareHighLowPoint[3]; // %值
    float stopL=m_pGlobalVar->timeShareHighLowPoint[4];

    float lineNums=6;

    float aveWidth=0;

    float maxPrice = (float)(FABSMY( high- fPreclose) > FABSMY(low - fPreclose) ? FABSMY( high - fPreclose) : FABSMY(low - fPreclose));
    high = maxPrice + fPreclose;
    low = fPreclose - maxPrice;
    if ((high -low)<1e-4 )
    {
        high *= 1.02f;
        low *= 0.90f;
    }

    if (trendsTotal!=0)
        aveWidth=(timeShareChartWidth-2*WIDTHEDGE)/trendsTotal;

    float volAveHeight=0;
    if (m_pGlobalVar->timeShareHighLowPoint[2]!=0)
        volAveHeight=( timeShareChartHeight-priceH-TOPHEIGHTEDGE-BOTTOMHEIGHTEDGE)/m_pGlobalVar->timeShareHighLowPoint[2];
//    qDebug()<<volAveHeight;
    int d=60;
    if (trendsTotal>391)
        d=180;
    else if(trendsTotal>241)
        d=90;

    float curXPos;
    for (int i=1;i<=trendsTotal/30;++i){
        curXPos=i*d*aveWidth/2+WIDTHEDGE;
        painter->drawLine(QPointF(curXPos,0),QPointF(curXPos,timeShareChartHeight-BOTTOMHEIGHTEDGE));
    }
    painter->setPen(QColor(79,79,79));
    float fvaluePer = (high - low) / (float)lineNums;
    float fMax = fvaluePer * ((float)lineNums / 2.0f) + fPreclose;
    float fMin =fPreclose - fvaluePer * ((float)lineNums / 2.0f);
    if ((fMax < high) or (fMin >low))
    {
        lineNums+= 2;
    }

    m_pGlobalVar->timeShareHighLowPoint[0] = fMax;
    m_pGlobalVar->timeShareHighLowPoint[1] = fMin;


    QRect rect;
    QString qStrTitle;
    float curYPos;
    int  offset;
    QFontMetrics metr(painter->font());
    for (int i=0;i<lineNums;++i){
       // curYPos=interval*i/(lineNums+1)*aveHeight+TOPHEIGHTEDGE;
        curYPos  = YTransferTimeChart(fvaluePer * (float)(i + 1) + fMin);
        painter->drawLine(QPointF(WIDTHEDGE,  curYPos), QPointF(timeShareChartWidth-WIDTHEDGE,  curYPos));
        qStrTitle=QString::number(fvaluePer * (float)(i + 1) + fMin, 'f', 2);
        offset=metr.boundingRect(qStrTitle).width();
        rect=QRect(timeShareChartWidth-offset-5, curYPos-10,100,20);
        painter->drawText(rect, Qt::AlignLeft, qStrTitle);
    }


    rect=QRect(KWIDTHEDGE,5,240,30);
    painter->setPen(Qt::yellow);
    painter->setFont(QFont("微软雅黑",10,60));
    painter->drawText(rect,Qt::AlignLeft, m_pGlobalVar->curCode+" "+m_pGlobalVar->curName);

   if (pTimeShareChartList->isEmpty())
        return;
    //绘制时间
    int count =pTimeShareChartList->count();
    QList<timeShareChartInfo>::iterator ptr =pTimeShareChartList->begin();
    painter->setPen(Qt::white);
    for (int i=0;i<trendsTotal;i=i+d)
    {
        if (i>=count) // fixed 2024.10.8
            break;

        int offset=WIDTHEDGE;
        if (i>0)
            offset=WIDTHEDGE-15;
        //        qDebug()<<offset+aveWidth*(i-1)<<timeShareChartWidth-d/3-1;
        if ((offset+aveWidth*(i-1))>(timeShareChartWidth-d/3-1))
            offset-=15;
        rect=QRect(offset+aveWidth*(i-1),timeShareChartHeight-BOTTOMHEIGHTEDGE,36,20);
        painter->drawText(rect,Qt::AlignLeft,(ptr+i)->time.right(5));
    }
    //curYPos=high*aveHeight+TOPHEIGHTEDGE;
    if (fPreclose>0)
    {
      //  qDebug()<<"Stop "<<stopH <<","<<stopL;
      curYPos  = YTransferTimeChart(fPreclose);
      int v= curYPos;
      if (v>=20)
        v=v-20;
      rect=QRect(timeShareChartWidth-210,v,200,20);
      painter->drawText(rect,Qt::AlignRight,QString::number(fPreclose,'f',2));

      rect=QRect(timeShareChartWidth-100, priceH,100,20);
      painter->drawText(rect,Qt::AlignLeft,QString::number(m_pGlobalVar->timeShareHighLowPoint[2],'f',0));
      painter->setPen(Qt::blue);

      painter->drawLine(QPointF(0, curYPos), QPointF(timeShareChartWidth, curYPos));
      float c=1.05f*fPreclose;
      if (high>=c)
      {
        painter->setPen(QColor(255,182,193));
        curYPos=YTransferTimeChart(high-5);//(high-5)*aveHeight+TOPHEIGHTEDGE;
        painter->drawLine(QPointF(WIDTHEDGE,  curYPos), QPointF(timeShareChartWidth-WIDTHEDGE,  curYPos));
      }
      c=(1.0f+stopH*0.01f)*fPreclose;
      if (high>=c)
      {
        painter->setPen(QColor(255,0,255));
        curYPos=YTransferTimeChart(high-stopH);//(high-stopH)*aveHeight+TOPHEIGHTEDGE;
        painter->drawLine(QPointF(WIDTHEDGE, curYPos), QPointF(timeShareChartWidth-WIDTHEDGE, curYPos));
      }
      c=1.30f*fPreclose;
      if (high>=c)
      {
        painter->setPen(Qt::red);
        curYPos= YTransferTimeChart(high-10);//(high-10)*aveHeight+TOPHEIGHTEDGE;
        painter->drawLine(QPointF(WIDTHEDGE, curYPos), QPointF(timeShareChartWidth-WIDTHEDGE,  curYPos));
      }
      c=0.97f*fPreclose;
      if (low<=c)
      {
        painter->setPen(QColor(60,179,113));
        curYPos=YTransferTimeChart(high+10);//(high+10)*aveHeight+TOPHEIGHTEDGE;
        painter->drawLine(QPointF(WIDTHEDGE, curYPos), QPointF(timeShareChartWidth-WIDTHEDGE, curYPos));
      }
      c=(1.0f+stopL*0.01f)*fPreclose;
      if (low<=c)
      {
        painter->setPen(QColor(0, 139, 0));
        curYPos=YTransferTimeChart(high-stopL);//(high-stopL)*aveHeight+TOPHEIGHTEDGE;
        painter->drawLine(QPointF(WIDTHEDGE, curYPos), QPointF(timeShareChartWidth-WIDTHEDGE, curYPos));
      }
      c=0.95f*fPreclose;
      if (low<=c)
      {
        painter->setPen(QColor(0, 255, 0));
        curYPos=YTransferTimeChart(high+5);//(high+5)*aveHeight+TOPHEIGHTEDGE;
        painter->drawLine(QPointF(WIDTHEDGE,curYPos ), QPointF(timeShareChartWidth-WIDTHEDGE, curYPos));
      }
    }
    float price1,price2,avePrice1,avePrice2;
    static bool  changecolor=false;
    curXPos=WIDTHEDGE+aveWidth;
    float PreYPos,vol;
    for (int i=0;i< count;++i)
    {
      //  curXPos=WIDTHEDGE+aveWidth*i;
        if (i>=1)
        {
            price1=(ptr+i-1)->price;
            price2=(ptr+i)->price;
            avePrice1=(ptr+i-1)->avePrice;
            avePrice2=(ptr+i)->avePrice;
            painter->setPen(Qt::white);
            PreYPos = YTransferTimeChart(price1);//
            curYPos= YTransferTimeChart(price2);//
            painter->drawLine(QPointF(curXPos-aveWidth,  PreYPos), QPointF(curXPos, curYPos));
            /*if (high!=low)
                painter->drawLine(QPointF(curXPos-aveWidth, (high-price1)*aveHeight+TOPHEIGHTEDGE), QPointF(curXPos, (high-price2)*aveHeight+TOPHEIGHTEDGE));
            else
            {
                if (high>0)
                    painter->drawLine(QPointF(curXPos-aveWidth, TOPHEIGHTEDGE), QPointF(curXPos, TOPHEIGHTEDGE));
                else
                {
                    painter->setPen(QColor(0, 255, 0));
                    painter->drawLine(QPointF(curXPos-aveWidth, priceH-TOPHEIGHTEDGE), QPointF(curXPos, priceH-TOPHEIGHTEDGE));
                }
            }*/
            painter->setPen(Qt::yellow);
            PreYPos = YTransferTimeChart(avePrice1);//
            curYPos= YTransferTimeChart(avePrice2);//
            painter->drawLine(QPointF(curXPos-aveWidth,  PreYPos), QPointF(curXPos, curYPos));
           // if (high!=low)
            //    painter->drawLine(QPointF(curXPos-aveWidth, (high-avePrice1)*aveHeight+TOPHEIGHTEDGE), QPointF(curXPos, (high-avePrice2)*aveHeight+TOPHEIGHTEDGE));

            if (i==count-1)
            {
                QPen pen= painter->pen();
                QBrush oldBrush = painter->brush();
                QColor oldColor = pen.color();
                if ( changecolor){
                    changecolor=false;
                    painter->setPen(Qt::yellow); // 设置画笔颜色
                    painter->setBrush(Qt::yellow);
                }
                else{
                    painter->setPen(Qt::red); // 设置画笔颜色
                    painter->setBrush(QColor(255, 0, 0)); // 设置为红色填充
                    changecolor=true;
                }
               // float curXPos=WIDTHEDGE+aveWidth*i;
                //rect=QRect(curXPos,(high-price2)*aveHeight+TOPHEIGHTEDGE,4, 4); // 圆的位置和大小
                rect=QRect(curXPos,YTransferTimeChart(price2),4, 4);
                painter->drawEllipse(rect);
                painter->setBrush(oldBrush);
                painter->setPen(oldColor) ;
            }
        }

        vol=(ptr+i)->vol;
//        qDebug()<<vol;
        if ((ptr+i)->direct==1)
            painter->setPen(QColor(0, 255, 0));
        else if ((ptr+i)->direct==2)
            painter->setPen(Qt::red);
        else
            painter->setPen(Qt::white);
        painter->drawLine(QPointF(curXPos, timeShareChartHeight-BOTTOMHEIGHTEDGE-vol*volAveHeight), QPointF(curXPos, timeShareChartHeight-BOTTOMHEIGHTEDGE));

        curXPos+=aveWidth;
//        qDebug()<<vol<<timeShareChartHeight-BOTTOMHEIGHTEDGE-vol*volAveHeight;
    }
    // painter->end();
}

float DrawChart::YTransferHisTimeChart(float y) //,float top,float bottom,float fmin,float fmax)//实际数据转换为屏幕坐标Y
{
    return (((m_rectHisTimeChart.top() - m_rectHisTimeChart.bottom())*(y - hisTimeShareHighLowPoint[1])) / (hisTimeShareHighLowPoint[0] - hisTimeShareHighLowPoint[1]) + m_rectHisTimeChart.bottom());
}

void DrawChart::drawHisTimeShare(QList<timeShareChartInfo> *pHisTimeShareChartList,QPainter *painter)
{
    int trendsTotal=pHisTimeShareChartList->count();
    float fPreclose= m_pGlobalVar->hisPreClose;
    painter->setPen(Qt::gray);
    painter->setBrush(Qt::black);
    int timeShareChartWidth=hisTimeShareChartView->width();
    int timeShareChartHeight=hisTimeShareChartView->height();
    int priceH=timeShareChartHeight*12/15;
    painter->drawRect(0,0,timeShareChartWidth-1,timeShareChartHeight-1);
    painter->drawLine(0,priceH,timeShareChartWidth,priceH);
    painter->drawLine(0,0,timeShareChartWidth,0);
    painter->drawLine(0,timeShareChartHeight-BOTTOMHEIGHTEDGE,timeShareChartWidth,timeShareChartHeight-BOTTOMHEIGHTEDGE);
    m_rectHisTimeChart =QRect(KWIDTHEDGE, TOPHEIGHTEDGE, timeShareChartWidth- KWIDTHEDGE ,  priceH - TOPHEIGHTEDGE);

    calcTSHighLowPoint(pHisTimeShareChartList,0,trendsTotal);

    float high=hisTimeShareHighLowPoint[0];
    float low=hisTimeShareHighLowPoint[1];
    int lineNums;

    int numLine = m_rectHisTimeChart.height() / 30+ 1;
    if (numLine % 2 == 1)
    {
        numLine++;
    }
    lineNums= numLine;

    float maxPrice = (float)(FABSMY( high- fPreclose) > FABSMY(low - fPreclose) ? FABSMY( high - fPreclose) : FABSMY(low - fPreclose));
    high = maxPrice + fPreclose;
    low = fPreclose - maxPrice;
    if ((high -low)<1e-4 )
    {
         high *= 1.02f;
         low *= 0.90f;
    }

    float aveWidth=0;
   // float aveHeight=0;

    if (trendsTotal!=0)
        aveWidth=(timeShareChartWidth-2*WIDTHEDGE)/trendsTotal;

    //painter->setPen(Qt::red);
    //painter->drawLine(QPointF(WIDTHEDGE, priceH), QPointF(timeShareChartWidth-WIDTHEDGE, priceH));
    painter->setPen(QColor(79,79,79));
    float fvaluePer = (high - low) / (float)lineNums;
    float fMax = fvaluePer * ((float)lineNums / 2.0f) + fPreclose;
    float fMin =fPreclose - fvaluePer * ((float)lineNums / 2.0f);
    if ((fMax < high) or (fMin >low))
    {
       lineNums+= 2;
    }

    hisTimeShareHighLowPoint[0] = fMax;
    hisTimeShareHighLowPoint[1] = fMin;
    QString qStrTitle;
    QRect rect;
    float curXPos,curYPos,offset;
    QFontMetrics metr(painter->font());
    for (int i=0;i<lineNums;i++){
        curYPos  = YTransferHisTimeChart(fvaluePer * (float)(i + 1) + fMin);//,TOPHEIGHTEDGE,priceH-TOPHEIGHTEDGE,fMin,fMax);
        painter->drawLine(QPointF(WIDTHEDGE, curYPos),
                          QPointF(timeShareChartWidth-WIDTHEDGE, curYPos));
        qStrTitle=QString::number(fvaluePer * (float)(i + 1) + fMin, 'f', 2);
        offset=metr.boundingRect(qStrTitle).width();
        rect=QRect(timeShareChartWidth-offset-5, curYPos-10,100,20);
        painter->drawText(rect, Qt::AlignLeft, qStrTitle);
    }

    float volAveHeight=0;
    if (hisTimeShareHighLowPoint[2]!=0)
        volAveHeight=( timeShareChartHeight-priceH-TOPHEIGHTEDGE-BOTTOMHEIGHTEDGE)/hisTimeShareHighLowPoint[2];

    int d=60;
    for (int i=1;i<trendsTotal/30;++i)
        painter->drawLine(QPointF(i*d*aveWidth/2+WIDTHEDGE+2,0),QPointF(i*d*aveWidth/2+WIDTHEDGE+2,timeShareChartHeight-BOTTOMHEIGHTEDGE));

    painter->setPen(Qt::white);
    for (int i=1;i<trendsTotal;i=i+d)
    {
        int offset=WIDTHEDGE;
        if (i>1)
            offset=WIDTHEDGE-15;
        //        qDebug()<<offset+aveWidth*(i-1)<<timeShareChartWidth-d/3-1;
        if ((i+d)>trendsTotal)
            offset-=15;
        rect=QRect(offset+aveWidth*(i-1),timeShareChartHeight-BOTTOMHEIGHTEDGE,36,20);
        painter->drawText(rect,Qt::AlignLeft,pHisTimeShareChartList->at(i).time.right(5));
    }

    if (fPreclose > 0)
    {
      curYPos  = YTransferHisTimeChart( fPreclose);//,TOPHEIGHTEDGE,priceH-TOPHEIGHTEDGE,fMin,fMax);
    //int v=(high-m_pGlobalVar->hisPreClose)*aveHeight+TOPHEIGHTEDGE;
      float v= curYPos;
      if (v>=20)
        v=v-20;
      rect=QRect(timeShareChartWidth-210,v,200,20);
      painter->drawText(rect,Qt::AlignRight,QString::number( fPreclose));

      rect=QRect(timeShareChartWidth-100, priceH,100,20);
      painter->drawText(rect,Qt::AlignLeft,QString::number(hisTimeShareHighLowPoint[2],'f',0));

      painter->setPen(Qt::blue);
      curYPos  = YTransferHisTimeChart( fPreclose);//,TOPHEIGHTEDGE,priceH-TOPHEIGHTEDGE,fMin,fMax);
      painter->drawLine(QPointF(0, curYPos), QPointF(timeShareChartWidth,  curYPos));

      float c=1.05f* fPreclose;
      if (high>=c)
      {
        painter->setPen(Qt::red);
        curYPos =YTransferHisTimeChart(c);//,TOPHEIGHTEDGE,priceH-TOPHEIGHTEDGE,fMin,fMax);
        painter->drawLine(QPointF(WIDTHEDGE,    curYPos), QPointF(timeShareChartWidth-WIDTHEDGE,    curYPos));

        qStrTitle=QString::number(c, 'f', 2);
        offset=metr.boundingRect(qStrTitle).width();
        rect=QRect(timeShareChartWidth-offset-5, curYPos-10,100,20);
        painter->drawText(rect, Qt::AlignLeft, qStrTitle);

      }
      c=1.1f* fPreclose;
      if (high>=c)
      {
        painter->setPen(QColor(255,182,193));
        curYPos =YTransferHisTimeChart(c);//,TOPHEIGHTEDGE,priceH-TOPHEIGHTEDGE,fMin,fMax);
        painter->drawLine(QPointF(WIDTHEDGE,    curYPos), QPointF(timeShareChartWidth-WIDTHEDGE,    curYPos));
        qStrTitle=QString::number(c, 'f', 2);
        offset=metr.boundingRect(qStrTitle).width();
        rect=QRect(timeShareChartWidth-offset-5, curYPos-10,100,20);
        painter->drawText(rect, Qt::AlignLeft, qStrTitle);
      }
      c=0.9f*fPreclose;
      if (low<=c)
      {
        painter->setPen(QColor(60,179,113));
        curYPos =YTransferHisTimeChart(c);//,TOPHEIGHTEDGE,priceH-TOPHEIGHTEDGE,fMin,fMax);
        painter->drawLine(QPointF(WIDTHEDGE,    curYPos), QPointF(timeShareChartWidth-WIDTHEDGE,    curYPos));
        qStrTitle=QString::number(c, 'f', 2);
        offset=metr.boundingRect(qStrTitle).width();
        rect=QRect(timeShareChartWidth-offset-5, curYPos-10,100,20);
        painter->drawText(rect, Qt::AlignLeft, qStrTitle);
      }
      c=0.95f*fPreclose;
      if (low<=c)
      {
        painter->setPen(QColor(0, 255, 0));
        curYPos =YTransferHisTimeChart(c);//,TOPHEIGHTEDGE,priceH-TOPHEIGHTEDGE,fMin,fMax);
        painter->drawLine(QPointF(WIDTHEDGE,    curYPos), QPointF(timeShareChartWidth-WIDTHEDGE,    curYPos));
        qStrTitle=QString::number(c, 'f', 2);
        offset=metr.boundingRect(qStrTitle).width();
        rect=QRect(timeShareChartWidth-offset-5, curYPos-10,100,20);
        painter->drawText(rect, Qt::AlignLeft, qStrTitle);
      }
    }
    QList<timeShareChartInfo>::Iterator ptr=pHisTimeShareChartList->begin();
    float PreY,vol,avePrice1,avePrice2,price1,price2;
    curXPos=WIDTHEDGE+aveWidth;
    for (int i=0;i<trendsTotal;++i)
    {
       vol=(ptr+i)->vol;
        if (i>=1)
        {
            price1=(ptr+i-1)->price;
            price2=(ptr+i)->price;

            avePrice1=(ptr+i-1)->avePrice;
            avePrice2=(ptr+i)->avePrice;
            painter->setPen(Qt::white);

             PreY =YTransferHisTimeChart( price1);//,TOPHEIGHTEDGE,priceH-TOPHEIGHTEDGE,fMin,fMax);
             curYPos =YTransferHisTimeChart( price2);//,TOPHEIGHTEDGE,priceH-TOPHEIGHTEDGE,fMin,fMax);
             painter->drawLine(QPointF(curXPos-aveWidth,  PreY), QPointF( curXPos, curYPos));

            /*if (high!=low)
                painter->drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), (high-price1)*aveHeight+TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, (high-price2)*aveHeight+TOPHEIGHTEDGE));
            else
            {
                if (high>0)
                    painter->drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, TOPHEIGHTEDGE));
                else
                {
                    painter->setPen(QColor(0, 255, 0));
                    painter->drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), priceH-TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, priceH-TOPHEIGHTEDGE));
                }
            }*/
            painter->setPen(Qt::yellow);
           // if (high!=low)
            PreY =YTransferHisTimeChart( avePrice1);//,TOPHEIGHTEDGE,priceH-TOPHEIGHTEDGE,fMin,fMax);
            curYPos =YTransferHisTimeChart(avePrice2);//,TOPHEIGHTEDGE,priceH-TOPHEIGHTEDGE,fMin,fMax);
            painter->drawLine(QPointF( curXPos-aveWidth,  PreY), QPointF( curXPos,  curYPos));

            if (price2<price1)
                painter->setPen(QColor(0, 255, 0));
            else if (price2>price1)
                painter->setPen(Qt::red);
            else
                painter->setPen(Qt::white);
            painter->drawLine(QPointF( curXPos, timeShareChartHeight-BOTTOMHEIGHTEDGE-vol*volAveHeight), QPointF( curXPos, timeShareChartHeight-BOTTOMHEIGHTEDGE));

        }
        else {
            if ((ptr+1)->price>m_pGlobalVar->hisPreClose)
                painter->setPen(Qt::red);
            else if ((ptr+1)->price<m_pGlobalVar->hisPreClose)
                painter->setPen(QColor(0, 255, 0));
            else
                painter->setPen(Qt::white);
            painter->drawLine(QPointF( curXPos, timeShareChartHeight-BOTTOMHEIGHTEDGE-vol*volAveHeight), QPointF( curXPos, timeShareChartHeight-BOTTOMHEIGHTEDGE));
        }
         curXPos+=aveWidth;
    }
}

//const float goldvv[7]={0,1,0.382,0.5,0.618,1.618,2.618};
//const float goldvv[5]={0.382,0.5,0.618,1.618,2.618};
const float goldvv[3]={0.382,0.5,0.618};

#define  FIGURE_HIGH  50

int DrawChart::XTransferCandleChart(int x)//把x坐标转换为对应的数组下标
{
    //int nIdx = (x * m_pGlobalVar->KRange) / m_rectCandleChart.width();
    int nIdx = (int)(((float)x -  KWIDTHEDGE)*m_pGlobalVar->KRange/ (float)m_rectCandleChart.width());
    return nIdx;
}

float DrawChart::YTransferCandleChart(int y)//屏幕坐标Y转换为实际数据
{
   return (((float)(candleHighLowPoint[0] - candleHighLowPoint[1]))*(y - m_rectCandleChart.bottom()) / (m_rectCandleChart.top() - m_rectCandleChart.bottom()) + candleHighLowPoint[1]);
}

float DrawChart::YTransferCandleChart(float y)
{
    return (((m_rectCandleChart.top() - m_rectCandleChart.bottom())*(y - candleHighLowPoint[1])) / (candleHighLowPoint[0] - candleHighLowPoint[1]) + m_rectCandleChart.bottom());
}

#define  BLANK_EDGE  30.0f

void DrawChart::drawCandleChart(QString freq, ushort nTech, bool bNewCalc, CKData *pKline,CLCore *pCLCore, QPainter* painter)
{    
   // qDebug()<<total<<","<<m_pGlobalVar->KRange<<","<<m_pGlobalVar->offsetLocal<<",begin "<<begin<<", end "<<end;
    //    qDebug()<<total<<m_pGlobalVar->KBegin<<end;
    painter->setPen(Qt::gray);
    painter->setBrush(Qt::black);
    int candleChartWidth = candleChart->width();
    int canldeChartHeight = candleChart->height();
    int priceH = canldeChartHeight * 12 / 15;
    //画图区
    m_rectCandleChart = QRect(KWIDTHEDGE, KTOPHEIGHTEDGE, candleChartWidth - KWIDTHEDGE - KRIGHTWIDTHEDGE-BLANK_EDGE , priceH - BOTTOMHEIGHTEDGE - KTOPHEIGHTEDGE);
    painter->drawRect(0, 0, candleChartWidth, canldeChartHeight);
    // painter->setPen(QPen(Qt::red, 2));
    // painter->drawLine(0,priceH,candleChartWidth-KRIGHTWIDTHEDGE,priceH);
    // painter->setPen(QPen(Qt::green, 2));
    painter->drawLine(0, m_rectCandleChart.bottom(), candleChartWidth - KRIGHTWIDTHEDGE, m_rectCandleChart.bottom());

    QRect rect;
    subRect = QRect(KWIDTHEDGE, priceH + TOPHEIGHTEDGE, candleChartWidth - KWIDTHEDGE - KRIGHTWIDTHEDGE-BLANK_EDGE , canldeChartHeight - priceH - TOPHEIGHTEDGE);
    rect = QRect(KWIDTHEDGE, 20, 320, 30);
    QString freqname;
    if (freq == "1" or freq == "7")
        freqname = "<1分钟>";
    else if (freq == "5")
        freqname = "<5分钟>";
    else if (freq == "15")
        freqname = "<15分钟>";
    else if (freq == "30")
        freqname = "<30分钟>";
    else if (freq == "60")
        freqname = "<60分钟>";
    else if (freq == "102")
        freqname = "<周K线>";
    else if (freq == "103")
        freqname = "<月K线>";
    else if (freq == "104")
        freqname = "<季K线>";
    else if (freq == "105")
        freqname = "<年K线>";
    else
        freqname = "<日K线>";
    painter->setPen(Qt::yellow);
    painter->setFont(QFont("微软雅黑", 12, 300));
    painter->drawText(rect, Qt::AlignLeft, m_pGlobalVar->curCode + " " + m_pGlobalVar->curName + " " + freqname);

    if (pKline==nullptr or pKline->empty())
    {
        painter->setPen(QColor(79, 79, 79));
        painter->drawLine(QPointF(candleChartWidth - KRIGHTWIDTHEDGE, 0),
            QPointF(candleChartWidth - KRIGHTWIDTHEDGE, canldeChartHeight));

        return;
    }
    m_pKline=pKline;
    int nKdCount = (int)pKline->size();
    int begin = nKdCount - m_pGlobalVar->offsetLocal;
    if (begin < 0)
        begin = 0;

    int end = begin + m_pGlobalVar->KRange ;//- m_pGlobalVar->offsetEnd;
      if ( end> nKdCount) // fixed 2024.10.7
        end =  nKdCount;


    calcHighLowPoint(begin, end);
    float highPoint = candleHighLowPoint[0];
    float lowPoint = candleHighLowPoint[1];
    if ((highPoint -lowPoint)<1e-4 )
    {
       highPoint *= 1.02f;
       lowPoint *= 0.90f;
    }
    float aveHeightVol = 0;
    //主图显示BOLL, 副图KDJ,MACD,成交量可选，缺省MACD
    if (freq == "1" or freq == "5" or freq == "30")
    {
        if (nOldMainTech != STT_BOLL)
        {
            CMA* ma =(CMA *)m_pGlobalVar->pMainTech;
            if (ma!=nullptr)
               delete ma;
            m_pGlobalVar->pMainTech=nullptr;
        }

        if (m_pGlobalVar->pMainTech == nullptr)
        {

            m_pGlobalVar->pMainTech = CTechnique::create(STT_BOLL,pKline);
            nOldMainTech = STT_BOLL;
        }
        else
        {
            if (bNewCalc)
                m_pGlobalVar->pMainTech->set_kdata(pKline);
        }
        CBOLL* boll = (CBOLL*)m_pGlobalVar->pMainTech;
        float dMin,dMax;
        boll->min_max_info(begin, end - 1, &dMin, &dMax);

         if (dMin<lowPoint)
            lowPoint=dMin;
        if (dMax> highPoint)
            highPoint=dMax;
    }
    else
    {
        if (nOldMainTech != STT_MA)
        {
            CBOLL* boll =( CBOLL*)m_pGlobalVar->pMainTech;
            if (boll!=nullptr)
               delete boll;
            m_pGlobalVar->pMainTech=nullptr;
        }

        if (m_pGlobalVar->pMainTech == nullptr)
        {            
            m_pGlobalVar->pMainTech = CTechnique::create(STT_MA,pKline);
            nOldMainTech = STT_MA;
        }
        else
        {
            if (bNewCalc)
                m_pGlobalVar->pMainTech->set_kdata(pKline);
        }
        CMA* ma = (CMA*)m_pGlobalVar->pMainTech;
        float dMin,dMax;
        ma->min_max_info(begin, end - 1, &dMin, &dMax);

         if (dMin<lowPoint)
            lowPoint=dMin;
        if (dMax> highPoint)
            highPoint=dMax;
    }

    short nDot = 2;
    //if (nTech != STT_VOLUME)
    {
        if (nOldSubTech != nTech)
        {
            if (m_pGlobalVar->pSubTech != nullptr)
                delete m_pGlobalVar->pSubTech;
            m_pGlobalVar->pSubTech = CTechnique::create(nTech, pKline);
        }
        else
        {
            if (bNewCalc)
                m_pGlobalVar->pSubTech->set_kdata(pKline);
        }
        nOldSubTech = nTech;

        switch (nTech) {
        case STT_VOLUME:
        {
            CVOLUME* pVol = (CVOLUME*)m_pGlobalVar->pSubTech;
            pVol->min_max_info(0, nKdCount - 1, &dMin, &dMax);
            if (dMin != dMax)
                aveHeightVol = subRect.height() / (dMax - dMin);
        }
        break;

        case STT_KDJ:
        {
            CKDJ* kdj = (CKDJ*)m_pGlobalVar->pSubTech;
            kdj->min_max_info(0, nKdCount- 1, &dMin, &dMax);
            if (dMin != dMax)
                aveHeightVol = subRect.height() / (dMax - dMin);

        }
        break;
        case STT_MACD:
        {
            CMACD* macd = (CMACD*)m_pGlobalVar->pSubTech;
            macd->min_max_info(0, nKdCount- 1, &dMin, &dMax);
            if (dMin != dMax)
                aveHeightVol = subRect.height() / (dMax - dMin);

           nDot = 4;
        }
        break;

        default:
            break;
        }
    }
    float finterval = highPoint - lowPoint;

    int numLine = m_rectCandleChart.height() / FIGURE_HIGH + 1;  
    float fvaluePer;
    fvaluePer = finterval / (float)numLine ;

    int num_b = (int)floor(lowPoint / fvaluePer) ;


    int num_all = (int)(highPoint / fvaluePer) - num_b + 1;


    float aveWidth = (float)m_rectCandleChart.width() / (float)(m_pGlobalVar->KRange);
    //        qDebug()<<aveWidth;
    int w = aveWidth / 4;
    if ((w % 2) == 0)
        w++;
    float aveHeight = 0;
    float  preXPos, curXPos;
    float preYPos, curYPos;
    if (highPoint != lowPoint) {
        aveHeight = m_rectCandleChart.height() / finterval;
    }

    painter->setPen(QColor(79, 79, 79));
    painter->drawLine(QPointF(candleChartWidth - KRIGHTWIDTHEDGE, 0),
        QPointF(candleChartWidth - KRIGHTWIDTHEDGE, canldeChartHeight));

    for (int i = 0; i <= num_all; i++)
    {
        painter->setPen(QColor(79, 79, 79));
        curXPos = candleChartWidth - KRIGHTWIDTHEDGE;
        //curYPos = YTransfer(highPoint,lowPoint,fvaluePer * (num_b + i));

        curYPos = (highPoint - fvaluePer * (num_b + i)) * aveHeight + m_rectCandleChart.top();


        painter->drawLine(QPointF(KWIDTHEDGE, curYPos), QPointF(curXPos + 5, curYPos));
        rect = QRect(curXPos, curYPos - 10, 60, 20);
        painter->setPen(QColor(255, 165, 0));
        painter->drawText(rect, Qt::AlignCenter, QString::number(fvaluePer * (num_b + i), 'f', 2));
    }

        finterval = dMax - dMin;
        if (finterval != 0)
            aveHeightVol = subRect.height() / finterval;

        num_all = 7;

    float valuePer = finterval / (num_all- 1);

    for (int i = 0; i < num_all; i++)
    {
        painter->setPen(QColor(179, 79, 79));
        curXPos = candleChartWidth - KRIGHTWIDTHEDGE;

        curYPos = subRect.bottom() - valuePer * i * aveHeightVol;

        painter->drawLine(QPointF(KWIDTHEDGE, curYPos),
            QPointF(curXPos + 5, curYPos));
        rect = QRect(curXPos, curYPos - 10, 60, 20);
        painter->setPen(QColor(255, 165, 0));
        if (nTech == STT_VOLUME)
           painter->drawText(rect, Qt::AlignCenter, format_conversion(valuePer * i + dMin));
        else
           painter->drawText(rect, Qt::AlignCenter, QString::number(valuePer * i + dMin,'f', nDot));       
    }



    float open, close, high, low, vol, Y, height;

    std::vector<CL::Kline>::iterator ptr = pKline->begin();
    static bool changecolor = false;

    float preMA;
    float MA;
    float XOffset = KWIDTHEDGE + aveWidth / 2;

    for (int nIdx = begin; nIdx < end; ++nIdx)
    {
        open = (ptr + nIdx)->open;

        close = (ptr + nIdx)->close;

        high = (ptr + nIdx)->high;
        low = (ptr + nIdx)->low;
       // vol = (ptr + nIdx)->vol;
        if (open > close)
        {
            Y = (highPoint - open) * aveHeight;
            height = (open - close) * aveHeight;

            // Y = YTransfer(highPoint,lowPoint,highPoint);
            // height=YTransfer(highPoint,lowPoint,open);

            painter->setPen(QColor(2, 148, 255));
            painter->setBrush(QColor(2, 148, 255));
        }
        else
        {
            Y = (highPoint - close) * aveHeight;
            height = (close - open) * aveHeight;
            painter->setPen(Qt::white);
            painter->setBrush(Qt::white);

        }

        curXPos = XOffset +(nIdx - begin)* m_rectCandleChart.width() / m_pGlobalVar->KRange;
        if (high != low) {

            painter->drawLine(QPointF(curXPos, (highPoint - high) * aveHeight +  m_rectCandleChart.top()),
                QPointF(curXPos, (highPoint - low) * aveHeight +  m_rectCandleChart.top()));
        }
        painter->drawRect(curXPos - w, Y +  m_rectCandleChart.top(), 2 * w, height);

        if (nIdx == nKdCount - 1)
        {
            QPen pen = painter->pen();
            QBrush oldBrush = painter->brush();
            QColor oldColor = pen.color();
            if (changecolor) {
                changecolor = false;
                painter->setPen(Qt::yellow); // 设置画笔颜色
                painter->setBrush(Qt::yellow);
            }
            else {
                painter->setPen(Qt::red); // 设置画笔颜色
                painter->setBrush(QColor(255, 0, 0)); // 设置为红色填充
                changecolor = true;
            }
            rect = QRect(curXPos - 3, (highPoint - close) * aveHeight +  m_rectCandleChart.top()+ 5, 6, 6); // 圆的位置和大小
            painter->drawEllipse(rect);
            painter->setBrush(oldBrush);
            painter->setPen(oldColor);
        }

    }


    if (freq == "1" or freq == "5" or freq == "30")
    {
        DrawTechBOLL(nKdCount, begin, end - 1, painter, aveWidth,  lowPoint, highPoint);
    }
    else{
        DrawTechMA(nKdCount, begin, end - 1, painter, aveWidth,  lowPoint, highPoint);
    }
    switch (nTech) {
    case STT_VOLUME:
        DrawTechVol(nKdCount, begin, end - 1, painter, aveWidth);
        break;
    case STT_KDJ:
        DrawTechKDJ(nKdCount, begin, end - 1, painter, aveWidth);
        break;
    case STT_MACD:
        DrawTechMACD(nKdCount, begin, end - 1, painter, aveWidth);
        break;

    default:
        break;
    }

    QBrush oldBrush = painter->brush();
    //标注顶底分型

//    qDebug()<<candleHighLowPoint[3]<<candleHighLowPoint[4];
  //  painter->setPen(Qt::red);
  //  painter->setFont(QFont("微软雅黑", 15, 700));
    //    qDebug()<<KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[3]<<m_pGlobalVar->format_conversion(highPoint);

        /*if (not m_pGlobalVar->offsetEnd)
        {

            if (highPoint!=0)
            {
                int yPos = KTOPHEIGHTEDGE;

                if (candleHighLowPoint[3]>=m_pGlobalVar->KRange/2)
                {
                    rect=QRect(KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[3]-100,yPos,100,30);
                    painter->drawText(rect,Qt::AlignRight,QString::number(highPoint,'f',2));
                }
                else
                {
                    rect=QRect(KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[3],yPos,100,30);
                    painter->drawText(rect,Qt::AlignLeft,QString::number(highPoint,'f',2));
                }
            }
            painter->setPen(QColor(0, 255, 0));
            if (lowPoint!=0)
            {
               int yPos =(highPoint-lowPoint)*aveHeight+KTOPHEIGHTEDGE;
                if (candleHighLowPoint[4]>=m_pGlobalVar->KRange/2)
                {
                    rect=QRect(KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[4]-100,yPos ,100,30);
                    painter->drawText(rect,Qt::AlignRight,QString::number(lowPoint,'f',2));
                }
                else
                {
                    rect=QRect(KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[4],yPos ,100,30);
                    painter->drawText(rect,Qt::AlignLeft,QString::number(lowPoint,'f',2));
                }
            }
        }*/
    appendAnnNews(end);
    oldSymbol = m_pGlobalVar->curCode;
}

void DrawChart::calcHighLowPoint(int begin,int end)
{
    candleHighLowPoint[0]=-9.0E20f;
    candleHighLowPoint[1]=9.0E20f;
    candleHighLowPoint[2]=-9.0E20f;
    float fValue;
    std::vector<CL::Kline>::const_iterator ptr = m_pKline->begin();
    for (int i=begin;i<end;++i)
    {
        fValue=( ptr+i)->high;
        if (fValue>candleHighLowPoint[0])
        {
            candleHighLowPoint[0]=fValue;
            candleHighLowPoint[3]=i-begin; // pos
        }
        fValue=( ptr+i)->low;
        if (fValue<candleHighLowPoint[1])
        {
            candleHighLowPoint[1]=fValue;
            candleHighLowPoint[4]=i-begin; // pos
        }
        fValue=( ptr+i)->vol;
        if (fValue>candleHighLowPoint[2])
            candleHighLowPoint[2]=fValue;
    }
    if (candleHighLowPoint[2]<0)
        candleHighLowPoint[2]=0;
}

void DrawChart::calcTSHighLowPoint(QList<timeShareChartInfo> *pHisTimeShareChartList,int begin, int end)
{
    hisTimeShareHighLowPoint[0]= -9.0E20f;
    hisTimeShareHighLowPoint[1]= 9.0E20f;
    hisTimeShareHighLowPoint[2]= -9.0E20f;
    float fValue;
    QList<timeShareChartInfo>::iterator  ptr=pHisTimeShareChartList->begin();
    for (int i=begin;i<end;i++)
    {
        fValue= (ptr+i)->price;
        if (fValue>hisTimeShareHighLowPoint[0])
        {
            hisTimeShareHighLowPoint[0]=fValue;
        }        
        if (fValue<hisTimeShareHighLowPoint[1])
        {
            hisTimeShareHighLowPoint[1]=fValue;
        }
        fValue= (ptr+i)->vol;
        if (fValue>hisTimeShareHighLowPoint[2])
            hisTimeShareHighLowPoint[2]=fValue;
    }

    if (hisTimeShareHighLowPoint[2]<0) // vol
        hisTimeShareHighLowPoint[2]=0;
}

void DrawChart::appendAnnNews(int end)
{
    if (m_pKline==nullptr)
        return;
    int m=0;
    QString content="";
    int pos=0;
    int n=m_pGlobalVar->KRange-1;
    int nKdCount =(int)m_pKline->size();
    if ((nKdCount-1)<n)
        n=nKdCount-1;
    if (n<0){
        return;
    }
    int b=end-1-KRANGE;
    if (b<0){
        b=0;
    }
    int count =m_pGlobalVar->annoucementList.count();
    QString c,l,t,time,qDate;
    int result;
    for (int j=end-1;j>=b;--j)
    {    
        for (int i=pos;i<count;++i)
        {
            c=m_pGlobalVar->annoucementList.at(i)[0];
            l=m_pGlobalVar->annoucementList.at(i)[1];
            t=m_pGlobalVar->annoucementList.at(i)[2];
            time=t.mid(1,10);

            if (j>=0 and j< nKdCount){
                QDateTime  dateTime = QDateTime::fromSecsSinceEpoch( m_pKline->at(j).date);
                qDate =dateTime.toString("yyyy-MM-dd");
                result=QString::compare(qDate,time);
             // result=QString::compare(m_pKline->at(j).date,time);
            }
            else
              result=0;

            if (result>0)
            {
                if (not content.isEmpty())
                {
                    annLabel[m]->show();
                    annLabel[m]->setToolTip(content);
                    int posX=(2*n+1)*(candleChart->width()-2*KWIDTHEDGE-KRIGHTWIDTHEDGE)/(2*m_pGlobalVar->KRange);
                    annLabel[m]->move(posX+KWIDTHEDGE-TIPWIDTH/2,10);
                    ++m;
                    pos=i;
                    content="";
                    if (m>49)
                        return;
                }
                break;
            }
            else
            {
                if (content.isEmpty())
                    content=t+l+"\n"+autoWordWrap(c,20);
                else
                    content=content+"\n"+t+l+"\n"+autoWordWrap(c,20);
            }
        }
        --n;
    }
    for (int i=m;i<50;++i)
        annLabel[i]->hide();
}

QString DrawChart::autoWordWrap(QString str, int width)
{
    int curPost=0;
    QString s="";
    while(1)
    {
        QString temp =str.mid(curPost,width);
        s=s+temp;
        curPost=curPost+width;
        if (curPost > str.size())
        {
            s=s+str.mid(curPost,width);
            break;
        }
        else
            s=s+"\n";
    }
    return s;
}

QString retain(QString s,QString b,QString e)
{
    QString Hlabel="<"+e;
    QString Elabel="</"+e+">";
    int headPos=s.indexOf(b);
    int curPos=headPos;
//    QString head=s.mid(0,headPos);
    int endPos=s.indexOf(Elabel,headPos);
    while(1)
    {
        if ((curPos=s.indexOf(Hlabel,curPos+1))!=-1)
        {
            if (curPos>endPos)
                break;
            endPos=s.indexOf(Elabel,endPos+1);
        }
        else
            break;
    }
    return (s.mid(headPos,endPos+Elabel.length()-headPos));
}

QString peel(QString s,QString b,QString e)
{
    QString Hlabel="<"+e;
    QString Elabel="</"+e+">";
    int headPos=s.indexOf(b);
    int curPos=headPos;
    QString head=s.mid(0,headPos);
    int endPos=s.indexOf(Elabel,headPos);
    while(1)
    {
        if ((curPos=s.indexOf(Hlabel,curPos+1))!=-1)
        {
            if (curPos>endPos)
                break;
            endPos=s.indexOf(Elabel,endPos+1);
        }
        else
            break;
    }
    return (head+s.mid(endPos+Elabel.length(),-1));
}

void DrawChart::annClicked(const QModelIndex index)
{
    int nCount =m_pGlobalVar->annoucementList.count();
    if (nCount==0)
        return;
    int nIndex = index.row();
    if ( nIndex >=nCount)
        nIndex =nCount-1;


    if (m_pGlobalVar->annoucementList.at(nIndex)[1]=="[新闻]")
    {
        QString s="<font size=\"4\" color=blue>"+m_pGlobalVar->annoucementList.at(nIndex)[4]
                +/*m_pGlobalVar->annoucementList.at(nIndex)[2]+*/"\r\n"+"</font>"
                +"<font size=\"3\">"+m_pGlobalVar->annoucementList.at(nIndex)[5]+"\r\n"+"</font>"
                +"<span> <a href="+m_pGlobalVar->annoucementList.at(nIndex)[3]+">"+
                +"原文"+"</a></span>";
        annText->setText(s);
    }
    else if(m_pGlobalVar->annoucementList.at(nIndex)[1]=="[大事提醒]")
    {
        QString s="<font size=\"4\" color=blue>"+m_pGlobalVar->annoucementList.at(nIndex)[0]
                    +"\r\n"+"</font>"
                    +"<font size=\"3\">"+m_pGlobalVar->annoucementList.at(nIndex)[3]+"\r\n"+"</font>";
        annText->setText(s);
    }
    else
    {
       QString html;
       QByteArray allData;

       QString url=m_pGlobalVar->annoucementList.at(nIndex)[3];
       NetworkManager networkManager;
       allData = networkManager.getSync<QByteArray>(QUrl(url));
       if (m_pGlobalVar->annoucementList.at(nIndex)[1]=="[证券时报]")
       {
           QTextCodec *codec = QTextCodec::codecForName("utf-8");
           html=codec->toUnicode(allData);
           html=retain(html,"<div class=\"detail-info\"","div")+retain(html,"<div class=\"detail-content\"","div");
           html=peel(html,"<div class=\"fenlei1\"","div");
           html=peel(html,"<div class=\"fenlei2\"","div");

   //        html=peel(html,"<div class=\"social-bar\"","div");
   //        html=retain(html,"<div class=\"main-content text-large\"","div");
   //        html=peel(html,"<div class=\"bullet\"","div");
   //        html=peel(html,"<div class=\"bullet\"","div");
           annText->setText(html);
       }
       else if (m_pGlobalVar->annoucementList.at(nIndex)[1]=="[公告]")
       {
           QTextCodec *codec = QTextCodec::codecForName("gbk");
           html=codec->toUnicode(allData);
           html=peel(html,"<div class=\"head\"","div");
           html=peel(html,"<div class=\"search\"","div");
           html=peel(html,"<div class=\"w1200 center\"","div");
           html=peel(html,"<div class=\"footer mg0\"","div");
           annText->setText(html);
       }
       else if (m_pGlobalVar->annoucementList.at(nIndex)[1]=="[东方公告]")
            annText->setText("<span> <a href="+m_pGlobalVar->annoucementList.at(nIndex)[3]+">"+
                         "<font size=\"4\">"+m_pGlobalVar->annoucementList.at(nIndex)[0]+"</font>"+"</a> </span>");
    }
}


void DrawChart::DrawTechMA(int nKdCount,int nStart,int nEnd, QPainter *painter,float aveWidth, float &fMin,float &fMax)
{
    CMA *pTech = (CMA *)m_pGlobalVar->pMainTech;
    if (pTech==nullptr)
        return;
    float	dMin = 0, dMax = 0;
    if(  !pTech->min_max_info( nStart, nEnd, &dMin, &dMax ) )
    {
        return;
    }
        
    if( (dMax - dMin )< 1e-4 )
        return;

    float aveHeight= m_rectCandleChart.height()/(fMax-fMin);
    float curXPos;//=KWIDTHEDGE+aveWidth/2;
    float preXPos;//=KWIDTHEDGE-aveWidth/2;
    float Xoffset=KWIDTHEDGE+aveWidth/2;
    // Draw
    int nParamNum = (int)pTech->m_adwMADays.size();
    //qDebug() <<"MA " << nParamNum <<"," << dMax <<","<<dMin;

    bool	bHasLast = false;
    float	dPreNow,dNow = dMin;
    int yBasePos =m_rectCandleChart.bottom();

    int	nIndexParam	=	(Cross_Cursor_Idx >= 0 and  Cross_Cursor_Idx<nKdCount) ? Cross_Cursor_Idx : nKdCount-1 ;
    float	dParam;

    QString qText;
    curXPos=840;
    painter->setPen(Qt::yellow);
    painter->setFont(QFont("微软雅黑",12,60));
    QRect rect;
    QRect rectfont;
    QFontMetrics metr(painter->font());
    for( int k=0; k<nParamNum; k++ )
    {
        if( pTech->calc( &dParam, nIndexParam,pTech->m_adwMADays[k],false ) )
        {
            if ((k% nParamNum)==0)
                painter->setPen(Qt::white);
            else if ((k% nParamNum)==1)
                painter->setPen(Qt::cyan);
            else
                painter->setPen(Qt::magenta);
            qText=QString("MA%1 %2 ").arg(pTech->m_adwMADays[k]).arg(dParam ,0,'f',2);
            rectfont=metr.boundingRect(qText);
            rect=QRect(curXPos,20,rectfont.width()+5,30);
            painter->drawText(rect,Qt::AlignLeft, qText);
            curXPos+=rectfont.width()+(k+1)*15;

        }
    }

    for( int k=0; k< nParamNum; k++ )
    {

        bHasLast = false;

        for( int nIndex=0; nIndex< nKdCount; nIndex++ )
        {

            if( pTech->calc( &dNow, nIndex,pTech->m_adwMADays[k], bHasLast ) )
            {
                  if( bHasLast and nIndex >nStart and nIndex<=nEnd)
                {
                    if ((k% nParamNum)==0)
                       painter->setPen(Qt::white);
                    else if ((k% nParamNum)==1)
                       painter->setPen(Qt::cyan);
                    else
                       painter->setPen(Qt::magenta);

                    preXPos =Xoffset+(nIndex-nStart-1)* m_rectCandleChart.width() / m_pGlobalVar->KRange;
                    curXPos =Xoffset+(nIndex-nStart)* m_rectCandleChart.width() / m_pGlobalVar->KRange;

                    painter->drawLine(QPointF(preXPos,yBasePos-(( dPreNow-dMin)*aveHeight)),
                                     QPointF(curXPos,yBasePos-((dNow-dMin)*aveHeight)));

                }
                dPreNow		=	dNow;
                bHasLast	=	true;
            }
        }
    }

}

void DrawChart::DrawTechBOLL(int nKdCount,int nStart,int nEnd, QPainter *painter,float aveWidth,float &fMin,float &fMax)
{
    CBOLL *pTech = (CBOLL *)m_pGlobalVar->pMainTech;
    if (pTech==nullptr)
        return;
    float	dMin = 0, dMax = 0;
    if( !pTech->min_max_info( nStart, nEnd, &dMin, &dMax ) )
    {
        return;
    }

    if( (fMax - fMin) < 1e-4 ){
        return;
    }
  

        int nIndexParam = (Cross_Cursor_Idx >= 0 and  Cross_Cursor_Idx<nKdCount) ? Cross_Cursor_Idx : nKdCount-1 ;
   
        float	dParam1 = dMin, dParam2 = dMin, dParam3 = dMin;
        QString	 qText;
        if( pTech->calc( &dParam1, &dParam2, &dParam3, nIndexParam, false) )
        {
            QRect rect;
            painter->setPen(Qt::yellow);
            painter->setFont(QFont("微软雅黑",12,60));
            qText=QString("UP %1 MID %2 DOWN %3").arg(dParam1,0,'f',2 ).arg(dParam2 ,0,'f',2).arg(dParam3  ,0,'f',2);
            rect=QRect(840,20,320,30);
            painter->drawText(rect,Qt::AlignLeft, qText);
            
          
        }
   

    float aveHeight;
    aveHeight=m_rectCandleChart.height()/(fMax-fMin);
    
    float curXPos;
    float preXPos;
    float Xoffset=KWIDTHEDGE+aveWidth/2;
    // Draw
    float yPrePos,yPos;
    float	yLast1, yNow1, yLast2, yNow2, yLast3, yNow3;
    bool  bHasLast = false;
    float dNow1 = dMin, dNow2 = dMin, dNow3 = dMin;
    for( int nIndex=0; nIndex<nKdCount; ++nIndex )
    {
        
        // Draw
        if( pTech->calc( &dNow1, &dNow2, &dNow3, nIndex, bHasLast ) )
        {
              
                         
            if(  bHasLast and nIndex >nStart and nIndex<=nEnd)
            {

                preXPos =Xoffset+(nIndex-nStart-1)* m_rectCandleChart.width() / m_pGlobalVar->KRange;
                curXPos =Xoffset+(nIndex-nStart)* m_rectCandleChart.width() / m_pGlobalVar->KRange;

                painter->setPen(Qt::white);


                painter->drawLine(QPointF(preXPos,(fMax-yLast2)*aveHeight+m_rectCandleChart.top()),
                                 QPointF(curXPos,(fMax-dNow2)*aveHeight+m_rectCandleChart.top()));
                
                painter->setPen(Qt::gray);


                painter->drawLine(QPointF(preXPos,(fMax- yLast1	)*aveHeight+m_rectCandleChart.top()),
                                  QPointF(curXPos,(fMax-dNow1)*aveHeight+m_rectCandleChart.top()));
              
                
                painter->setPen(Qt::magenta);

                painter->drawLine(QPointF(preXPos,(fMax- yLast3)*aveHeight+m_rectCandleChart.top()),
                                  QPointF(curXPos,(fMax-dNow3)*aveHeight+m_rectCandleChart.top()));


            }
            yLast1		=	dNow1;
            yLast2		=	dNow2;
            yLast3		=	dNow3;
            bHasLast	=	true;
        }

    }
}

void DrawChart::DrawTechMACD(int nKdCount,int nStart,int nEnd, QPainter *painter,float aveWidth)
{

    CMACD *pTech = (CMACD *)m_pGlobalVar->pSubTech;
   if (pTech==nullptr)
       return;
    float	dMin = 0, dMax = 0;
    if( !pTech->min_max_info( nStart, nEnd, &dMin, &dMax ) )
    {
        qDebug() <<"DrawTechMACD " << dMax <<","<<dMin;
        return;
    }
    float D = (dMax-dMin) / 40;
    dMax= dMax + D * 1.5;
    dMin  = dMin - D * 2;
    
    if(( dMax - dMin) < 1e-4 ){
         qDebug() <<"DrawTechMACD error" << dMax <<","<<dMin;
        return;
    }
   
    float yPos = subRect.top();
    QRect rect;
    rect=QRect(KWIDTHEDGE,yPos,60,30);
    painter->setPen(Qt::yellow);
    painter->setFont(QFont("微软雅黑",12,60));
    painter->drawText(rect,Qt::AlignLeft, "KDJ");
    
    
      int	nIndexParam	=(Cross_Cursor_Idx >= 0 and  Cross_Cursor_Idx<nKdCount) ? Cross_Cursor_Idx : nKdCount-1 ;
  
    float aveHeight=subRect.height()/(dMax-dMin);
    
     float	dEMA1 = 0, dEMA2 = 0, dDIF = 0, dDEA = 0;

    bool bHasLast	=	false;
    float curXPos;
    float preXPos;

    float Xoffset=KWIDTHEDGE+aveWidth/2;
    float  dPreDIF , dPreDEA ;
    for( int nIndex=0; nIndex<nKdCount; nIndex++ )
    {        

        if(  pTech->calc( &dEMA1, &dEMA2, &dDIF, &dDEA, nIndex, bHasLast ) )
        {
            

            if( bHasLast )
            {

                if( nIndex > nStart and nIndex <= nEnd)
                {
                    
                    preXPos =Xoffset+(nIndex-nStart-1)* m_rectCandleChart.width() / m_pGlobalVar->KRange;
                    curXPos =Xoffset+(nIndex-nStart)* m_rectCandleChart.width() / m_pGlobalVar->KRange;


                    painter->setPen(Qt::white);
                    painter->drawLine(QPointF(preXPos,subRect.bottom()-(dPreDEA-dMin)*aveHeight),
                                      QPointF(curXPos,subRect.bottom()-(dDEA-dMin)*aveHeight));
                     painter->setPen(Qt::yellow);
                    painter->drawLine(QPointF(preXPos,subRect.bottom()-(dPreDIF-dMin)*aveHeight),
                                      QPointF(curXPos,subRect.bottom()-(dDIF-dMin)*aveHeight));
                                     
                }
            }
            bHasLast	=	true;
   
            dPreDIF =dDIF;
            dPreDEA =dDEA;
            
            if(  nIndex == nIndexParam )
            {
                
                QString qText;
                qText=QString("EMA(%1) EMA(%2) DIF %3 DEA(%4) %5 ").arg(pTech->m_nEMA1Days).arg(pTech->m_nEMA2Days).arg(dDIF ,0,'f',3).arg(pTech->m_nDIFDays).arg(dDEA ,0,'f',3);
                rect=QRect(KWIDTHEDGE+60,yPos,320,30);
                painter->drawText(rect,Qt::AlignLeft, qText);

            }
        }

    }
    
}

void DrawChart::DrawTechKDJ(int nKdCount,int nStart,int nEnd, QPainter *painter,float aveWidth)
{
    CKDJ *pTech = (CKDJ *)m_pGlobalVar->pSubTech;
    if (pTech==nullptr)
        return;
    float	dMin = 0, dMax = 0;
    if( !pTech->min_max_info( nStart, nEnd, &dMin, &dMax ) )
    {
        qDebug()<<"CKDJ error";
        return;
    }
    float D = (dMax-dMin) / 40;
    dMax= dMax + D * 1.5;
    dMin  = dMin - D * 2;
    
    if( (dMax - dMin) < 1e-4 ){
         qDebug()<<"CKDJ error-1";
        return;
    }
    

    float yPos = subRect.top();
    QRect rect;
    rect=QRect(KWIDTHEDGE,yPos,60,30);
    painter->setPen(Qt::yellow);
    painter->setFont(QFont("微软雅黑",12,60));
    painter->drawText(rect,Qt::AlignLeft, "KDJ");
    

        int	nIndexParam	=(Cross_Cursor_Idx >= 0 and  Cross_Cursor_Idx<nKdCount) ? Cross_Cursor_Idx : nKdCount-1 ;
      
        float	dParam1 = dMin, dParam2 = dMin, dParam3 = dMin;
        QString	qText;
        if( pTech->calc( &dParam1, &dParam2, &dParam3, nIndexParam, false ) )
        {
            
            qText=QString("K %1 D %2 J %3 ").arg(dParam1,0,'f',2).arg(dParam2 ,0,'f',2).arg(dParam3 ,0,'f',2);
            rect=QRect(KWIDTHEDGE+60,yPos,320,30);
            painter->drawText(rect,Qt::AlignLeft, qText);

        }

    float aveHeight=subRect.height()/(dMax-dMin);
    float curXPos;
    float preXPos;
    // Draw
    float	yLast1, yNow1, yLast2, yNow2, yLast3, yNow3;
    bool	bHasLast = false;
    float	dNow1 = dMin, dNow2 = dMin, dNow3 = dMin;
    float Xoffset=KWIDTHEDGE+aveWidth/2;
    for( int nIndex=0; nIndex<nKdCount; nIndex++ )
    {

        if( pTech->calc( &dNow1, &dNow2, &dNow3, nIndex, bHasLast ) )
        {
            if( bHasLast and nIndex>nStart and nIndex<=nEnd)
            {
                preXPos =Xoffset+(nIndex-nStart-1)* m_rectCandleChart.width() / m_pGlobalVar->KRange;
                curXPos =Xoffset+(nIndex-nStart)* m_rectCandleChart.width() / m_pGlobalVar->KRange;

                painter->setPen(Qt::white);
                painter->drawLine(QPointF(preXPos,subRect.bottom()-(yLast1	-dMin)*aveHeight),
                                  QPointF(curXPos,subRect.bottom()-(dNow1-dMin)*aveHeight));
                painter->setPen(Qt::yellow);
                painter->drawLine(QPointF(preXPos,subRect.bottom()-(yLast2	-dMin)*aveHeight),
                                  QPointF(curXPos,subRect.bottom()-(dNow2-dMin)*aveHeight));
                painter->setPen(Qt::gray);
                painter->drawLine(QPointF(preXPos,subRect.bottom()-(yLast3	-dMin)*aveHeight),
                                  QPointF(curXPos,subRect.bottom()-(dNow3-dMin)*aveHeight));
               
            }
            yLast1		=	dNow1;
            yLast2		=	dNow2;
            yLast3		=	dNow3;
            bHasLast	=	true;
        }

    }
    
}

 //显示成交量指标线
void DrawChart::DrawTechVol(int nKdCount,int nStart,int nEnd, QPainter *painter,float aveWidth)
{
    CVOLUME *pTech = (CVOLUME *)m_pGlobalVar->pSubTech;
    if (pTech==nullptr)
        return;
    
    float	dMin = 0, dMax = 0;
    if( !pTech->min_max_info( nStart, nEnd, &dMin, &dMax ) )
    {
        return;
    }

    if( (dMax - dMin)< 1e-4 )
        return;

    float aveHeight=subRect.height()/(dMax-dMin);
    float curXPos;//=KWIDTHEDGE+aveWidth/2;
    float preXPos;//=KWIDTHEDGE-aveWidth/2;
    float yPos = subRect.top();
    QRect rect;
    rect=QRect(KWIDTHEDGE,yPos,60,30);
    painter->setPen(Qt::yellow);
    painter->setFont(QFont("微软雅黑",12,60));
    painter->drawText(rect,Qt::AlignLeft, "Vol");
    int w = aveWidth / 4;
    if ((w % 2) == 0)
        w++;
    int nParamNum =(int)pTech->m_adwMADays.size();



        QFontMetrics metr(painter->font());

        int	nIndexParam	= (Cross_Cursor_Idx >= 0 and  Cross_Cursor_Idx<nKdCount) ? Cross_Cursor_Idx : nKdCount-1 ;
        float	dParam;
        QString	strText;
        curXPos=KWIDTHEDGE+60;
        QRect rectfont;
        for( int k=0; k<nParamNum ; k++ )
        {
            if( pTech->calc( &dParam, nIndexParam,pTech->m_adwMADays[k],false ) )
            {

                if ((k%nParamNum)==0)
                    painter->setPen(QColor(255, 255, 0));
                else
                    painter->setPen(QColor(0, 255, 0));


                strText=QString("MA%1 %2").arg(pTech->m_adwMADays[k]).arg(dParam ,0,'f',0);
                rectfont= metr.boundingRect(strText);
                rect=QRect(curXPos,yPos, rectfont.width()+5,30);
                painter->drawText(rect,Qt::AlignLeft, strText);
                curXPos+= rectfont.width() + (k+1)*20;
            }
        }


    // Draw

    int	yLast, yNow;
    bool	bHasLast ;
    float	dNow;
    float	dPreNow1=0, dPreNow2=0;
    int yBasePos =subRect.bottom();
    float Xoffset=KWIDTHEDGE+aveWidth/2;
    for( int k=0; k<nParamNum; k++ )
    {
        bHasLast = false;

        for( int nIndex=nStart; nIndex<=nEnd; nIndex++ )
        {

            if( 0 == k )
            {
                CL::Kline	kd	=	m_pKline->at(nIndex);

               if( kd.close < kd.open )
               {
                 painter->setPen(QColor(2, 148, 255));
                 painter->setBrush(QColor(2, 148, 255));
               }
               else
               {
                 painter->setPen(Qt::white);
                 painter->setBrush(Qt::white);
               }

                curXPos =Xoffset+(nIndex-nStart)* m_rectCandleChart.width() / m_pGlobalVar->KRange;
                painter->drawRect(curXPos - w, subRect.bottom() -  kd.vol* aveHeight, 2 * w, kd.vol * aveHeight);
            }


            if( pTech->calc(&dNow, nIndex,pTech->m_adwMADays[k], bHasLast ) )
            {
                if( bHasLast )
                {
                    if ((k%nParamNum)==0){
                      painter->setPen(QColor(255, 255, 0));
                    }
                    else{
                     painter->setPen(QColor(0, 255, 0));
                    }
                    preXPos =Xoffset+(nIndex-nStart-1)* m_rectCandleChart.width() / m_pGlobalVar->KRange;
                    curXPos =Xoffset+(nIndex-nStart)* m_rectCandleChart.width() / m_pGlobalVar->KRange;

                    painter->drawLine(QPointF(preXPos,yBasePos-(( yLast-dMin)*aveHeight)),
                                      QPointF(curXPos,yBasePos-((dNow-dMin)*aveHeight)));
                }
                bHasLast	=	true;
                yLast		=	dNow;
            }
        }
    }
}
