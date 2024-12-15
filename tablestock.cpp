#include "commondelegate.h"
#include "qheaderview.h"
#include "modeltablestock.h"
#include "modelfundflow.h"
#include "modeltimeshare.h"
#include "globalvar.h"
#include "tablestock.h"


TableStock::TableStock( GlobalVar *pGlobalVar, QStringList *&pMyStockCode,QList<StockInfo> *&pMyStockList,
                       QList<StockInfo> *&pTableList,
                       QList<StockInfo> *&pRisingSpeedList,
                       QList<timeShareTickInfo> *&pTimeShareTickList)
{
    m_pGlobalVar =pGlobalVar;
    m_pMyStockCode=pMyStockCode;
    m_pMyStockList=pMyStockList;
    m_pTableList=pTableList;
    m_pRisingSpeedList=pRisingSpeedList;
    m_pTimeShareTickList=pTimeShareTickList;
    isFlashTable=true;
    preCode="";
    preSize=0;
    stockTableView=new QTableView(this);
    blockView=new QTableView(this);
    risingSpeedView=new QTableView(this);
    myStockView=new QTableView(this);

    timeShareTickView=new QTableView(this);

    /*timeShareTickView->setMinimumWidth(300);
    risingSpeedView->setMaximumWidth(285);
    risingSpeedView->setMinimumHeight(496);
    myStockView->setMinimumHeight(496);
    blockView->setMinimumWidth(785);*/

    m_tableModel= new ModelTableStock(pGlobalVar,pMyStockCode,stockTableView);
    m_fundFlowModel=new ModelFundFlow(pGlobalVar,blockView);
    m_risingSpeedModel= new ModelTableStock(pGlobalVar,pMyStockCode,risingSpeedView);
    m_myStockModel= new ModelTableStock(pGlobalVar,pMyStockCode,myStockView);

    stockTableView->setFocusPolicy(Qt::StrongFocus);
    blockView->setFocusPolicy(Qt::StrongFocus);
    risingSpeedView->setFocusPolicy(Qt::StrongFocus);
    myStockView->setFocusPolicy(Qt::StrongFocus);

    stockTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    blockView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    risingSpeedView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    myStockView->setEditTriggers(QAbstractItemView::NoEditTriggers);

  //  stockTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    stockTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    blockView->setSelectionMode(QAbstractItemView::SingleSelection);
    risingSpeedView->setSelectionMode(QAbstractItemView::SingleSelection);
    myStockView->setSelectionMode(QAbstractItemView::SingleSelection);

    stockTableView->setTabKeyNavigation(false);
    blockView->setTabKeyNavigation(false);
    risingSpeedView->setTabKeyNavigation(false);
    myStockView->setTabKeyNavigation(false);

    timeShareTickView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    m_timeShareTickModel= new ModelTimeShare(pGlobalVar,timeShareTickView);
    timeShareTickView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    initTableView();
}

void TableStock::setTableView()
{
    int row=stockTableView->currentIndex().row();
    if (row==-1)
        row=0;
    if (isFlashTable)
    {
        m_tableModel->setModelData(*m_pTableList,false,true,false);
        stockTableView->setCurrentIndex(m_tableModel->index(row,0));
    }
    if (m_pGlobalVar->WhichInterface==CNMARKET)
    {
        m_risingSpeedModel->setModelData(*m_pRisingSpeedList,false,true,false);
        m_myStockModel->setModelData(*m_pMyStockList,false,false,true);
    }
}

void TableStock::setColumnWidth(QTableView *TV)
{

    if (m_pGlobalVar->WhichInterface==HKMARKET || m_pGlobalVar->WhichInterface==USMARKET || m_pGlobalVar->WhichInterface==UKMARKET)
    {
        TV->setColumnWidth(0, 75);
        TV->setColumnWidth(1, 280);
    }
    else
    {
        TV->setColumnWidth(0, 60);
        TV->setColumnWidth(1, 75);
    }
    TV->setColumnWidth(2, 50);
    TV->setColumnWidth(3, 66);
    TV->setColumnWidth(4, 75);
    TV->setColumnWidth(5, 60);
    TV->setColumnWidth(6, 85);
    TV->setColumnWidth(7, 80);
    TV->setColumnWidth(8, 70);
    TV->setColumnWidth(9, 90);
    TV->setColumnWidth(10, 65);
    TV->setColumnWidth(11, 65);
    TV->setColumnWidth(12, 75);
    TV->setColumnWidth(13, 70);
    TV->setColumnWidth(14, 70);
    TV->setColumnWidth(15, 70);
    TV->setColumnWidth(16, 70);
}

void TableStock::setBlockView()
{
    blockView->setColumnWidth(0,135);
    blockView->setColumnWidth(1,78);
    blockView->setColumnWidth(3,85);
    blockView->setColumnWidth(4,50);
    blockView->setColumnWidth(6,50);
    blockView->setColumnWidth(7,85);
    blockView->setColumnWidth(8,50);
    blockView->setColumnWidth(9,85);
    blockView->setColumnWidth(10,50);
    blockView->setColumnWidth(11,85);
    blockView->setColumnWidth(12,50);
}

void TableStock::setTimeShareTickView()
{
    if (m_pTimeShareTickList->isEmpty())
    {
        timeShareTickView->clearSpans();
    }

    m_timeShareTickModel->setModelData(*m_pTimeShareTickList);
    if ((preSize-timeShareTickView->verticalScrollBar()->value())==13 or
        preCode!=m_pGlobalVar->curCode)
        timeShareTickView->scrollToBottom();
    preSize=m_pTimeShareTickList->size();
    preCode=m_pGlobalVar->curCode;
    timeShareTickView->setColumnWidth(0,60);
    timeShareTickView->setColumnWidth(1,85);
    timeShareTickView->setColumnWidth(2,75);
    timeShareTickView->setColumnWidth(3,12);
    timeShareTickView->setColumnWidth(4,48);
//    timeShareTickView->resizeColumnsToContents();

}

void TableStock::initTableView()
{
    stockTableView->setSortingEnabled(false);
    // 隐藏排序图标
    stockTableView->horizontalHeader()->setSortIndicatorShown(false);
    QFont boldFont;
    boldFont.setBold(true);
    boldFont.setPixelSize(18);
    QHeaderView *header=myStockView->verticalHeader();
    //拖拽交换行
    header->setSectionsMovable(true);

    connect(header,&QHeaderView::sectionMoved,this,[=](int /*logicIndex*/,int oldIndex,int newIndex){
        StockInfo info=m_pMyStockList->at(oldIndex);
        m_pMyStockList->remove(oldIndex);
        m_pMyStockList->insert(newIndex,info);
        QStringList s;
        int nCount=m_pMyStockList->count();
        for (int i=0;i<nCount;++i)
            s<<m_pMyStockList->at(i).code;
        m_pGlobalVar->settings->setValue("myStock",s);

        *m_pMyStockCode=s;

        QList<StockInfo> t;
        m_myStockModel->setModelData(t,false,false,true);
        m_myStockModel->setModelData(*m_pMyStockList,false,false,true);
        myStockView->setCurrentIndex(m_myStockModel->index(newIndex,0));
    });

    QTableView *tl[4]={stockTableView,risingSpeedView,myStockView,blockView};
    for (int i=0;i<4;++i)
    {
        tl[i]->horizontalHeader()->setSortIndicatorShown(false);
        tl[i]->horizontalHeader()->setSortIndicatorShown(false);
        tl[i]->horizontalHeader()->setFont(boldFont);
        tl[i]->verticalHeader()->setMinimumSectionSize(21);
        tl[i]->verticalHeader()->setDefaultSectionSize(21);
        tl[i]->setAlternatingRowColors(true);
        tl[i]->setShowGrid(false);
        tl[i]->setSelectionBehavior(QAbstractItemView::SelectRows);
        tl[i]->setSelectionMode(QAbstractItemView::SingleSelection);
        tl[i]->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tl[i]->setStyleSheet("QTableView{selection-background-color:lightgray}");
        if (i==0)
            tl[0]->verticalScrollBar()->setStyleSheet("QScrollBar{width:10px;}");
        else
            tl[i]->verticalScrollBar()->setStyleSheet("QScrollBar{width:0px;}");
        tl[i]->horizontalScrollBar()->setStyleSheet("QScrollBar{width:0px;}");
        tl[i]->setItemDelegate(new CommonDelegate());
    }
    timeShareTickView->verticalHeader()->setVisible(false);
    timeShareTickView->horizontalHeader()->setVisible(false);
    timeShareTickView->setShowGrid(false);
    timeShareTickView->verticalHeader()->setMinimumSectionSize(18);
    timeShareTickView->verticalHeader()->setDefaultSectionSize(18);
    timeShareTickView->setSelectionMode(QAbstractItemView::NoSelection);
    timeShareTickView->horizontalHeader()->setMinimumSectionSize(1);
    timeShareTickView->setFrameStyle(0);
//    timeShareTickView->verticalScrollBar()->setStyleSheet("QScrollBar{width:15px;}");

    stockTableView->setModel(m_tableModel);
    risingSpeedView->setModel(m_risingSpeedModel);
    myStockView->setModel(m_myStockModel);
    blockView->setModel(m_fundFlowModel);
    timeShareTickView->setModel(m_timeShareTickModel);
    connect(m_tableModel, &ModelTableStock::dataChanged, stockTableView, [=](){update();});
    connect(m_risingSpeedModel, &ModelTableStock::dataChanged, risingSpeedView, [=](){update();});
    connect(m_myStockModel, &ModelTableStock::dataChanged, myStockView, [=](){update();});
    connect(m_fundFlowModel, &ModelTableStock::dataChanged, blockView, [=](){update();});
    connect(m_timeShareTickModel, &ModelTableStock::dataChanged, timeShareTickView, [=](){update();});
    connect(m_tableModel,&ModelTableStock::modelReset,stockTableView,[=](){
        setColumnWidth(stockTableView);
    });
    connect(m_risingSpeedModel,&ModelTableStock::modelReset,risingSpeedView,[=](){
        risingSpeedView->setCurrentIndex(m_risingSpeedModel->index(0,0));
        setColumnWidth(risingSpeedView);
    });
    connect(m_myStockModel,&ModelTableStock::modelReset,myStockView,[=](){
        myStockView->setCurrentIndex(m_myStockModel->index(0,0));
        setColumnWidth(myStockView);
    });
    connect(m_fundFlowModel,&ModelTableStock::modelReset,blockView,[=](){
        setBlockView();
    });
}
