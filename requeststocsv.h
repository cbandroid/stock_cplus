
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

class RequestsToCsv : public QDialog
{
    Q_OBJECT
public:
    explicit RequestsToCsv(QDialog *parent = nullptr);

    bool getIndexList();
    bool getPlateList();
    void dealWithPlateList(QList<QStringList> &list,const QByteArray &allData);
    QString getStockList();
    void dealWithAllList();
    void downStockIndexPlateInfo();
    void downloadAllStockK();
    Q_INVOKABLE void setText();
   
//    void baoShareStockK();
    QThreadPool threadPool;
    QDialog *progressBarWindow = new QDialog();
    QPushButton *stopBtn = new QPushButton("终止下载",progressBarWindow);
    QLabel *numLine = new QLabel(progressBarWindow);
    QProgressBar *progressBar = new QProgressBar(progressBarWindow);
    QString CNToEL(const QString &cnstr);
    int downloadedNums=0;
    QString curDate;
    bool isDownload=false;

signals:

};

#endif // REQUESTSTOCSV_H
