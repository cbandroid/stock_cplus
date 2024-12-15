#include "globalvar.h"
#include "searchstock.h"
#include "NetworkManager.h"

SearchStock::SearchStock(GlobalVar *pGlobalVar)
{
    m_pGlobalVar =pGlobalVar;
//    naManager = new QNetworkAccessManager(this);
    searchCodeLine=new QLineEdit(this);
    searchCodeLine->setStyleSheet("background-color: rgb(211, 211, 211);");
    matchCodeText=new QTextEdit(this);
    connect(searchCodeLine,&QLineEdit::textChanged,this,&SearchStock::matchInput);
    connect(searchCodeLine,&QLineEdit::returnPressed,this,&SearchStock::searchFinished);
}

void SearchStock::matchInput()
{
    matchCodeText->clear();
    m_pGlobalVar->curBlock=0;
    QString str=searchCodeLine->text();
    if (not str.isEmpty())
    {
      QStringList Data;
      bool ok;
      if (m_pGlobalVar->WhichInterface!=CNMARKET)
      {
         QTextCharFormat charFmt=QTextCharFormat();
         charFmt.setForeground(QColor("black"));
         matchCodeText->mergeCurrentCharFormat(charFmt);
         if (m_pGlobalVar->WhichInterface==USMARKET || m_pGlobalVar->WhichInterface==UKMARKET)
         {
             QString strUpper=str.toUpper();
             Data= m_pGlobalVar->CodeNameData;
             if (!compare(Data,strUpper,0))
             {
               Data=m_pGlobalVar->PinYinData;
               compare(Data,strUpper,2);
             }
         }
         else if (m_pGlobalVar->WhichInterface==HKMARKET)
         {
           str.toInt(&ok);
           if (ok){
             Data= m_pGlobalVar->CodeNameData;
             compare(Data,str,0);
            }
            else
            {
              QString strUpper=str.toUpper();
              Data=m_pGlobalVar->PinYinData;
              compare(Data,strUpper,2);
            }
         }
      }
      else
      {

        QTextCharFormat charFmt=QTextCharFormat();
        charFmt.setForeground(QColor("black"));
        matchCodeText->mergeCurrentCharFormat(charFmt);
        QTextCodec *codec;
        str.toInt(&ok);
        if (ok){
          QFile digitFile(m_pGlobalVar->currentPath+"/list/digit_list.csv");
          if (digitFile.open(QIODevice::ReadOnly))
          {
            codec = QTextCodec::codecForName("GBK");
            Data=codec->toUnicode(digitFile.readAll()).split("\n",Qt::SkipEmptyParts);
            compare(Data,str,0);
           }
        }
        else
        {
          QString strUpper=str.toUpper();
          QFile file(m_pGlobalVar->currentPath+"/list/abbreviation_list.csv");

          if (file.open(QIODevice::ReadOnly))
          {
            codec = QTextCodec::codecForName("GBK");
            Data=codec->toUnicode(file.readAll()).split("\n",Qt::SkipEmptyParts);
            compare(Data,strUpper,2);
          }
        }
      }
      matchCodeText->moveCursor(QTextCursor::Start);
      QTextBlockFormat blockFmt=QTextBlockFormat();
      blockFmt.setBackground(QColor("white"));
      matchCodeText->textCursor().setBlockFormat(blockFmt);
      QTextBlock block=matchCodeText->document()->findBlockByNumber(0);
      matchCodeText->setTextCursor(QTextCursor(block));
      blockFmt=matchCodeText->textCursor().blockFormat();
      blockFmt.setBackground(QColor(0, 199, 255));
      matchCodeText->textCursor().setBlockFormat(blockFmt);
    }
}

void SearchStock::searchFinished()
{
    QStringList cn=matchCodeText->document()->findBlockByNumber(m_pGlobalVar->curBlock).text().split(" ");
    m_pGlobalVar->isBoard=false;
    int cn_len =  cn.count();
    if (cn.contains(">>> ") or  cn_len< 3)
        return;
    qDebug() << "searchFinished |" << cn.at(1)  <<"| " <<  cn_len <<"|"<< cn.join(" ") << " |" << cn.size();
    if (cn.size() <3)
          return;
    QString strSymbol="";
    if (m_pGlobalVar->WhichInterface!=CNMARKET)
    {
        if (m_pGlobalVar->WhichInterface==USMARKET)
           strSymbol=cn.at(1);
        else if (m_pGlobalVar->WhichInterface==UKMARKET)
          strSymbol=cn.at(1);
        else
           strSymbol=cn.at(1);

         qDebug() <<" strSymol="<< strSymbol;
         m_pGlobalVar->curCode=strSymbol;
    }
    else
    {
        strSymbol=cn.at(1);//.right(6);
        if ( cn_len>2 and cn.at(2)=="指数")
        {
          if (strSymbol.left(1)=="0")
            m_pGlobalVar->curCode="1."+strSymbol;
         else if (strSymbol.left(1)=="B")
             m_pGlobalVar->curCode="90."+strSymbol;
          else
            m_pGlobalVar->curCode=strSymbol;
        }
        else if ( cn_len>3 and cn.at(3)=="板块")
        {
          m_pGlobalVar->curBoard=strSymbol;
          m_pGlobalVar->isBoard=true;
          getBoardData(m_pTableList);
        }
        else
         m_pGlobalVar->curCode=strSymbol;
    }
    if (strSymbol.isEmpty())
          return;
    if ( cn_len >3)
     m_pGlobalVar->curName=cn.at(2);

    qDebug() <<" strSymbol="<< strSymbol;

    emit showSearch();
    matchCodeText->clear();
}

bool SearchStock::compare(QStringList Data,QString strUpper,int col)
{
    int num=Data.count();
    QStringList strLine;
    int matchNum=0;
    int mid,l = 0;
    int r = num - 1;
    int m=0;
    QString blank=" ";
    QString class_,strSymol;
    int size;
    bool result=false;
    while (l <= r)
    {
        mid = (l + r) / 2;
        strLine = Data.at(mid).split(",");
        size =  strLine.size();
        if (col >= size){
           matchNum=mid;
           break;
        }
       // qDebug() << strLine;
        if(strUpper==strLine.at(col).left(searchCodeLine->text().length()))
        {
            matchNum=mid;
            result=true;
            strSymol=strLine.at(0);
            if (m_pGlobalVar->WhichInterface!=CNMARKET){

                if (m_pGlobalVar->WhichInterface==USMARKET){
                    class_="美股";
                    matchCodeText->append(strLine.at(3)+"."+strSymol+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);
                }
                else if (m_pGlobalVar->WhichInterface==UKMARKET){
                    class_="英股";
                    matchCodeText->append(strLine.at(3)+"."+strSymol+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);
                }
                else{
                     class_="港股";
                     matchCodeText->append(strSymol+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);
                }
            }
            else
            {
             if (strSymol.right(1)=="G" or strSymol.left(3)=="399" or strSymol.left(3)=="899")
                class_="指数";
             else if(strSymol.left(1)=="B")
                class_="板块";
             else if(strSymol.left(1)=="6")
                class_="上海A股";
             else if(strSymol.left(1)=="0")
                class_="深圳A股";
             else if(strSymol.left(1)=="8" or strSymol.left(1)=="4")
                class_="北京A股";
             else if(strSymol.left(1)=="3")
                class_="创业板";
             matchCodeText->append(strSymol+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);
            // if (class_=="板块")
            //     matchCodeText->append(s.mid(0,6)+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+"指数");
            }
			break;
        }
        else if ( strUpper<strLine.at(col).left(searchCodeLine->text().length()))
            r = mid - 1;
        else
            l = mid + 1;
    }

    for (int i=matchNum-1;i>=0;--i)
    {
        strLine = Data.at(i).split(",");
        size =  strLine.size();
        if (col >= size)
            break;
        strSymol=strLine.at(0);
        if (m_pGlobalVar->WhichInterface!=CNMARKET){
            if (m_pGlobalVar->WhichInterface==USMARKET)
                class_="美股";
            else if (m_pGlobalVar->WhichInterface==UKMARKET)
                class_="英股";
            else
                class_="港股";
            if ( strUpper==strLine.at(col).left(searchCodeLine->text().length()))
            {
                result=true;
                if (m_pGlobalVar->WhichInterface==HKMARKET)
                    matchCodeText->append(strSymol+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);
                else
                    matchCodeText->append(strLine.at(3)+"."+strSymol+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);

                m+=1;
                if (m>100)
                    break;
            }
            else
                break;
        }
        else
        {
         if (strSymol.right(1)=="G" or strSymol.left(3)=="399")
            class_="指数";
         else if(strSymol.left(1)=="B")
            class_="板块";
         else if(strSymol.left(1)=="6")
            class_="上海A股";
         else if(strSymol.left(1)=="0")
            class_="深圳A股";
         else if(strSymol.left(1)=="8" or strSymol.left(1)=="4")
            class_="北京A股";
         else if(strSymol.left(1)=="3")
            class_="创业板";
         if (strUpper==strLine.at(col).left(searchCodeLine->text().length()))
         {
            result=true;
            matchCodeText->append(strSymol+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);
            // if (class_=="板块")
            //     matchCodeText->append(s.mid(0,6)+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+"指数");
            m+=1;
            if (m>100)
                break;
         }
         else
            break;
        }
    }
    for (int i=matchNum+1;i<num;++i)
    {
        strLine = Data.at(i).split(",");
        size =  strLine.size();
        if (col >= size)
            break;
        strSymol=strLine.at(0);

        if (m_pGlobalVar->WhichInterface!=CNMARKET){
            if (m_pGlobalVar->WhichInterface==USMARKET)
                class_="美股";
            else if (m_pGlobalVar->WhichInterface==UKMARKET)
                class_="英股";
            else
                class_="港股";
            if ( strUpper==strLine.at(col).left(searchCodeLine->text().length()))
            {
                result=true;
                if (m_pGlobalVar->WhichInterface==HKMARKET)
                   matchCodeText->append(strSymol+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);
                else
                    matchCodeText->append(strLine.at(3)+"."+strSymol+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);
                m+=1;
                if (m>100)
                    break;
            }
            else
                break;
        }
        else
        {
         if (strSymol.right(1)=="G" or strSymol.left(3)=="399")
            class_="指数";
         else if(strSymol.left(1)=="B")
            class_="板块";
         else if(strSymol.left(1)=="6")
            class_="上海A股";
         else if(strSymol.left(1)=="0")
            class_="深圳A股";
         else if(strSymol.left(1)=="8" or strSymol.left(1)=="4")
            class_="北京A股";
         else if(strSymol.left(1)=="3")
            class_="创业板";
         if ( strUpper==strLine.at(col).left(searchCodeLine->text().length()))
         {
            result=true;
            matchCodeText->append(strSymol+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);
            // if (class_=="板块")
            //     matchCodeText->append(s.mid(0,6)+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+"指数");
            m+=1;
            if (m>100)
                break;
         }
         else
            break;
        }
    }
    return result;
}

void SearchStock::getBoardData(QList<StockInfo> *&pTableList)
{

    m_pTableList=pTableList;
    QByteArray allData;

    QString url="http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=6000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=1&fid="+m_pGlobalVar->columns[m_pGlobalVar->curSortNum[1]]+"&fs=b:"+m_pGlobalVar->curBoard+"+f:!50&fields=f2,f3,f5,f6,f8,f9,f12,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667954879297";
    NetworkManager networkManager;
    allData = networkManager.getSync<QByteArray>(QUrl(url));

    if (allData.isEmpty())
        return;
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        pTableList->clear();

       // QList<StockInfo> mTableList;
        // m_pGlobalVar->mTableList.clear();
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();
        QJsonValue value ;
        QVariantMap ceilMap;
        int nSize=data.size();
        StockInfo info;
        for (int i = 0; i < nSize; ++i)
        {
            value = data.at(i);
            ceilMap = value.toVariant().toMap();  
            info.code = ceilMap.value("f12").toString();
            info.name = ceilMap.value("f14").toString();
            info.close = ceilMap.value("f2").toFloat();
            info.pctChg=ceilMap.value("f3").toFloat();
            info.turn=ceilMap.value("f8").toFloat();
            info.amount=ceilMap.value("f6").toFloat();
            info.velocity = ceilMap.value("f22").toFloat();
            info.pe = ceilMap.value("f9").toFloat();
            info.totalValue = ceilMap.value("f20").toFloat();
            info.circulatedValue=ceilMap.value("f21").toFloat();
            info.pctYear=ceilMap.value("f25").toFloat();
            info.pctSixty=ceilMap.value("f24").toFloat();
            info.volume = ceilMap.value("f5").toFloat();
            info.high = ceilMap.value("f15").toFloat();
            info.low = ceilMap.value("f16").toFloat();
            info.open=ceilMap.value("f17").toFloat();
            info.preClose=ceilMap.value("f18").toFloat();
            pTableList->append(info);
        }

    }
    allData.clear();
}
