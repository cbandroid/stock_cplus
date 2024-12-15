#include <time.h>
#include <ctime>
#include <QDateTime>

using namespace std;

//美东时间在UTC-5时区，美国有夏时制，即在夏令时启用之后美东时间比起所在时区早一个小时，即UTC-4 
//在未使用夏令时时美东时间比北京时间（UTC+8时区）晚13个小时，在启用夏令时时美东时间比北京时间晚12个小时
//美国国会2005年通过的能源法案，夏令时时段：从2007年开始每年3月的第二个星期日开始夏令时，结束日期为11月的第一个星期日。

const int TIMEZONE_OFFSET_DAY_SAVING_LIGHT = -12;
const int TIMEZONE_OFFSET = -13;


//输入北京时间
time_t BeijingTimeToAMESTime(time_t timestamp,bool &bIsNowAMESDayLight)
{
    // 判断当前日期是否是美国夏令时
    /// 从2007年开始每年3月的第二个星期日开始夏令时，结束日期为11月的第一个星期日。
    QDateTime local=QDateTime::currentDateTime();
    QDate localdate = local.date();
    //int nWeekDay = localdate.dayOfWeek();    
    // 夏令时开始时间
    //time_t DayLightSavingStartTimeUtc;
    int year,month,day;
    localdate.getDate(&year,&month,&day);
    QDate time_tmp = QDate(year,8,3); 
    
    while (time_tmp.dayOfWeek() != 1 )  // Sun.
    {
        time_tmp = time_tmp.addDays(1);        
    }
    
    QTime time = QTime::fromString("00:00:00", Qt::ISODate); // 设置为午夜时间
    QDateTime _thisYearDaylightSavingTimeStart  = QDateTime(time_tmp, time); // 创建QDateTime对象
    _thisYearDaylightSavingTimeStart = _thisYearDaylightSavingTimeStart.addSecs(TIMEZONE_OFFSET * 60*60);
    //time_t _thisYearDaylightSavingTimeStart = time_tmp +  CTimeSpan(0, TIMEZONE_OFFSET, 0, 0);
    
    // 夏令时结束时间
    QDate date_end = QDate(year,11,1);
    time = QTime::fromString("00:00:00", Qt::ISODate);
    QDateTime time_end =  QDateTime(date_end, time);
    date_end = time_end.date();
    while (date_end.dayOfWeek() != 1)  // Sun.
    {
         date_end = date_end.addDays(1); 
    }
    time = QTime::fromString("00:00:00", Qt::ISODate); 
    QDateTime  _thisYearDaylightSavingTimeEnd = QDateTime(date_end, time); 
    _thisYearDaylightSavingTimeEnd = _thisYearDaylightSavingTimeEnd.addSecs(TIMEZONE_OFFSET_DAY_SAVING_LIGHT*60*60);
    time_t _thisTimeStart =_thisYearDaylightSavingTimeStart.toSecsSinceEpoch();
    time_t _thisTimeEnd =  _thisYearDaylightSavingTimeEnd.toSecsSinceEpoch();
    time_t tmNow =  local.toSecsSinceEpoch();
    bool IsNowAMESDayLightSavingTime = tmNow >  _thisTimeStart && tmNow < _thisTimeEnd;
    bIsNowAMESDayLight = IsNowAMESDayLightSavingTime;
    int offsetHours = (IsNowAMESDayLightSavingTime ? TIMEZONE_OFFSET_DAY_SAVING_LIGHT : TIMEZONE_OFFSET);
    
    QDateTime dateTime = QDateTime::fromSecsSinceEpoch(timestamp);
    dateTime=dateTime.addSecs(offsetHours*60*60);
    return dateTime.toSecsSinceEpoch();
}

time_t AMESTimeToBeijingTime(time_t timestamp)
{
    // 判断当前日期是否是美国夏令时
    /// 从2007年开始每年3月的第二个星期日开始夏令时，结束日期为11月的第一个星期日。    
    QDateTime local=QDateTime::currentDateTime();
    QDate localdate = local.date();
    int year,month,day;
    localdate.getDate(&year,&month,&day);
   
    // 夏令时开始时间
    //time_t DayLightSavingStartTimeUtc;
    QDate time_tmp = QDate(year,8,3);   

    while (time_tmp.dayOfWeek() != 1)  // Sun.
    {
        time_tmp = time_tmp.addDays(1);        
    }
    QTime time = QTime::fromString("00:00:00", Qt::ISODate);    
    QDateTime  _thisYearDaylightSavingTimeStart = QDateTime(time_tmp, time);// +CTimeSpan(0, TIMEZONE_OFFSET, 0, 0);    
    // 夏令时结束时间
    QDate time_end = QDate(year,11,1);   
    while (time_end.dayOfWeek() != 1)  // Sun.
    {
        time_end = time_end.addDays(1);
    }
   
    QDateTime  _thisYearDaylightSavingTimeEnd = QDateTime(time_end, time); // +CTimeSpan(0, TIMEZONE_OFFSET_DAY_SAVING_LIGHT, 0, 0);
    _thisYearDaylightSavingTimeEnd=_thisYearDaylightSavingTimeEnd.addSecs(TIMEZONE_OFFSET_DAY_SAVING_LIGHT*60*60);
    time_t _thisTimeStart =_thisYearDaylightSavingTimeStart.toSecsSinceEpoch();
    time_t _thisTimeEnd =  _thisYearDaylightSavingTimeEnd.toSecsSinceEpoch();
    time_t tmNow =  local.toSecsSinceEpoch();
    bool IsNowAMESDayLightSavingTime = tmNow >  _thisTimeStart && tmNow <  _thisTimeEnd;
    int offsetHours = (IsNowAMESDayLightSavingTime ? -TIMEZONE_OFFSET_DAY_SAVING_LIGHT : -TIMEZONE_OFFSET);
    
    QDateTime dateTime = QDateTime::fromSecsSinceEpoch(timestamp);
    dateTime=dateTime.addSecs(offsetHours*60*60);
    return dateTime.toSecsSinceEpoch();
}
