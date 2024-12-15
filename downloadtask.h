#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

#include <qobjectdefs.h>
#include <QRunnable>
#include <QString>
#include <QFile>
#include "stockinfo.h"
class GlobalVar;

class DownloadTask : public QRunnable
{
public:
    explicit DownloadTask(GlobalVar *pGlobalVar,QList<StockInfo> *pTableList,QObject *obj);
    int nums;
    QString curDate;
    QString endDate;
    void run() override
    {
        downloadK();
        QMetaObject::invokeMethod(m_pObj,"setText");
    }
    void downloadK();
private:
    QObject * m_pObj = nullptr;
    QList<StockInfo> *m_pTableList;
    GlobalVar *m_pGlobalVar;
};



#endif // DOWNLOADTASK_H
