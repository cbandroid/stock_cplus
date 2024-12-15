
#ifndef THREADINDEX_H
#define THREADINDEX_H
#include <QNetworkReply>
#include <QObject>
#include "stockinfo.h"
class GlobalVar;
class ThreadIndex : public QObject
{
    Q_OBJECT
public:
    explicit ThreadIndex(GlobalVar *pGlobalVar,QList<IndexInfo> *&pIndexList, QList<QStringList> *&pFundFlowList,QObject *parent = nullptr);
    void getAllIndex(bool reset);
    double totalAmount;
private:
    void initIndexList();
    void getEastFundFlow();
    QByteArray indexData;
    QList<QStringList> *m_pFundFlowList;
    QList<IndexInfo> *m_pIndexList;
    GlobalVar *m_pGlobalVar;
signals:
    void getIndexFinished();
    void getBlockFinished();

};

#endif // THREADINDEX_H
