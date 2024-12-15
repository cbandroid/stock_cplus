#ifndef UTILITYEX_H
#define UTILITYEX_H

#include "stockinfo.h"
#include <QDateTime>


extern bool isWorkDay(QDateTime curTime) ;
extern QString format_conversion(float data);
extern QString FormatNumber(float data,int digitdot,QString strDef );
extern StockInfo findStock(QList<StockInfo> *pTableList,QString code);
extern void sortByColumn(QList<StockInfo> *mList, const int column, const bool is_asc);
extern void sortByStrColumn(QList<QStringList> *mList,int column,bool is_asc);
extern  QString CNToEL(const QString &cnstr);

#endif // UTILITYEX_H
