
#ifndef THREADTIMESHARECHART_H
#define THREADTIMESHARECHART_H

#include <QObject>
#include <QNetworkReply>
#include "stockinfo.h"

class GlobalVar;

class ThreadTimeShareChart : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTimeShareChart(GlobalVar *pGlobalVar,QList<timeShareChartInfo> *&pTimeShareChartList,QObject *parent = nullptr);
    ~ThreadTimeShareChart();
    void getSSEData();
    void getAllTimeShareChart(bool r);

private:
    void initTimeShareChartList();
    void initSSETimeShareChartList();
    QNetworkReply *reply=nullptr;

    QByteArray allData;
    // bool isFirst=true;
    QString preGCode="";
    // int mRetries=0;
    float pp;
    QList<timeShareChartInfo> *m_pTimeShareChartList;
    GlobalVar *m_pGlobalVar;
signals:
    void getTimeShareChartFinished();

};

#endif // THREADTIMESHARECHART_H
