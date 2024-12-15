
#ifndef REQUESTSTOCSV_H
#define REQUESTSTOCSV_H

#include <qdialog.h>
#include <qpushbutton.h>
//#include "qwidget.h"
#include <QObject>
//#include <QNetworkAccessManager>
#include <QFile>
#include <QJsonArray>
#include <QTextCodec>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>

#include <qmessagebox.h>
#include <qpushbutton.h>
#include <QDir>

#include <QThreadPool>
#include "stockinfo.h"

class GlobalVar;

class RequestsToCsv : public QDialog
{
    Q_OBJECT
public:
    explicit RequestsToCsv(GlobalVar *pGlobalVar,QList<StockInfo> *&pTableList,QDialog *parent = nullptr);
    ~RequestsToCsv();
    bool getIndexList();
    bool getPlateList();
    void dealWithPlateList(QList<QStringList> &list,const QByteArray &allData);
    QString getStockList();
     bool dealWithAllList();
    bool downStockIndexPlateInfo();
    void downloadAllStockK(QList<StockInfo> *&pTableListCopy);
    Q_INVOKABLE void setText();
   
//    void baoShareStockK();
    QThreadPool threadPool;
    QDialog *progressBarWindowDlg;
    QPushButton *stopBtn ;
    QLabel *lblnumLine ;
    QProgressBar *progressBar ;

    int downloadedNums;
    QString curDate;
    bool isDownload;
private:
    static QDialog *promptDlg;
    static QTimer *qTimer;
    QList<StockInfo> *m_pTableListCopy;
    GlobalVar *m_pGlobalVar;
    void isDirExist(QString fullPath);
    void getEastStockList();
    static void OnAccept(); // override;
signals:

};

#endif // REQUESTSTOCSV_H
