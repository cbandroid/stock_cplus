#ifndef MODELFUNDFLOW_H
#define MODELFUNDFLOW_H

#include <QAbstractTableModel>
#include <qfont.h>
class GlobalVar;

class ModelFundFlow : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ModelFundFlow(GlobalVar *pGlobalVar,QObject *parent = nullptr);
    void setModelData(const QList<QStringList> &data,bool forced);
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QStringList tableHeader;

private:
    //    QStringList tableList;
    QList<QStringList> m_modelData;
    GlobalVar *m_pGlobalVar;
};

#endif // MODELFUNDFLOW_H
