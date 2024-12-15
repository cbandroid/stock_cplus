
#ifndef MODELTABLESTOCK_H
#define MODELTABLESTOCK_H

#include <QAbstractTableModel>
#include "stockinfo.h"
#include <qfont.h>

class GlobalVar;

class ModelTableStock : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ModelTableStock(GlobalVar *pGlobalVar,QStringList *&pMyStockCode,QObject *parent = nullptr);
    void setModelData(const QList<StockInfo> &data,bool forced,bool marking,bool selfSymbol);
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QStringList tableHeader;
    bool mark;
    bool myself;
private:
    GlobalVar *m_pGlobalVar;
//    QStringList tableList;
    QList<StockInfo> m_modelData;
    QStringList *m_pMyStockCode;
};

#endif // MODELTABLESTOCK_H
