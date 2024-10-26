
#ifndef THREADTABLE_H
#define THREADTABLE_H

#include <QFile>
#include <QTextCodec>
#include "stockinfo.h"

class ThreadTable : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTable(QObject *parent = nullptr);
    void getTableData();

private:
    void initTableList();
    void readMyStock();
    void reFlaseMyStock();
    QByteArray allData;
    int risingSpeedSize=29;
    static bool compareStockInfoData(const  StockInfo &s1, const  StockInfo &s2);
//    QNetworkAccessManager *naManager =new QNetworkAccessManager(this);
     QString CNToEL(const QString &cnstr);
signals:
    void getTableDataFinished();
};

#endif // THREADTABLE_H
