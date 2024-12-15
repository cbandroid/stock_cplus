
#ifndef THREADTABLE_H
#define THREADTABLE_H
#include <QNetworkReply>
#include <QFile>
#include <QTextCodec>
#include "stockinfo.h"

class GlobalVar;

class ThreadTable : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTable(GlobalVar *pGlobalVar,QList<StockInfo> *&pRisingSpeedList,QList<StockInfo> *&pTableList,QList<StockInfo> *&pTableListCopy,QList<StockInfo> *&pMyStockList,QList<QStringList> *&pFundFlowList,QStringList *&pMyStockCode
                         ,QObject *parent = nullptr);
    void getTableData(bool bFirst);

private:
    void initTableList(bool bFirst);
    void readMyStock();
    void reFlaseMyStock();
    QByteArray allData;
    int risingSpeedSize=29;
    static bool compareStockInfoData(const  StockInfo &s1, const  StockInfo &s2);

    QList<StockInfo> *m_pTableList;
    QList<StockInfo> *m_pTableListCopy;
    QList<StockInfo> *m_pMyStockList;
    QList<QStringList> *m_pFundFlowList;
    QStringList *m_pMyStockCode;
    QList<StockInfo> *m_pRisingSpeedList;
    GlobalVar *m_pGlobalVar;
signals:
    void getTableDataFinished();

};

#endif // THREADTABLE_H
