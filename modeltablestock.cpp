#include "utilityex.h"
#include "globalvar.h"
#include "modeltablestock.h"

ModelTableStock::ModelTableStock(GlobalVar *pGlobalVar,QStringList *&pMyStockCode,QObject *parent)
    : QAbstractTableModel(parent)
{
    m_pGlobalVar=pGlobalVar;
    m_pMyStockCode=pMyStockCode;
    mark=true;
    myself=false;
    tableHeader<<"代码"<<"名称"<<"涨速"<<"涨跌幅"<<"最新价"<<"换手率"<<"成交额"<<"总市值"<<"市盈率"<<"流通市值"<<"今年"<<"60日"<<"成交量"<< "最高"<< "最低"<< "今开"<<"昨收";
}

void ModelTableStock::setModelData(const QList<StockInfo> &data,bool forced,bool marking,bool selfSymbol)
{
    mark=marking;
    myself = selfSymbol;
    if (m_modelData.size()!=data.size() or forced)
    {
        beginResetModel();
        m_modelData = data;
        endResetModel();
    }
    else
    {
        m_modelData = data;
        emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
    }
}

int ModelTableStock::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_modelData.size();
}

int ModelTableStock::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return COLNUMS;
}

QVariant ModelTableStock::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) // 返回一个默认数据的表头
        return QAbstractTableModel::headerData(section, orientation, role);

    // 仅仅只是返回水平方向的表头
    if (orientation == Qt::Orientation::Horizontal) {
        return tableHeader[section];
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant ModelTableStock::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        //DisplayRole返回显示的文本值
        const int row = index.row();
        switch(index.column())
        {
        case 0: return m_modelData.at(row).code;
        case 1: return m_modelData.at(row).name;
        case 2: return FormatNumber(m_modelData.at(row).velocity,2,"");
        case 3: {
            if (m_modelData.at(row).pctChg!=0)
              return format_conversion(m_modelData.at(row).pctChg)+"%";
            else
                return ("--");
        }
        case 4:
        {
            int d=2;
            if (m_pGlobalVar->WhichInterface==UKMARKET or m_pGlobalVar->WhichInterface==HKMARKET)
                d=3;
            return FormatNumber(m_modelData.at(row).close,d,"");
        }
        case 5: return m_modelData.at(row).turn;
        case 6: return format_conversion(m_modelData.at(row).amount);
        case 7: return format_conversion(m_modelData.at(row).totalValue);
        case 9: return format_conversion(m_modelData.at(row).circulatedValue);
        case 8: return FormatNumber(m_modelData.at(row).pe,2,"");
        case 10: return FormatNumber(m_modelData.at(row).pctYear,2,"%");
        case 11: return FormatNumber(m_modelData.at(row).pctSixty,2,"%");
        case 12: return format_conversion(m_modelData.at(row).volume);
        case 13: return FormatNumber(m_modelData.at(row).high,2,"");
        case 14: return FormatNumber(m_modelData.at(row).low,2,"");
        case 15: return FormatNumber(m_modelData.at(row).open,2,"");
        case 16: return FormatNumber(m_modelData.at(row).preClose,2,"");
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        const int row = index.row();
        switch(index.column())
        {
        case 0:
        return QColor(0, 0, 0); //QColor(128,128,0);
        case 1:
            if (mark and not myself)
            {
                int nCount=m_pMyStockCode->count();
                /*int mid,l = 0;
                int r = nCount - 1;
                while (l <= r)
                {
                    mid = (l + r) / 2;
                    if (m_modelData.at(row).code==m_pGlobalVar->mMyStockCode[mid])
                        return QColor(255,140,0);
                    else if (m_pGlobalVar->mMyStockCode[mid]> m_modelData.at(row).code)
                        r = mid - 1;
                    else
                        l = mid + 1;
                }*/
                for (int i=0;i<nCount;++i)
                {
                    if (m_modelData.at(row).code== m_pMyStockCode->at(i))
                        return QColor(255,140,0);
                }

            }
            return QColor(0,0,0); // QColor(72,61,139);
        case 2:
            if (m_modelData.at(row).velocity>= 2)
                return QColor(153, 0, 153);
            else if (m_modelData.at(row).velocity >0)
                return QColor(Qt::red);
            else if (m_modelData.at(row).velocity < 0)
                return QColor(0, 191, 0);
            break;
        case 3:
            if (m_modelData.at(row).pctChg>0)
                return QColor(Qt::red);
            else if (m_modelData.at(row).pctChg<0)
                return QColor(0, 191, 0);
            break;
        case 4:
            if (m_modelData.at(row).close>m_modelData.at(row).open)
                return QColor(255, 0, 255);
            else if (m_modelData.at(row).close<m_modelData.at(row).open)
                return QColor(0, 191, 0);
            break;
        case 5:
            if (m_modelData.at(row).turn>=15)
                return QColor(204, 204, 0);
            break;
        case 6:
            if (m_modelData.at(row).amount>= 1000000000)
                return QColor(153, 0, 153);
            else if (m_modelData.at(row).amount >= 300000000)
                return QColor(0, 191, 255);
            break;
        case 7:
            if (m_modelData.at(row).totalValue/100>100000000)
                return QColor(32,178,170);
            break;
        case 8:
            if (m_modelData.at(row).pe<0)
                return QColor(0, 191, 0);
            break;
        case 9:
            if (m_modelData.at(row).circulatedValue/100>100000000)
                return QColor(32,178,170);
            break;
        case 10:
            if (m_modelData.at(row).pctYear >= 100)
                return QColor(153, 0, 153);
            else if (m_modelData.at(row).pctYear < 100 and m_modelData.at(row).pctYear >0)
                return QColor(255, 155, 153);
            else if (m_modelData.at(row).pctYear < 0)
                return QColor(0, 191, 0);
            break;
        case 11:
            if (m_modelData.at(row).pctSixty >= 100)
                return QColor(153, 0, 153);
            else if (m_modelData.at(row).pctSixty < 100 and m_modelData.at(row).pctSixty >0)
                return QColor(255, 155, 153);
            else if (m_modelData.at(row).pctSixty < 0)
                return QColor(0, 191, 0);
            break;
        case 13:
            if (m_modelData.at(row).high>m_modelData.at(row).preClose)
                return QColor(Qt::red);
            else if (m_modelData.at(row).high<m_modelData.at(row).preClose)
                return QColor(0, 191, 0);
            break;
        case 14:
            if (m_modelData.at(row).low>m_modelData.at(row).preClose)
                return QColor(Qt::red);
            else if (m_modelData.at(row).low<m_modelData.at(row).preClose)
                return QColor(0, 191, 0);
            break;
        case 15:
            if (m_modelData.at(row).close>m_modelData.at(row).open)
                    return QColor(255, 0, 255);
            else if (m_modelData.at(row).close<m_modelData.at(row).open)
                    return QColor(0, 191, 0);
            break;
        }
    }
    else if (role == Qt::FontRole)
    {
        QFont boldFont = QFont();
        if (index.column() == 1)
            boldFont.setFamily("宋体");
        if (index.column() == 3 or index.column() == 1 or index.column() == 6)
        {
            boldFont.setBold(true);
            boldFont.setPixelSize(16);
            return boldFont;
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        switch(index.column())
        {
        case 2:return Qt::AlignRight;
        case 3:return Qt::AlignRight;
        case 4:return Qt::AlignRight;
        case 5:return Qt::AlignRight;
        case 6:return Qt::AlignRight;
        case 7:return Qt::AlignRight;
        case 8:return Qt::AlignRight;
        case 9:return Qt::AlignRight;
        case 10:return Qt::AlignRight;
        case 11:return Qt::AlignRight;
        case 12:return Qt::AlignRight;
        case 13:return Qt::AlignRight;
        case 14:return Qt::AlignRight;
        case 15:return Qt::AlignRight;
        case 16:return Qt::AlignRight;
        }
    }
    return QVariant();
}

