
#ifndef SEARCHSTOCK_H
#define SEARCHSTOCK_H

#include <QLineEdit>
#include <QTextEdit>
#include <QFile>
#include <QTextCodec>
#include <QKeyEvent>
#include <QTextBlock>
#include "stockinfo.h"
class GlobalVar;

class SearchStock : public QLineEdit
{
    Q_OBJECT
public:
    SearchStock(GlobalVar *pGlobalVar);
    QLineEdit *searchCodeLine;
    QTextEdit *matchCodeText;

    QStringList CodeNameData;

    void getBoardData(QList<StockInfo> *&pTableList);

signals:
    void showSearch();
private:
    GlobalVar *m_pGlobalVar;
    QList<StockInfo> *m_pTableList;
//    QNetworkAccessManager *naManager=new QNetworkAccessManager(this);
    void matchInput();
    void searchFinished();
    bool compare(QStringList Data,QString s,int col);

};

#endif // SEARCHSTOCK_H
