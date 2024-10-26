
#ifndef THREADTIMESHARECHART_H
#define THREADTIMESHARECHART_H

#include <QObject>
#include <QNetworkReply>

class ThreadTimeShareChart : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTimeShareChart(QObject *parent = nullptr);
    ~ThreadTimeShareChart();
    void getSSEData();
    void getAllTimeShareChart(bool r);

private:
    void initTimeShareChartList();
    void initSSETimeShareChartList();
    QNetworkReply *reply=nullptr;
    // QByteArray* qByteArray=new QByteArray();
   // QNetworkAccessManager *naManager =new QNetworkAccessManager(this);
    // QNetworkRequest request;
    QByteArray allData;
    // bool isFirst=true;
    QString preGCode="";
    // int mRetries=0;
    float pp;

signals:
    void getTimeShareChartFinished();
};

#endif // THREADTIMESHARECHART_H
