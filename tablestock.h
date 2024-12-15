#ifndef TABLESTOCK_H
#define TABLESTOCK_H

#include <QTableView>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QStandardItemModel>
#include "stockinfo.h"

class ModelTableStock;
class ModelFundFlow;
class ModelTimeShare;
class GlobalVar;

class TableStock : public QTableView
{
public:
    TableStock(class GlobalVar *pGlobalVar,  QStringList *&pMyStockCode,  QList<StockInfo> *&pMyStockList,
               QList<StockInfo> *&pTableList,
               QList<StockInfo> *&pRisingSpeedList,
               QList<timeShareTickInfo> *&pTimeShareTickList);
    QTableView *stockTableView;
    QTableView *blockView;
    QTableView *risingSpeedView;
    QTableView *myStockView;
    QTableView *timeShareTickView;
    ModelTableStock *m_tableModel;
    ModelTableStock *m_risingSpeedModel;
    ModelTableStock *m_myStockModel;
    ModelTimeShare *m_timeShareTickModel;
    ModelFundFlow *m_fundFlowModel;
    bool isFlashTable;
    QString preCode;
    int preSize;
    void setTableView();
    void setTimeShareTickView();
    void setColumnWidth(QTableView *TV);
    void setBlockView();

private:
    GlobalVar *m_pGlobalVar;
    QStringList *m_pMyStockCode;
    QList<StockInfo> *m_pMyStockList;
    QList<StockInfo> *m_pTableList;
    QList<StockInfo> *m_pRisingSpeedList;
    QList<timeShareTickInfo> *m_pTimeShareTickList;
    void initTableView();

};

#endif // TABLESTOCK_H
