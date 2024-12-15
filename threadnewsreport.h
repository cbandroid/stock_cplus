
#ifndef THREADNEWSREPORT_H
#define THREADNEWSREPORT_H
#include <QNetworkReply>
#include <QTextToSpeech>
//#include <QRegularExpression>
//#include <QRegularExpressionMatch>
class GlobalVar;

class ThreadNewsReport : public QObject
{
    Q_OBJECT
public:
    explicit ThreadNewsReport(GlobalVar *pGlobalVar,QObject *parent = nullptr);
    void getNewsData();
    bool isQuit=false;
    QTextToSpeech *tts;//=new QTextToSpeech(this);
    void SpeechNow();
private:
    QByteArray allData;
//    QNetworkAccessManager *naManager =new QNetworkAccessManager(this);
    QString jinShiNewsReportCurTime;
    int64_t eastNewsReportCurTime;
    QList<QStringList> eastNewsList;
    int count=0;
    bool HasAudioDevices=false;
    QString id;
    QString newsContent;
    void initEastNews() ;
    void initNewsReport() ;
    void initEastNewsReport() ;
    void sayJsNews(QJsonObject object)  ;
    void sayEastNews(QStringList l,int time) ;
    GlobalVar *m_pGlobalVar;
signals:
    void getNewsFinished(QString s);

};

#endif // THREADNEWSREPORT_H
