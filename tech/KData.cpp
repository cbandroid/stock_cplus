
//#include "DateTime.h"
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
//#include "InstrumentInfo.h"
//#include "Container.h"
//#include <boost/make_shared.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/date_time/posix_time/ptime.hpp>
//#include "../SimpleLogLib/logging.h"
//#include "DateTimeHelper.h"
//#include "Report.h"
#include "KData.h"

#define ktypeDay   6
#define ktypeWeek  7
#define ktypeMonth 8
    
CKData::CKData()
{
	//LOGINIT("kdata_container");
    m_nKType	=	ktypeDay;
	m_nCurFormat=	formatOriginal;
	m_nCurMaindataType	=	mdtypeClose;
    m_pDataOriginal	=	nullptr;
	m_nSizeOriginal = m_nMaxSizeOriginal = 0;
	//m_pData	=	NULL;
	//m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

CKData::CKData( int ktype, int maindatatype )
{
	//LOGINIT("kdata_container");
	m_nKType	=	ktype;
	m_nCurFormat=	formatOriginal;
	m_nCurMaindataType	=	maindatatype;
    m_pDataOriginal	=	nullptr;
	m_nSizeOriginal = m_nMaxSizeOriginal = 0;
	//m_pData = NULL;
	//m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

CKData::CKData( const CKData &src )
{
  //  Clear();
	//LOGINIT("kdata_container");
	/*m_nKType	=	ktypeDay;
	m_nCurFormat=	formatOriginal;
	m_nCurMaindataType	=	mdtypeClose;
    m_pDataOriginal	=	nullptr;
	m_nSizeOriginal = m_nMaxSizeOriginal = 0;*/
    m_nKType			=	src.m_nKType;
    m_nCurFormat		=	src.m_nCurFormat;
    m_nCurMaindataType	=	src.m_nCurMaindataType;

    m_pDataOriginal = src.m_pDataOriginal;
    m_nSizeOriginal = src.m_nSizeOriginal;     // # of elements (upperBound - 1)
    m_nMaxSizeOriginal = src.m_nMaxSizeOriginal;  // max allocated

	//m_pData	=	NULL;
	//m_nSize = m_nMaxSize = m_nGrowBy = 0;
	//*this	=	src;
    std::copy(src.begin(),src.end(),std::back_inserter(*this));
    //std::vector<CL::Kline>::operator =(src);
}

CKData	& CKData::operator = ( const CKData &src )
{
	Clear();

	m_nKType			=	src.m_nKType;
	m_nCurFormat		=	src.m_nCurFormat;
	m_nCurMaindataType	=	src.m_nCurMaindataType;   

    m_pDataOriginal = src.m_pDataOriginal;
    m_nSizeOriginal = src.m_nSizeOriginal;     // # of elements (upperBound - 1)
    m_nMaxSizeOriginal = src.m_nMaxSizeOriginal;  // max allocated

	//CopyData( src );
	/**this = src;*/
	//std::copy(src.begin(),src.end(),std::back_inserter(*this));
    std::vector<CL::Kline>::operator =(src);
	return	*this;
}
/**
 * \brief 时间周期是按天还时按份钟;
 *
 */
bool CKData::IsDayOrMin( int nKType )
{
	return (nKType == ktypeDay || nKType == ktypeWeek || nKType == ktypeMonth);
}

CKData::~CKData()
{
	Clear();
}

/**
 * \brief 获取最近的时间;
 */
bool CKData::LatestDate( int &nYear, int &nMonth, int &nDay, int &nHour, int &nMinute ) const
{
	nYear	=	nMonth	=	nDay	=	nHour	=	nMinute	=	0;
	if( size() > 0 )
		return DateAt( size()-1, nYear, nMonth, nDay, nHour, nMinute );
	return false;
}
//取得K线数据中某个点的时间;
//如果索引值大于K线数组的元素个数，则返回最后一个元素的日期;
time_t CKData::GetDate(int nIndex )
{
    int nSize =(int)size();
    if ( nIndex >=0 and nIndex <nSize ){
        return at(nIndex).date;
    }
    if( nSize > 0 ){
        return at(nSize-1).date;
    }
	return 0;
}

time_t CKData::GetDateDay( int nIndex )
{
	time_t	date	=	GetDate(nIndex);
// 	if( !IsDayOrMin(m_nKType) )
// 	{
// 		DateTime	sptime;
// 		if(!sptime.FromInstrumentTimeMin( date ) )
// 			return sptime.ToInstrumentTimeDay();
// 	}
	return date;
}
//-1为无效索引;
int CKData::GetIndexByDate( time_t date )
{
    if( size() > 0 && at(size()-1).date < date )
		return -1;
    if( size() > 0 && at(0).date > date )
		return -1;
	//这里可以使用二分插入方法;
	for( int k=size()-1; k >= 0 ; k-- )
	{
        if( at(k).date ==date)
			return k;
        else if( at(k).date<date )
			break;
	}
	return -1;
}
/**
 * \brief 返回小于或等于指定日期的最大值;
 *
 */
int CKData::GetAboutIndexByDate( time_t date )
{
	for( int k=size()-1; k >= 0 ; k-- )
	{
        if(at(k).date<=date)
			return k;
	}
	return -1;
}

int CKData::InsertKDataSort(const CL::Kline& newElement )
{
	for( size_t i=0; i<size(); i++ )
	{
        CL::Kline	& temp = at(i);
        if( temp.date == newElement.date)
		{
			at(i)=newElement;//替换
			return i;
		}
        if( temp.date > newElement.date)
		{
			insert(i+begin(),newElement);//插入
			return i;
		}
	}
	push_back( newElement);//追加
	return size()-1;
}

bool CKData::IsNewValue( int nIndex, bool bTopOrBottom, int nDays )
{
    if( nIndex < nDays-1 || nIndex < 0 || nIndex >= (int)size() || nDays < 3 )
		return false;

	bool	bFirst	=	true;
    float	fLast	=	0;
    int		nLast	=	0;
    float	fNow	=	MaindataAt( nIndex );

    for( int k=nIndex-1; k>=0 && k>nIndex-nDays; k-- )
	{
        float	fCur	=	MaindataAt(k);
		if( bFirst )
		{
			fLast	=	fCur;
			nLast	=	k;
			bFirst	=	false;
		}

		if( bTopOrBottom )
		{
			if( fCur > fNow )
				return false;

			if( fCur > fLast )
			{
				fLast	=	fCur;
				nLast	=	k;
			}
		}
		else
		{
			if( fCur < fNow )
				return false;

			if( fCur < fLast )
			{
				fLast	=	fCur;
				nLast	=	k;
			}
		}
	}
	if( bFirst )
		return false;

	if( nLast >= nIndex-30 && nLast <= nIndex-5 )
		return true;

	return false;
}

bool CKData::GetMinMaxInfo( int nStart, int nEnd, float * pfMin, float *pfMax )
{
    if( nStart < 0 || nEnd < 0 || nStart > nEnd || nEnd >= (int)size() )
		return false;

    float	fMin = 0, fMax = 0;
	bool	bFirst	=	true;
    for( int k=nStart; k<=nEnd; ++k )
	{
        CL::Kline	& kd	=	at(k);
		if( bFirst )
		{
			fMin	=	kd.low;
			fMax	=	kd.high;
			bFirst	=	false;
		}
		if( kd.low < fMin )
			fMin	=	kd.low;
		if( kd.high > fMax )
			fMax	=	kd.high;
	}

	if( pfMin )		*pfMin	=	fMin;
	if( pfMax )		*pfMax	=	fMax;
	return !bFirst;
}

#define ktypeMin5  2
#define ktypeMin15 3
#define ktypeMin30 4
#define ktypeMin60 5

void CKData::SetKType( int ktype ) {	m_nKType = ktype;	}
void CKData::AutoSetKType( )
{
	if( size() >= 4 )
	{
        long elapse1 = at(1).date - at(0).date;
        long elapse2 = at(2).date - at(1).date;
        long elapse3 = at(3).date - at(2).date;
		long elapse = min(elapse1,elapse2);
		elapse = min(elapse,elapse3);
		if( elapse < 600 )
			m_nKType = ktypeMin5;
		else if( elapse < 1200 )
			m_nKType = ktypeMin15;
		else if( elapse < 2400 )
			m_nKType = ktypeMin30;
		else if( elapse < 7200 )
			m_nKType = ktypeMin60;
		else if( elapse < 172800 )
			m_nKType = ktypeDay;
		else if( elapse < 864000 )
			m_nKType = ktypeWeek;
		else if( elapse < 4320000 )
			m_nKType = ktypeWeek;
	}
}
int CKData::GetKType( ) { return m_nKType; }
int CKData::GetCurFormat( ) { return m_nCurFormat; }
void CKData::SetMaindataType( int type ) { m_nCurMaindataType = type; }
int CKData::GetMaindataType( ) { return m_nCurMaindataType; }


void CKData::ChangeCurFormat( int format, time_t dateAutoDRBegin, float dAutoDRLimit )
{
//	assert( GetKType() != ktypeMonth && GetKType() != ktypeWeek );

	if( m_nCurFormat == format )
		return;

	switch( format )
	{
	case formatOriginal:
		LoadDataOriginal( );
		m_nCurFormat = format;
		break;
	case formatXDRup:
		StoreDataOriginal( );
		ConvertXDR( true, dateAutoDRBegin, dAutoDRLimit );
		m_nCurFormat = format;
		break;
	case formatXDRdown:
		StoreDataOriginal( );
		ConvertXDR( false, dateAutoDRBegin, dAutoDRLimit );
		m_nCurFormat = format;
		break;
	default:;
//		assert( false );
	}
}

void CKData::Clear( )
{
	if( m_pDataOriginal )
	{
		delete [] (char*)m_pDataOriginal;
        m_pDataOriginal	=	nullptr;
	}
// 	if( m_pData )
// 	{
// 		delete [] (BYTE*)m_pData;
// 		m_pData	=	NULL;
// 	}
	clear();
    shrink_to_fit();
    std::vector<CL::Kline>().swap(*this);
	//m_nKType	=	ktypeDay;
	m_nCurFormat=	formatOriginal;
	m_nCurMaindataType	=	mdtypeClose;
    m_pDataOriginal	=	nullptr;
	m_nSizeOriginal = m_nMaxSizeOriginal = 0;
	//m_pData	=	NULL;
	//m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

int CKData::CompareLatestDate( CKData &kd )
{
	// compare this kdata with kd's latest time
	int	nYearThis, nMonthThis, nDayThis, nHourThis, nMinuteThis;
	int nYear, nMonth, nDay, nHour, nMinute;
	LatestDate( nYearThis, nMonthThis, nDayThis, nHourThis, nMinuteThis );
	kd.LatestDate( nYear, nMonth, nDay, nHour, nMinute );

	if( nYearThis > nYear || nMonthThis > nMonth || nDayThis > nDay
		|| nHourThis > nHour || nMinuteThis > nMinute )
		return 1;
	if( nYearThis < nYear || nMonthThis < nMonth || nDayThis < nDay
		|| nHourThis < nHour || nMinuteThis < nMinute )
		return -1;
	return 0;
}

int CKData::Min5ToMin15( CKData &kdm5, CKData &kdm15 )
{
//	assert( ktypeMin5 == kdm5.GetKType() );
//	assert( ktypeMin15 == kdm15.GetKType() );
	return ConvertKData( kdm5, kdm15, 3 );
}

int CKData::Min5ToMin30( CKData &kdm5, CKData &kdm30 )
{
//	assert( ktypeMin5 == kdm5.GetKType() );
//	assert( ktypeMin30 == kdm30.GetKType() );
	return ConvertKData( kdm5, kdm30, 6 );
}


int CKData::Min1ToDay( CKData& kdSrc,CKData& kdday )
{
	if (kdSrc.size()<=0)
	{
		return 0;
	}
	int	nCount		=	0;

    CL::Kline dataDest = {0, 0,0,0,0,0,0,0, 0,0,{{0,0}}};

	size_t nStart=0;
	for( size_t pos=nStart; pos<kdSrc.size(); pos++ )
	{
         CL::Kline & dataSrc = kdSrc.at( pos );
        if (kdday.empty() && dataDest.date!=0)
		{
            memcpy(&dataDest,&dataSrc,sizeof(	CL::Kline));
			//dataDest.TradingTime = boost::posix_time::to_time_t(boost::posix_time::from_time_t(dataSrc.TradingTime).date());
			//kdday.push_back(dataDest);
		}
		else
		{
			//直接看交易日;
            auto tmSrc = dataSrc.date;
            auto tmDest = dataDest.date;
			if (tmSrc!=tmDest)
			{
				//不在同一天;
				kdday.push_back(dataDest);
				memcpy( &dataDest, &dataSrc, sizeof(dataDest) );
			}
			else
			{
				//更新;
				if( dataDest.high < dataSrc.high )	
					dataDest.high=	dataSrc.high;
				if( dataDest.low > dataSrc.low)		
					dataDest.low		=	dataSrc.low;
				dataDest.amount		+=	dataSrc.amount;
                dataDest.vol		+=	dataSrc.vol;
                dataDest.close		=	dataSrc.close;
			}
		}
	}
	//最后一个K永远不会被前的循环加到数据里来，所以手动把它放到这里面来;
	kdday.push_back(dataDest);
	return kdday.size();
}


int CKData::Min5ToMin60( CKData &kdm5, CKData &kdm60 )
{
//	assert( ktypeMin5 == kdm5.GetKType() );
//	assert( ktypeMin60 == kdm60.GetKType() );
	return ConvertKData( kdm5, kdm60, 12 );
}
/**
 * \brief 日线转月线
 *
 */
int CKData::DayToMonth( CKData &kdday, CKData &kdmonth )
{
	// convert day k line to month k line
	//assert( ktypeDay == kdday.GetKType() );
	//assert( ktypeMonth == kdmonth.GetKType() );

	kdmonth.reserve(  kdday.size() / 20 + 5 );
	
	/*int nStart		=	kdday.size() % 3;
	KDATA	dataDest;
	memset( &dataDest, 0, sizeof(dataDest) );
	int	nYearCur = 0, nMonthCur = 0 ;
	for( size_t pos=nStart; pos<kdday.size(); pos++ )
	{
		KDATA & dataSrc = kdday.at( pos );

		boost::posix_time::ptime	tm;
		if( !FromInstrumentTimeDayOrMin(tm,std::stoi(dataSrc.TradingDay),true) )
			continue;
		int nYear	=	tm.date().year();
		int nMonth = tm.date().month();

		if( nYear != nYearCur || nMonth != nMonthCur )	// a new month
		{
			if( 0 != pos )
				kdmonth.push_back(dataDest);				// add a month

			memcpy( &dataDest, &dataSrc, sizeof(dataDest) );	// begin a new month
			nYearCur	=	nYear;
			nMonthCur	=	nMonth;
		}
		else
		{
			dataDest.Turnover		+=	dataSrc.Turnover;
			if( dataDest.HighestPrice < dataSrc.HighestPrice )	dataDest.HighestPrice	=	dataSrc.HighestPrice;
			if( dataDest.LowestPrice > dataSrc.LowestPrice )		dataDest.LowestPrice		=	dataSrc.LowestPrice;
			dataDest.Volume		+=	dataSrc.Volume;
			dataDest.ClosePrice		=	dataSrc.ClosePrice;
		}

		if( pos == kdday.size()-1 )	// the latest one
			kdmonth.push_back(dataDest);
 	}*/

	return kdmonth.size();
}

int CKData::DayToWeek( CKData &kdday, CKData &kdweek )
{
	// convert day k line to week k line
//	assert( ktypeDay == kdday.GetKType() );
//	assert( ktypeWeek == kdweek.GetKType() );
    int nKdCount=(int)kdday.size();
    kdweek.reserve(nKdCount / 5 + 5);
	
    int nStart		=	nKdCount % 3;
	/*KDATA	dataDest;
	memset( &dataDest, 0, sizeof(dataDest) );
	for( size_t pos=nStart; pos<kdday.size(); pos++ )
	{
		Kline & dataSrc = kdday.at( pos );

		boost::posix_time::ptime	tm;
		if( !FromInstrumentTimeDayOrMin(tm,dataSrc.TradingDate,true) )
			continue;

		if (tm.date().day_of_week().as_enum() == boost::date_time::Tuesday)	// a new week
		{
			if( 0 != pos )
				kdweek.push_back(dataDest);				// add a week

			memcpy( &dataDest, &dataSrc, sizeof(dataDest) );	// begin a new week
		}
		else
		{
			dataDest.Turnover		+=	dataSrc.Turnover;
			if( dataDest.HighestPrice < dataSrc.HighestPrice )	dataDest.HighestPrice	=	dataSrc.HighestPrice;
			if( dataDest.LowestPrice > dataSrc.LowestPrice )		dataDest.LowestPrice		=	dataSrc.LowestPrice;
			dataDest.Volume		+=	dataSrc.Volume;
			dataDest.ClosePrice		=	dataSrc.ClosePrice;
		}

		if( pos == kdday.size()-1 )	// the latest one
			kdweek.push_back(dataDest);
	}*/

	return kdweek.size();
}

time_t CKData::ToDayDate( time_t date )
{
	switch( GetKType() )
	{
	case ktypeMin60:
	case ktypeMin30:
	case ktypeMin15:
	case ktypeMin5:
		return (date / 10000 + 1990 * 10000);
	default:
		return date;
	}
}

int CKData::MergeKData( CKData * pother )
{
 	if( !pother || pother->size() == 0 )
		return 0;
	if( GetKType() != pother->GetKType() )
		return 0;

	if( size() == 0 )
	{
		//CopyData( *pother );
		*this = *pother;
		return size();
	}

	int	nCount		= 0;
	//resize( size(), pother->size()+1 );
	for( size_t i=0; i<pother->size(); i++ )
	{
        CL::Kline	kdnew	=	pother->at(i);
		if( kdnew.close < 1e-4 || kdnew.open < 1e-4 || kdnew.high< 1e-4 || kdnew.low < 1e-4 )
			continue;

		size_t j;
		for( j=0; j<size(); j++ )
		{
			if (IsDayOrMin(GetKType()))
			{
                if( kdnew.date ==at(j).date )
				{
					at(j)=kdnew;
					break;
				}
                if( kdnew.date < at(j).date )
				{
					insert(j+begin(),kdnew);
					break;
				}
			}
			else
			{
                if( kdnew.date ==at(j).date )
				{
					at(j) = kdnew;
					break;
				}
                if( kdnew.date < at(j).date )
				{
					insert(j + begin(), kdnew);
					break;
				}
			}
		}
		if( size() == j )
			push_back( kdnew );
		nCount	++;
	}

	return nCount;

/*	The Old Version
	if( !pother || pother->GetSize() == 0 )
		return GetSize();
	if( GetKType() != pother->GetKType() )
		return GetSize();

	if( GetSize() == 0 )
	{
		CopyData( *pother );
		return GetSize();
	}

	int nLen = GetSize();
	int nLenOther = pother->GetSize();

	assert( 0 != nLen && 0 != nLenOther );

	if( ElementAt(nLen-1).m_date < pother->ElementAt(0).m_date )
		CopyData( *pother );
//	else if( ElementAt(0).m_date > pother->ElementAt(nLenOther-1).m_date )
//		;
//	else if( ElementAt(0).m_date <= pother->ElementAt(0).m_date
//		&& ElementAt(nLen-1).m_date >= pother->ElementAt(nLenOther-1).m_date )
//		;
	else if( ElementAt(0).m_date > pother->ElementAt(0).m_date
		&& ElementAt(nLen-1).m_date < pother->ElementAt(nLenOther-1).m_date )
		CopyData( *pother );
	else if( ElementAt(0).m_date <= pother->ElementAt(0).m_date
		&& ElementAt(nLen-1).m_date < pother->ElementAt(nLenOther-1).m_date )
	{
		// append from pother
		DWORD	date	=	ElementAt(nLen-1).m_date;
		SetSize( GetSize(), pother->GetSize() );
		for( int i=0; i<pother->GetSize(); i++ )
		{
			KDATA	& kd	=	pother->ElementAt(i);
			if( kd.m_date > m_data )
				Add( kd );
		}
	}
	else if( ElementAt(0).m_date >= pother->ElementAt(0).m_date
		&& ElementAt(nLen-1).m_date > pother->ElementAt(nLenOther-1).m_date )
	{
		// insert from pother
		kdata_container	temp	=	(*pother);
		DWORD	date	=	pother->ElementAt(nLenOther-1).m_date;
		temp.SetSize( temp.GetSize(), GetSize()+5 );
		for( int i=0; i<GetSize(); i++ )
		{
			KDATA	& kd	=	ElementAt(i);
			if( kd.m_date > date )
				temp.Add( kd );
		}
		CopyData( temp );
	}
	
	return GetSize();
*/
}

int CKData::FullFillKData( CKData & kdataMain, bool bFillToEnd )
{
//	assert( GetKType() == kdataMain.GetKType() );
	if( GetKType() != kdataMain.GetKType() )
		return 0;
    int nKdCount=(int)kdataMain.size();
    int selfCount=(int)size();
    if( selfCount == 0 || nKdCount == 0 )
		return 0;

    time_t	dateBegin	=	at(0).date;
    time_t	dateMainEnd5	=	(nKdCount >= 5 ? kdataMain.at(nKdCount-5).date : 0);
    time_t	dateEnd5		=	(selfCount>= 5 ? at(selfCount-5).date : 0);
    int		iMain = 0, iSelf = 0;
    for( iMain=0; iMain<nKdCount; iMain ++ )
	{
        if( dateBegin == kdataMain.at(iMain).date )
			break;
	}

    resize(nKdCount- iMain - selfCount> 0 ?nKdCount- iMain - selfCount : selfCount);
	int	nCount	=	0;
    for( ; iMain <= nKdCount && iSelf <= selfCount; iMain++, iSelf++ )
	{
        if( !bFillToEnd && iSelf == selfCount && at(iSelf-1).date< dateMainEnd5 )
			break;
        if( !bFillToEnd && iMain == nKdCount && kdataMain.at(iMain-1).date< dateEnd5 )
			break;
        while( iMain > 0 && iMain <=nKdCount&& iSelf < selfCount
                && ( iMain == nKdCount|| kdataMain.at(iMain).date > at(iSelf).date ) )
		{
//			KDATA	kd;
//			memset( &kd, 0, sizeof(kd) );
//			kd.m_date	=	ElementAt(iSelf).m_date;
//			kd.open	=	kd.high	=	kd.low	=	kd.close	=	kdataMain.ElementAt(iMain-1).close;
//			kdataMain.InsertAt( iMain, kd );
//          nKdCount=(int)kdataMain.size();
//			iMain	++;
			iSelf	++;
//			nCount	++;
		}

        while( iMain < nKdCount && iSelf <= selfCount && iSelf > 0
                && ( iSelf == selfCount || kdataMain.at(iMain).date < at(iSelf).date ) )
		{
            CL::Kline	kd;
			memset( &kd, 0, sizeof(kd) );
            kd.date	=	kdataMain.at(iMain).date;
			kd.open	=	kd.high	=	kd.low	=	kd.close	=	at(iSelf-1).close;
			insert( iSelf+begin(), kd );
            selfCount=(int)size();
			iMain	++;
			iSelf	++;
			nCount	++;
		}
	}
	return 0;
}

bool CKData::IsAdjacentDays( int nIndex, int nDays )
{
	// check parameters
//	assert( nIndex >= 0 && nIndex < GetSize() && nDays >= 1 );
    if( nIndex < 0 || nIndex >= (int)size() || nDays < 1 )
		return false;

	// data not enougy
    if( nDays > nIndex )
		return false;

	//boost::posix_time::ptime	sptime1, sptime2;
	//FromInstrumentTimeDayOrMin(sptime1, at(nIndex - nDays).TradingDate, CKData::IsDayOrMin(m_nKType));
	//FromInstrumentTimeDayOrMin(sptime2, at(nIndex).TradingDate, CKData::IsDayOrMin(m_nKType));

	if( ktypeMonth == m_nKType )
	{
		//if( sptime2.date() - sptime1.date() >= boost::gregorian::date_duration(nDays+63) )
			return false;
	}
	else if( ktypeWeek == m_nKType )
	{
		//if (sptime2.date() - sptime1.date() >= boost::gregorian::date_duration(nDays + 15))
			return false;
	}
	else
	{
		//if (sptime2.date() - sptime1.date() >= boost::gregorian::date_duration(nDays + 8))
			return false;
	}
	
	return true;
}

bool CKData::GetDiff( float * pValue, time_t dateCur, int nDays )
{
//	assert( pValue && nDays > 0 );

	// Find date Current to calculate from
	int		nIndex	=	GetIndexByDate( dateCur );
	if( -1 == nIndex )
		return false;

	// data not enough
	if( nDays > nIndex )
		return false;

	// 检查是否是相邻成交日
	if( !IsAdjacentDays( nIndex, nDays ) )
		return false;

	if( at(nIndex-nDays).close < 1e-4 )
		return false;
	if( pValue )
		*pValue	=	at(nIndex).close - at(nIndex-nDays).close;
	return true;
}

bool CKData::GetDiffPercent( float * pValue, time_t dateCur, int nDays )
{
//	assert( pValue && nDays > 0 );

	// Find date Current to calculate from
	int		nIndex	=	GetIndexByDate( dateCur );
	if( -1 == nIndex )
		return false;

	// data not enough
	if( nDays > nIndex )
		return false;

	// 检查是否是相邻成交日
	if( !IsAdjacentDays( nIndex, nDays ) )
		return false;

	if( at(nIndex-nDays).amount < 1e-4 )
		return false;
	if( pValue )
		*pValue	=	(100. * at(nIndex).close) / at(nIndex-nDays).close - 100;
	return true;
}

bool CKData::GetScope( float * pValue, time_t dateCur, int nDays )
{
//assert( pValue && nDays > 0 );

	// Find date Current to calculate from
	int		nIndex	=	GetIndexByDate( dateCur );
	if( -1 == nIndex )
		return false;

	// data not enough
	if( nDays > nIndex )
		return false;

	// 检查是否是相邻成交日
	if( !IsAdjacentDays( nIndex, nDays ) )
		return false;

	double	dMax = 0, dMin = 0;
	int	nCount	=	0;
	for( int k=nIndex; k>=0; k-- )
	{
		if( nIndex == k )
		{
			dMin	=	at(k).low;
			dMax	=	at(k).high;
		}
		if( dMin > at(k).low )	dMin	=	at(k).low;
		if( dMax < at(k).high )	dMax	=	at(k).high;

		nCount	++;
		if( nCount == nDays )
			break;
	}
	// data not enough
	if( nCount != nDays || nCount <= 0 )
		return false;

	if( at(nIndex-nDays).close <= 0 )
		return false;
	if( pValue )
		*pValue	=	( 100. * (dMax-dMin) ) / at(nIndex-nDays).close ;
	return true;
}

bool CKData::GetVolumeSum( float * pValue, time_t dateCur, int nDays )
{
//	assert( pValue && nDays > 0 );

	// Find date Current to calculate from
	int		nIndex	=	GetIndexByDate( dateCur );
	if( -1 == nIndex )
		return false;

	// data not enough
	if( nDays > nIndex )
		return false;

	// 检查是否是相邻成交日
	if( !IsAdjacentDays( nIndex, nDays ) )
		return false;

	// begin calculate
    float	dAll	=	0;
	int		nCount	=	0;
	for( int i=nIndex; i>=0; i-- )
	{
        dAll	+=	at(i).vol;
		nCount	++;
		if( nCount >= nDays )
			break;
	}
	// data not enough
	if( nCount != nDays || nCount <= 0 )
		return false;

	if( pValue )
		*pValue	=	dAll;
	return true;
}

bool CKData::GetRatioVolume( float * pValue,time_t dateCur, int nDays )
{
//	assert( pValue && nDays > 0 );

	// Find date Current to calculate from
	int		nIndex	=	GetIndexByDate( dateCur );
	if( -1 == nIndex )
		return false;

	// data not enough
	if( nDays > nIndex )
		return false;

	// 检查是否是相邻成交日
	if( !IsAdjacentDays( nIndex, nDays ) )
		return false;

	// begin calculate
    float	dAll	=	0;
	int		nCount	=	0;
	for( int i=nIndex-1; i>=0; i-- )
	{
        dAll	+=	at(i).vol;
		nCount	++;
		if( nCount >= nDays )
			break;
	}
	// data not enough
	if( nCount != nDays || nCount <= 0 )
		return false;

	if( fabs(dAll) < 1 )
		return false;

	if( pValue )
		*pValue	=	(at(nIndex).vol / dAll ) * nCount;
	return true;
}

bool CKData::GetRS( float * pValue, time_t dateCur, int nDays )
{
//	assert( pValue && nDays > 0 );

	// Find date Current to calculate from
	int		nIndex	=	GetIndexByDate( dateCur );
	if( -1 == nIndex )
		return false;

	// data not enough
	if( nDays > nIndex )
		return false;

	// 检查是否是相邻成交日
	if( !IsAdjacentDays( nIndex, nDays ) )
		return false;

	int	nCount	=	0;
    float	dA = 0, dB = 0;
	for( int k=nIndex; k>=1; k-- )
	{
		if( at(k).close > at(k-1).close )
			dA	+=	(at(k).close - at(k-1).close);
		else
			dB	+=	(at(k-1).close - at(k).close);
		
		nCount	++;
		if( nCount == nDays )
		{
            float	dResult;
			if( fabs(dB) < 1e-4 )
				dResult	=	100;
			else
				dResult	=	dA / dB;
			if( pValue )
				*pValue	=	dResult;
			return true;
		}
	}
	return false;
}

bool CKData::GetMA( float * pValue, int nIndex, int nDays )
{
    if( nIndex < 0 || nIndex >=(int)size() || nDays <= 0 )
		return false;

	int	nCount	=	0;
    if( nDays > (int)(nIndex+1 ))
		return false;
    float	dResult	=	0;
	for( int k=nIndex; k>=0; k-- )
	{
		dResult	+=	MaindataAt(k);
		nCount	++;
		if( nCount == nDays )
		{
			if( pValue )
				*pValue	=	dResult / nDays;
			return true;
		}
	}
	return false;
}

bool CKData::GetMA(float & pValue, int nIndex, int nDays)
{
	return GetMA(&pValue,nIndex,nDays);
}

float  CKData::GetMA(int nIndex, int nDays)
{
	float MA = 0;
	GetMA(&MA, nIndex, nDays);
	return MA;
}

/////////////////////////////////////////////////////////////////////////////
// Private Operations
int CKData::ConvertKData( CKData &kdSrc, CKData &kdDest, int multiple )
{
	// convert kdSrc k line to kdDest k line accordding to their multiple
//	assert( multiple > 1 );
	if( multiple < 2 )	return 0;

	kdDest.reserve( /*0,*/ kdSrc.size() / multiple + 5 );
	
	int nStart		=	kdSrc.size() % multiple;
	int	nCount		=	0;
    CL::Kline	dataDest;
	for( size_t pos=nStart; pos<kdSrc.size(); pos++ )
	{
        CL::Kline & dataSrc = kdSrc.at( pos );

		nCount	++;
		if( 1 == nCount )
		{
			memcpy( &dataDest, &dataSrc, sizeof(dataDest) );
		}
		else
		{
			dataDest.amount		+=	dataSrc.amount;
			if( dataDest.high < dataSrc.high)	
				dataDest.high	=	dataSrc.high;
			if( dataDest.low > dataSrc.low)		
				dataDest.low		=	dataSrc.low;
			dataDest.vol		+=	dataSrc.vol;
			dataDest.close		=	dataSrc.close;
		}
		
		if( multiple == nCount )	// new dataDest is over
		{
			nCount	=	0;
			kdDest.push_back(dataDest);
		}
	}
	
	return kdDest.size();
}

void CKData::LoadDataOriginal( )
{
	if( m_pDataOriginal )
	{
// 		if( m_pData )
// 			delete [] (BYTE*)m_pData;
// 
// 		m_pData	=	m_pDataOriginal;
// 		m_nSize	=	m_nSizeOriginal;
// 		m_nMaxSize	=	m_nMaxSizeOriginal;

        m_pDataOriginal	=	nullptr;
		m_nSizeOriginal = m_nMaxSizeOriginal = 0;
	}
}

void CKData::StoreDataOriginal( )
{
    if( nullptr == m_pDataOriginal )
	{
// 		m_pDataOriginal		=	m_pData;
// 		m_nSizeOriginal		=	m_nSize;
// 		m_nMaxSizeOriginal	=	m_nMaxSize;

//		m_pData	=	NULL;
//		m_nSize = m_nMaxSize = 0;
	}
}
/*
float kdata_container::GetRatio( float fLastClose, DRDATA & dr )
{
	if( fLastClose < 1e-4 )
		return	1.0000;

	float	fRatio	=	1.0000;
	if( dr.m_fProfit > 1e-4 )
	{
		fRatio	=	fRatio * ( 1 - dr.m_fProfit/fLastClose );
	}
	if( dr.m_fGive > 1e-4 )
	{
		fRatio	=	(float)( fRatio * 1./(1.+dr.m_fGive) );
	}
	if( dr.m_fPei > 1e-4 )
	{
		float	priceNow = (float)( (dr.m_fPeiPrice * dr.m_fPei + fLastClose)/(1.+dr.m_fPei) );
		fRatio	=	fRatio * priceNow / fLastClose;
	}
	return fRatio;
}
*/
void CKData::ConvertXDR( bool bUP, time_t dateAutoDRBegin, float dAutoDRLimit )
{
	/*
	CDRData	drtemp;
	drtemp	=	m_drdata;
	drtemp.Sort( );

	if( m_pData )
	{
		delete [] (BYTE*)m_pData;
		m_nSize	=	m_nMaxSize	=	0;
		m_pData	=	NULL;
	}

	if( NULL == m_pDataOriginal || 0 == m_nSizeOriginal )
		return;
	*/
/*	if( drtemp.GetSize() == 0 )
	{
		SetSize( m_nSizeOriginal );
		if( m_pData )
			memcpy( m_pData, m_pDataOriginal, sizeof(KDATA)*m_nSize );
		return;
	}
*/
	/*
	dAutoDRLimit	=	dAutoDRLimit / 100;

	if( bUP )
	{
		SetSize( 0, m_nSizeOriginal );
		int	drPos	=	0;
		float	fRatio	=	1.000000;
		for( int i=0; i<m_nSizeOriginal; i++ )
		{
			KDATA	& kd = m_pDataOriginal[i];
			KDATA	newkd	=	kd;
			
			if( drPos < drtemp.GetSize() && ToDayDate(kd.m_date) >= drtemp.ElementAt(drPos).m_date )
			{
				if( i > 0 )
				{
					KDATA	kdLast	=	m_pDataOriginal[i-1];
					fRatio	=	fRatio * GetRatio( kdLast.m_fClose, drtemp.ElementAt(drPos) );
				}
				
				drPos	++;
			}
			else if( ToDayDate(kd.m_date) >= dateAutoDRBegin && i > 0 )	//	Auto XDR
			{
				KDATA	kdLast	=	m_pDataOriginal[i-1];
				if( kdLast.m_fClose > 1e-4 && kd.m_fOpen < kdLast.m_fClose
					&& fabs(kd.m_fOpen/kdLast.m_fClose-1) > dAutoDRLimit )
					fRatio	=	fRatio * kd.m_fOpen / kdLast.m_fClose;
			}

			newkd.m_fOpen	=	(kd.m_fOpen / fRatio);
			newkd.m_fHigh	=	(kd.m_fHigh / fRatio);
			newkd.m_fLow	=	(kd.m_fLow / fRatio);
			newkd.m_fClose	=	(kd.m_fClose / fRatio);
			newkd.m_fVolume	=	(kd.m_fVolume * fRatio);
			Add( newkd );
		}
	}
	else
	{
		SetSize( m_nSizeOriginal );
		int	drPos	=	drtemp.GetSize()-1;
		float	fRatio	=	1.000000;
		for( int i=m_nSizeOriginal-1; i>=0; i-- )
		{
			KDATA	& kd = m_pDataOriginal[i];
			KDATA	newkd	=	kd;
			
			if( drPos >= 0 && ToDayDate(kd.m_date) < drtemp.ElementAt(drPos).m_date )
			{
				if( i < m_nSizeOriginal-1 )
					fRatio	=	fRatio * GetRatio( kd.m_fClose, drtemp.ElementAt(drPos) );
				
				drPos	--;
			}
			else if( ToDayDate(kd.m_date) >= dateAutoDRBegin && i+1 < m_nSizeOriginal )	//	Auto XDR
			{
				KDATA	kdNext	=	m_pDataOriginal[i+1];
				if( kdNext.m_fOpen > 1e-4 && kdNext.m_fOpen < kd.m_fClose
					&& fabs(kdNext.m_fOpen/kd.m_fClose-1) > dAutoDRLimit )
					fRatio	=	fRatio * kdNext.m_fOpen / kd.m_fClose;
			}

			newkd.m_fOpen	=	(kd.m_fOpen * fRatio);
			newkd.m_fHigh	=	(kd.m_fHigh * fRatio);
			newkd.m_fLow	=	(kd.m_fLow * fRatio);
			newkd.m_fClose	=	(kd.m_fClose * fRatio);
			newkd.m_fVolume	=	(kd.m_fVolume / fRatio);
			SetAt( i, newkd );
		}
	}
	*/
}

int CKData::GetUpperBound() const
{ 
	return size()-1; 
}

// void kdata_container::SetAt(int nIndex, KDATA newElement)
// { 
// 	//assert(nIndex >= 0 && nIndex < m_nSize);
// 	m_pData[nIndex] = newElement; 
// }


bool CKData::DateAt(int nIndex,
								  int &nYear, int &nMonth, int &nDay, int &nHour, int &nMinute ) const
{
//	assert(nIndex >= 0 && nIndex < m_nSize);
    if( nIndex < 0 || nIndex >= (int)size() )
		return 0;
	/*time_t	date	=	(*this)[nIndex].day;
	//boost::posix_time::ptime	sptime;
	bool	bOK	=	false;
	if( ktypeMonth == m_nKType 
		|| ktypeWeek == m_nKType 
		|| ktypeDay == m_nKType )
	{
		bOK = FromInstrumentTimeDayOrMin(sptime,date,true);
	}
	else if( ktypeMin60 == m_nKType
		|| ktypeMin30 == m_nKType
		|| ktypeMin15 == m_nKType
		|| ktypeMin5 == m_nKType )
	{
		bOK = FromInstrumentTimeDayOrMin(sptime, date, false);
	}
	else
	{
		nYear	=	nMonth	=	nDay	=	nHour	=	nMinute	=	0;
		return false;
	}
	if( !bOK )
		return false;

	nYear	=	sptime.date().year();
	nMonth = sptime.date().month();
	nDay	=	sptime.date().day();
	nHour	=	sptime.time_of_day().hours();
	nMinute	=	sptime.time_of_day().minutes();
*/
	return true;
}

float	CKData::MaindataAt(int nIndex) const
// 得到kdata_container的nIndex日的主数据，根据主数据类型不同，返回值可能是开盘价、收盘价或者平均价;
{
//	assert(nIndex >= 0 && nIndex < m_nSize);
    if( nIndex < 0 || nIndex >= (int)size() )
		return 0;
	if( mdtypeOpen == m_nCurMaindataType )
		return (*this)[nIndex].open;
	else if( mdtypeAverage == m_nCurMaindataType
		&& (*this)[nIndex].vol > 1e-4 && (*this)[nIndex].amount > 1e-4)
	{
		int		nCount	=	0;
		float	average = ((float)((*this)[nIndex].amount)) / (*this)[nIndex].vol;
		while (average < (*this)[nIndex].low && nCount < 10)	{ average *= 10;	nCount++; }
		while (average >(*this)[nIndex].high && nCount < 20)	{ average /= 10;	nCount++; }
		if (average < (*this)[nIndex].low)		//	说明是指数;
			average = ((*this)[nIndex].open + (*this)[nIndex].high + (*this)[nIndex].low + (*this)[nIndex].close) / 4;
		return average;
	}
	else if (mdtypeHigh ==  m_nCurMaindataType)
	{
        return (*this)[nIndex].high;
	}
	else if (mdtypeLow ==  m_nCurMaindataType)
	{
		return (*this)[nIndex].low;
	}
	else if (mdtypeTrunover==m_nCurMaindataType)
	{
		return (*this)[nIndex].amount;
	}
	else if (mdtypeVolume ==  m_nCurMaindataType)
	{
        return (*this)[nIndex].vol;
	}
	else if (mdtypeOpenInterest == m_nCurMaindataType)
	{
		return (*this)[nIndex].open;
	}
	else
	{
		return (*this)[nIndex].close;
	}

	// WARNING CPV::Calculate( ... ) use the save code.
}
/*
bool CKData::update(const Tick* pReport)
{
	assert(pReport);
	if (!pReport)
		return false;
	if (!m_LastTickPtr)
	{
		m_LastTickPtr = boost::make_shared<Tick>(*pReport);
	}
	else
	{
		if (m_LastTickPtr->UpdateTime==pReport->UpdateTime && m_LastTickPtr->UpdateMillisec == pReport->UpdateMillisec)
		{
			//这个Tick已经更新过了;
			LOGDEBUG("duplicate update err kd:{} {}.{}",pReport->TradingDay,pReport->UpdateTimeStr, pReport->UpdateMillisec);
			return true;
		}
	}
	int dwType = GetKType();
	if (dwType <= 0)
	{
		return false;
	}

	double LastTurnover = m_LastTickPtr->Turnover;
	int64_t LastVolume = m_LastTickPtr->Volume;
	*m_LastTickPtr = *pReport;
	//换算成整点时间(以K线周期的整数倍);

	//看在不在一个周期内;
	time_t tTemp = (pReport->UpdateTime / dwType)*dwType + dwType;

	if (size() == 0)
	{
		KDATA kd;
		strcpy(kd.TradingDay, pReport->TradingDay);
		strcpy(kd.szCode, pReport->szCode);
		strcpy(kd.szExchange, pReport->szExchange);
		kd.OpenPrice = pReport->LastPrice;
		kd.HighestPrice = pReport->LastPrice;
		kd.LowestPrice = pReport->LastPrice;
		kd.ClosePrice = pReport->LastPrice;
		kd.Turnover = pReport->Turnover-LastTurnover;
		kd.Volume = pReport->Volume-LastVolume;
		kd.TradingTime = tTemp;
		kd.AveragePrice = pReport->AveragePrice;
		push_back(kd);
		return true;
	}
	KDATA kd=back();
	bool bNewKData = false;
	if (strcmp(kd.TradingDay, pReport->TradingDay) != 0)
	{
		//日期不一样了,夜盘开盘?;
		//InsertKDataSort(kd);
		kd.OpenPrice = pReport->LastPrice;
		kd.HighestPrice = pReport->LastPrice;
		kd.LowestPrice = pReport->LastPrice;
		kd.ClosePrice = pReport->LastPrice;

		kd.Turnover = pReport->Turnover;
		kd.Volume = pReport->Volume;

		strcpy(kd.TradingDay, pReport->TradingDay);
		kd.TradingTime = tTemp;
		push_back(kd);
		bNewKData = true;
	}
	else
	{
		//下一个周期开始;
		if (kd.TradingTime < tTemp) //这里有BUG,没处理过了0点时的状态
		{
			//新的周期;
			kd.OpenPrice = pReport->LastPrice;
			kd.HighestPrice = pReport->LastPrice;
			kd.LowestPrice = pReport->LastPrice;
			kd.ClosePrice = pReport->LastPrice;

			kd.Volume = (pReport->Volume - LastVolume);
			kd.Turnover = (pReport->Turnover - LastTurnover);

			kd.TradingTime = tTemp;
			push_back(kd);
			bNewKData = true;
		}
		else if (kd.TradingTime ==  tTemp)
		{
			kd.ClosePrice = pReport->LastPrice;
			//最高价与最低价的计算;
			if (pReport->LastPrice > kd.HighestPrice)
			{
				kd.HighestPrice = pReport->LastPrice;
			}
			if (pReport->LastPrice < kd.LowestPrice)
			{
				kd.LowestPrice = pReport->LastPrice;
			}
			kd.Turnover += (pReport->Turnover - kd.Turnover);
			kd.Volume += (pReport->Volume - kd.Volume);
			this->back()=kd;
		}
		else
		{
			//回溯K线;
			LOGDEBUG("kdata update err kd:{} {}->t:{}"
				, kd.szCode
				, boost::posix_time::to_iso_extended_string(boost::posix_time::from_time_t(kd.TradingTime))
				, boost::posix_time::to_iso_extended_string(boost::posix_time::from_time_t(tTemp)));
		}
	}
	
	return true;
}*/

bool CKData::update(const CL::Kline * pKData)
{
	if (size() == 0)
	{
        CL::Kline kd;
		//memcpy(&kd,pKData,sizeof(KDATA));
		//strcpy(kd.TradingDay, pKData->TradingDay);
		//strcpy(kd.szCode, pKData->szCode);
		//strcpy(kd.szExchange, pReport->szExchange);
        kd.open = pKData->open;
        kd.high = pKData->high;
		kd.low = pKData->low;
		kd.close = pKData->close;
		kd.amount = pKData->amount;
		kd.vol = pKData->vol;
        kd.date = pKData->date;
		//kd.AveragePrice = pKData->AveragePrice;
		push_back(kd);
		return true;
	}
    CL::Kline kd = back();
	bool bNewKData = false;
	//if (strcmp(kd.day, pKData->day) != 0)
    if (kd.date!= pKData->date)
	{
		//日期不一样了,夜盘开盘?;
		//strcpy(kd.TradingDay, pKData->TradingDay);
		//strcpy(kd.szCode, pKData->szCode);
		//strcpy(kd.szExchange, pReport->szExchange);
        kd.open = pKData->open;
        kd.high = pKData->high;
        kd.low = pKData->low;
        kd.close = pKData->close;
        kd.amount = pKData->amount;
        kd.vol = pKData->vol;
        kd.date = pKData->date;
	//	kd.AveragePrice = pKData->AveragePrice;

		push_back(kd);
		bNewKData = true;
	}
	else
	{
		//下一个周期开始;
        if (kd.date < pKData->date) //这里有BUG,没处理过了0点时的状态
		{
			//新的周期;
			//strcpy(kd.TradingDay, pKData->TradingDay);
			//strcpy(kd.szCode, pKData->szCode);
			//strcpy(kd.szExchange, pReport->szExchange);
            kd.open = pKData->open;
            kd.high = pKData->high;
            kd.low = pKData->low;
            kd.close = pKData->close;
            kd.amount = pKData->amount;
            kd.vol = pKData->vol;
            kd.date = pKData->date;
			//kd.AveragePrice = pKData->AveragePrice;
			push_back(kd);
			bNewKData = true;
		}
        else if (kd.date == pKData->date)
		{
			//strcpy(kd.TradingDay, pKData->TradingDay);
			//strcpy(kd.szCode, pKData->szCode);
			//strcpy(kd.szExchange, pReport->szExchange);
            kd.open = pKData->open;
            kd.high = pKData->high;
            kd.low = pKData->low;
            kd.close = pKData->close;
            kd.amount = pKData->amount;
            kd.vol = pKData->vol;
            kd.date = pKData->date;
			//kd.AveragePrice = pKData->AveragePrice;
			this->back() = kd;
		}
		else
		{
			//回溯K线;
			//LOGDEBUG("kdata update err kd:{} {}->t:{}"
			//	, kd.szCode
			//	, boost::posix_time::to_iso_extended_string(boost::posix_time::from_time_t(kd.TradingTime))
			//	, boost::posix_time::to_iso_extended_string(boost::posix_time::from_time_t(pKData->TradingTime)));
		}
	}
	return true;
}

/*
int CKData::ToReport(ReportContainer & report)
{
	report.clear();

	if (size()==0)
	{
		return 0;
	}

	report.resize(size());
	InstrumentInfo info;
	double price_tick = 0;
	if (InstrumentContainer::get_instance().GetInstrumentInfo((*this)[0].szCode, &info))
	{
		price_tick = info.PriceTick;
	}
	convert_KDATA_to_REPORT(&(*this)[0], &report[0]);

	report[0].AskPrice[0] = report[0].LastPrice + price_tick;
	report[0].BidPrice[0] = report[0].LastPrice - price_tick;
	report[0].OpenPrice = (*this)[0].OpenPrice;
	

	boost::posix_time::ptime prevTickTime = boost::posix_time::from_time_t(report[0].UpdateTime) - boost::posix_time::seconds(1);
	strcpy(report[0].UpdateTimeStr, boost::posix_time::to_simple_string(prevTickTime.time_of_day()).c_str());

	std::string TradingDay((*this)[0].TradingDay);
	for (size_t i = 1; i < size(); ++i)
	{
		convert_KDATA_to_REPORT(&(*this)[i],&report[i]);
		report[i].AskPrice[0] = report[0].LastPrice + price_tick;
		report[i].BidPrice[0] = report[0].LastPrice - price_tick;
		boost::posix_time::ptime prevTickTime = boost::posix_time::from_time_t(report[i].UpdateTime) - boost::posix_time::seconds(1);
		strcpy(report[i].UpdateTimeStr, boost::posix_time::to_simple_string(prevTickTime.time_of_day()).c_str());
		if (strcmp(report[i-1].TradingDay , report[i].TradingDay) == 0)
		{
			//交易日没变,则累加;
			report[i].Volume = report[i-1].Volume + (*this)[i].Volume;
			report[i].Turnover = report[i-1].Turnover + (*this)[i].Turnover;

			if (report[i].HighestPrice < report[i-1].HighestPrice)
			{
				report[i].HighestPrice = report[i - 1].HighestPrice;
			}

			if (report[i].LowestPrice > report[i - 1].LowestPrice)
			{
				report[i].LowestPrice = report[i - 1].LowestPrice;
			}

			report[i].OpenPrice = report[i - 1].OpenPrice;
		}
		else
		{
			report[i - 1].ClosePrice = (*this)[i].ClosePrice;
			report[i].OpenPrice = (*this)[i].OpenPrice;
			report[i].PreClosePrice = report[i - 1].ClosePrice;
			report[i].PreSettlementPrice = report[i - 1].ClosePrice;
		}
	}
	return report.size();
}*/
