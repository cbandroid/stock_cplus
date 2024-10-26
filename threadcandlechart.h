
#ifndef THREADCANDLECHART_H
#define THREADCANDLECHART_H

#include <QString>
#include <qobject.h>

class ThreadCandleChart : public QObject
{
    Q_OBJECT
public:
    explicit ThreadCandleChart(QObject *parent = nullptr);
    void getAllCandleChart(QString freq,QString adjustFlag,bool isFirst);
private:
    QByteArray allData;
//    QNetworkAccessManager *naManager =new QNetworkAccessManager(this);
    void initCandleChartList();

signals:
    void getCandleChartFinished();
};

#endif // THREADCANDLECHART_H
