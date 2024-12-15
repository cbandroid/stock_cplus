
#ifndef THREADCANDLECHART_H
#define THREADCANDLECHART_H

#include <QString>
#include <qobject.h>
#include <QNetworkReply>

class CFormularComputeParent;
class CLCore;
class CKData;
class GlobalVar;

class ThreadCandleChart : public QObject
{
    Q_OBJECT
public:
    explicit ThreadCandleChart(GlobalVar *pGlobalVar,CKData *&pKline,CLCore *&pCLCore,QObject *parent = nullptr);
     ~ThreadCandleChart();
    void getAllCandleChart(QString freq,QString adjustFlag,bool isFirst);

private:
    QNetworkAccessManager *pNetManager;
    QByteArray responseData;
    bool bReq;
    QString m_strPreHead;
    QString m_strPreSymbol;
    QString Oldfreq;
    QString OldAdjustFlag;

    void initTrendCandleChartList(QString freq);
    void initCandleChartList(QString freq,bool isFirst);
    void getSSEData(QString freq,QString url);
    CFormularComputeParent *pCLFormular;
    CLCore *m_pCLCore;
    CKData *m_pKline;
    GlobalVar *m_pGlobalVar;

signals:
    void getCandleChartFinished();


};

#endif // THREADCANDLECHART_H
