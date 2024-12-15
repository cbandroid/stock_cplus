#ifndef _KDATA_H_
#define _KDATA_H_

#include <./include/StructKlineView.h>

inline bool operator == (const CL::Kline& pTemp1, const CL::Kline& pTemp2)
{
	return //strcmp(pTemp1.szCode, pTemp2.szCode) == 0
         pTemp1.date == pTemp2.date;
}

inline bool operator < (const CL::Kline& pTemp1, const CL::Kline& pTemp2)
{
	return// strcmp(pTemp1.szCode, pTemp2.szCode) == 0
         pTemp1.date < pTemp2.date;
}
//K线数据数组类
//template class FACILITY_API array_container<kdata_t>;
class ReportContainer;

class CKData :public std::vector<CL::Kline>
{
public:
	//K线数据类型;
	enum Formats{
		formatNone=0x00,
		formatMin=0x01,
		formatOriginal=0x01,
		formatXDRup=0x02,
		formatXDRdown=0x03,
		formatMax=0x03
	};
	enum MaindataTypes{
		mdtypeNone=0x00,
		mdtypeMin=0x01,
		mdtypeHigh=0x01,
		mdtypeClose=0x02,
		mdtypeOpen=0x03,
		mdtypeAverage=0x04,
		mdtypeLow=0x05,
		mdtypeVolume=0x06,
		mdtypeTrunover=0x07,
		mdtypeOpenInterest=0x08,
		mdtypeMax=0x07
	};
    virtual ~CKData();
	CKData();
	CKData(int ktype,int maindatatype=mdtypeClose);
    explicit CKData( const CKData & src );
	CKData& operator=(const CKData& src);
	static bool IsDayOrMin(int nKType);
public:
	//数组操作函数;
	int GetUpperBound() const;
	//void SetAt(int nIndex,KDATA newElement);

    int  InsertKDataSort(const CL::Kline& newElement );
	//目前还不确定要如何管理时间;
	//与K线相关的函数
	//得到kdata_container的nIndex日的主数据，根据主数据类型不同，返回值可以是开盘价，收盘价或平均价;
    float MaindataAt(int nIndex) const;
	//得到nIndex日的具体时间,这个函数用的似乎少;
    bool DateAt(int nIndex,int &nYear,int &nMonth,int &nDay,int &nHour,int &nMinute) const;
	//获得最近日期;
	bool LatestDate(int &nYear,int &nMonth,int &nDay,int &nHour,int &nMinute) const;
	//通过索引获得日期;
	
    time_t GetDate(int nIndex);
	time_t GetDateDay(int nIndex);

	//通过日期获得索引;
	int GetIndexByDate(time_t date );
	int GetAboutIndexByDate(time_t date);
	
    bool IsNewValue(int nIndex,bool bTopOrBottom,int nDays);
    bool GetMinMaxInfo(int nStart,int nEnd,float* pfMin,float* pfMax);
	//根据TICK更新数据;
	//bool update(const Tick* pTick);
    bool update(const CL::Kline* pKData);
public:
	void SetKType(int ktype);
	void AutoSetKType();
	int GetKType();
	int GetCurFormat();
	void ChangeCurFormat(int format,time_t dateAutoDRBegin,float dAutoDRLimit);
	void SetMaindataType(int type);
	int GetMaindataType();
	void Clear();
	int CompareLatestDate(CKData& kd);
	static int Min5ToMin15(CKData& kdm5,CKData& kdm15);
	static int Min5ToMin30(CKData& kdm5,CKData& kdm30);
	static int Min5ToMin60(CKData& kdm5,CKData& kdm60);
	//将1分钟线转换成日线;
	static int Min1ToDay(CKData& kdm15, CKData& kdday);
	static int DayToMonth(CKData& kdday,CKData& kdmonth);
	static int DayToWeek(CKData& kdday,CKData& kdweek);
	time_t	ToDayDate( time_t date );

	int		MergeKData( CKData * pother );
	int		FullFillKData( CKData & kdataMain, bool bFillToEnd );

    bool	IsAdjacentDays( int nIndex,int nDays );
	bool	GetDiff( float * pValue, time_t dateCur, int nDays );
	bool	GetDiffPercent( float * pValue, time_t dateCur, int nDays );
	bool	GetScope( float * pValue, time_t dateCur, int nDays );
	bool	GetVolumeSum( float * pValue, time_t dateCur, int nDays );
	bool	GetRatioVolume( float * pValue, time_t dateCur, int nDays );
	bool	GetRS( float * pValue, time_t dateCur, int nDays );
    bool	GetMA(float * pValue, int nIndex, int nDays);
    bool	GetMA(float& pValue, int nIndex, int nDays);

	float  GetMA(int nIndex, int nDays);

	static int ConvertKData( CKData &kdSrc, CKData &kdDest, int multiple );
	//将K线转换成Tick线;
	int		ToReport(ReportContainer & kdata);
protected:
	//void	CopyData( const kdata_container &src );
	void	LoadDataOriginal( );
	void	StoreDataOriginal( );
	void	ConvertXDR( bool bUP, time_t dateAutoDRBegin, float dAutoDRLimit );
	
	//static	float	GetRatio( float fLastClose, DRDATA & dr );
protected:
	int		m_nKType;
	int		m_nCurFormat;
	int		m_nCurMaindataType;
	struct kdata_t * m_pDataOriginal;
	int m_nSizeOriginal;     // # of elements (upperBound - 1)
	int m_nMaxSizeOriginal;  // max allocated
	//TickSPtr m_LastTickPtr;
};

#endif
