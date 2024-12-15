#ifndef STOCKINFO_H
#define STOCKINFO_H
#include <QString>

#define COLNUMS           17
//#define OFFSETEND 0
#define KRANGE            360
#define WIDTHEDGE         2.0f
#define BOTTOMHEIGHTEDGE  20
#define TOPHEIGHTEDGE     4.0f
#define KWIDTHEDGE        4.0f
#define KRIGHTWIDTHEDGE   60.0f
#define KTOPHEIGHTEDGE    60.0f
#define KBOTTOMHEIGHTEDGE 5
#define myper(c,precls) ((c>=precls)?(int(((c-precls)/precls)*10000+0.5f)/100.0f):(int(((c-precls)/precls)*10000-0.5f)/100.0f))
//#define per3(c) ((c)>=(m_pGlobalVar->preClose))?(int((((c)-(m_pGlobalVar->preClose))/(m_pGlobalVar->preClose))*100000+0.5)/1000.0):(int((((c)-(m_pGlobalVar->preClose))/(m_pGlobalVar->preClose))*100000-0.5)/1000.0)
#define SPLITMID "/R"
#define SPLITBACK "////"
#define TITLEHEIGHT   40
#define TIPWIDTH      15
#define PI            3.1415926f
#define MAX_RETRIES   3
#define POSXCHART     640
#define POSYCHART     120

struct StockInfo
{
    QString code;
    QString name;
    float close;
    float pctChg;
    float turn;
    float amount;
    float velocity;
    float pe;
    float totalValue;
    float circulatedValue;
    float pctYear;
    float pctSixty;
    float volume;
    float high;
    float low;
    float open;
    float preClose;
};

struct IndexInfo
{
    QString strSymbol;
    QString name="name";
    QString close="1";
    QString pctChg="0.01";
};

struct timeShareTickInfo
{
    QString time;
    float price;
    int nums;
    int direct;
    int tick;
};

struct timeShareChartInfo
{
    QString time;
    float price;
    float vol;
    int direct;
    float avePrice;
};

struct candleChartInfo
{
    QString time;
    float open;
    float close;
    float pctChg;
    float high;
    float low;
    float vol;
    float turn;
    float amount;
    float MA5;
    float MA10;
    float MA20;
    float MA60;
    float VMA5;
    float VMA10;
};

#endif // STOCKINFO_H
