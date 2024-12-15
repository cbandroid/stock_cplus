
#ifndef GLOBALVAR_H
#define GLOBALVAR_H

#include <QDateTime>
#include <QTimeZone>
#include <QSettings>
#include "stockinfo.h"
#include <QPalette>
#include <QSharedPointer>
#include <QSharedDataPointer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QTimer>
#include <QCoreApplication>
#include "include\enums.h"

#define  CNMARKET  1
#define  HKMARKET  2
#define  USMARKET  5
#define  UKMARKET  6

#define  FUNDFLOW  4

enum CANCLICK
{
    NORMALCLICK=0,
    CNCLICK=1,
    OTHERCLICK=-1,
    FUNDFLOWCLICK=2
};

typedef void (*Callback)(QNetworkReply *);
typedef void (*Pfn)();

class  CTechnique;

class GlobalVar
{
public:
    GlobalVar();
    QDateTime  GetLatestTradeTime(QDateTime tmNow ) const;
    bool isZhWorkDay(QDateTime curTime,int select) const;
    QDateTime curRecentWorkDay(int select) const;
    QDateTime curWorkDay() const;
    bool isZhMarketDay(QDateTime curTime) const;
    bool isUSMarketDay(QDateTime curTime) const;
    bool isUKMarketDay(QDateTime curTime) const;
    bool isHKMarketDay(QDateTime curTime) const;

    bool isTradeTime(QDateTime curTime,int  nDelta=120) const;

    bool getVacation() const;
    QString getComCode() const;
    QString getStockSymbol() const;

    QString getCookies(QString url);


    QString peelStr(const QString &s,QString begin,QString end) const;
    QPair<QString, QString> cutStr(const QString &s,QString begin,QString end) const;
    QString getContent(const QString &s);
    QString getAttributeContent(const QString &s,QString attribute,QString symbol);
    void getAllContent(QString &content,QStringList &strList,QString begin) const;
    bool isInt(QString s) const;

    int setRound();

    QString curCode;
    volatile bool isBoard;

    QString curBoard;
    QString curName;
    QString EPSReportDate;
    QString PEName;

    float preClose;
    float hisPreClose;
    volatile int WhichInterface;
    volatile bool isKState;
    volatile bool isUsZhStock;

    QSettings *settings;

    int KRange;
    float timeShareHighLowPoint[5];

    float buySellPrice[10];
    float buySellNum[10];
    float baseInfoData[14];
    int curSortNum[4];
    QString columns[17];
    bool is_asc[4];
    QPalette pRed;
    QPalette pGreen;
    QPalette pBlack;
    QPalette pBlue;
    QPalette pWhite;
    int upNums[4];
    int downNums[4];
    int trendsTotal;
    volatile bool isSayNews;
    float speechrate;
    int curBlock;
    QString currentPath;
    int offsetEnd;
    int offsetLocal;
    bool areaFlag[5];
    QString formulaContent;
    int mTableListNum;
    QString mCandleListCode;
    QList<QStringList> formula;
    QList<QStringList> annoucementList;
    
    CTechnique *pMainTech;
    CTechnique *pSubTech;
    
    QStringList CodeNameData;
    QStringList PinYinData;


protected:
    QByteArray replyResult;
    QByteArray replyData;

};

inline int GlobalVar::setRound()
{
    int p=2;
    QString s=GlobalVar::curCode.left(3);
    if (s=="133")
        p=4;
    else if (GlobalVar::curCode.size()==5 or s=="155")
        p=3;
    return p;
}

#endif // GLOBALVAR_H
