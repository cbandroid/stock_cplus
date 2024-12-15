#include "Technique.h"
#include "KData.h"
#include "MA.h"
#include "KDJ.h"
#include "DPER.h"
#include "MACD.h"
#include "BOLL.h"
#include "Express.h"

using namespace std;

class KdataContainer;

//////////////////////////////////////////////////////////////////////
//	technical_indicator

TechnicalIndicator::TechnicalIndicator( )
{
	m_pKData		=	NULL;

	m_pdCache1		=	NULL;
	m_pdCache2		=	NULL;
	m_pdCache3		=	NULL;
	m_pdCache4		=	NULL;
	m_pdCache5		=	NULL;
	m_pbHasCache1	=	NULL;
	m_pbHasCache2	=	NULL;
	m_pbHasCache3	=	NULL;
	m_pbHasCache4	=	NULL;
	m_pbHasCache5	=	NULL;
	m_nLength		=	0;
	m_nIndexStart	=	-1;

	m_nLastIntensity		=	ITS_NOTHING;
	m_nLastIntensityCode	=	ITSC_NOTHING;
}

TechnicalIndicator::TechnicalIndicator( KdataContainer * pKData )
{
	m_pKData	=	pKData;

	m_pdCache1		=	NULL;
	m_pdCache2		=	NULL;
	m_pdCache3		=	NULL;
	m_pdCache4		=	NULL;
	m_pdCache5		=	NULL;
	m_pbHasCache1	=	NULL;
	m_pbHasCache2	=	NULL;
	m_pbHasCache3	=	NULL;
	m_pbHasCache4	=	NULL;
	m_pbHasCache5	=	NULL;
	m_nLength		=	0;
	m_nIndexStart	=	-1;

	m_nLastIntensity		=	ITS_NOTHING;
	m_nLastIntensityCode	=	ITSC_NOTHING;
}

TechnicalIndicator::~TechnicalIndicator()
{
	TechnicalIndicator::clear( );
}

void TechnicalIndicator::set_kdata( KdataContainer * pKData )
{
	m_pKData	=	pKData;
	TechnicalIndicator::clear( );
}

KdataContainer * TechnicalIndicator::get_kdata( )
{
	return m_pKData;
}

void TechnicalIndicator::clear( )
{
	if( m_pdCache1 )
		delete	[]	m_pdCache1;
	if( m_pdCache2 )
		delete	[]	m_pdCache2;
	if( m_pdCache3 )
		delete	[]	m_pdCache3;
	if( m_pdCache4 )
		delete	[]	m_pdCache4;
	if( m_pdCache5 )
		delete	[]	m_pdCache5;

	if( m_pbHasCache1 )
		delete	[]	m_pbHasCache1;
	if( m_pbHasCache2 )
		delete	[]	m_pbHasCache2;
	if( m_pbHasCache3 )
		delete	[]	m_pbHasCache3;
	if( m_pbHasCache4 )
		delete	[]	m_pbHasCache4;
	if( m_pbHasCache5 )
		delete	[]	m_pbHasCache5;

	m_pdCache1		=	NULL;
	m_pdCache2		=	NULL;
	m_pdCache3		=	NULL;
	m_pdCache4		=	NULL;
	m_pdCache5		=	NULL;
	m_pbHasCache1	=	NULL;
	m_pbHasCache2	=	NULL;
	m_pbHasCache3	=	NULL;
	m_pbHasCache4	=	NULL;
	m_pbHasCache5	=	NULL;
	m_nLength		=	0;
	m_nIndexStart	=	-1;

	m_nLastIntensity		=	ITS_NOTHING;
	m_nLastIntensityCode	=	ITSC_NOTHING;
}

void TechnicalIndicator::clear_last_intensity( )
{
	m_nLastIntensity		=	ITS_NOTHING;
	m_nLastIntensityCode	=	ITSC_NOTHING;
}

// 准备缓冲区;
bool TechnicalIndicator::prepare_cache( size_t nStart, size_t nEnd, bool bForceRecalculate )
{
	assert( m_pKData );
	if( NULL == m_pKData )
		return false;
    if( -1 == (int)nEnd )
		nEnd	=	(size_t)(m_pKData->size()-1);
    if( nStart < 0 || nEnd < nStart || nEnd >=(size_t)(m_pKData->size()) )
		return false;

	if( bForceRecalculate )
		clear( );

	return min_max_info( nStart, nEnd, NULL, NULL );
}

// 从缓冲区中读取计算好的数据;
bool TechnicalIndicator::load_from_cache( size_t nIndex, double * pValue1, double * pValue2 /*= NULL*/, double * pValue3 /*= NULL*/, double * pValue4 /*= NULL*/, double * pValue5 /*= NULL */ )
{
	if( m_nLength <= 0 || m_nIndexStart < 0
		|| nIndex < m_nIndexStart || nIndex-m_nIndexStart>=m_nLength )
		return false;

	bool	bOK	=	true;
	if( pValue1 && !( m_pdCache1 && m_pbHasCache1 && m_pbHasCache1[nIndex-m_nIndexStart] ) )
		bOK	=	false;
	if( pValue2 && !( m_pdCache2 && m_pbHasCache2 && m_pbHasCache2[nIndex-m_nIndexStart] ) )
		bOK	=	false;
	if( pValue3 && !( m_pdCache3 && m_pbHasCache3 && m_pbHasCache3[nIndex-m_nIndexStart] ) )
		bOK	=	false;
	if( pValue4 && !( m_pdCache4 && m_pbHasCache4 && m_pbHasCache4[nIndex-m_nIndexStart] ) )
		bOK	=	false;
	if( pValue5 && !( m_pdCache5 && m_pbHasCache5 && m_pbHasCache5[nIndex-m_nIndexStart] ) )
		bOK	=	false;

	if( !bOK )
		return false;

	if( pValue1 )
		*pValue1	=	m_pdCache1[nIndex-m_nIndexStart];
	if( pValue2 )
		*pValue2	=	m_pdCache2[nIndex-m_nIndexStart];
	if( pValue3 )
		*pValue3	=	m_pdCache3[nIndex-m_nIndexStart];
	if( pValue4 )
		*pValue4	=	m_pdCache4[nIndex-m_nIndexStart];
	if( pValue5 )
		*pValue5	=	m_pdCache5[nIndex-m_nIndexStart];
	return true;
}

// 将计算好的数据保存至缓冲区;
bool TechnicalIndicator::store_to_cache( size_t nIndex, double * pValue1, double * pValue2 /*= NULL*/, double * pValue3 /*= NULL*/, double * pValue4 /*= NULL*/, double * pValue5 /*= NULL */ )
{
	STT_ASSERT_CALCULATE1( m_pKData, nIndex );

	if( m_nLength <= 0 || m_nIndexStart < 0 )
	{
		m_nLength		=	( NULL!=m_pKData ? m_pKData->size() : 0 );
		if( m_nLength > 0 )
			m_nIndexStart	=	0;
	}
	if( m_nLength <= 0 || m_nIndexStart <  0 )
		return false;

	// Realocate
	if( pValue1 )
	{
		if( !m_pdCache1 )
			m_pdCache1		=	new double[m_nLength];
		if( !m_pbHasCache1 )
		{
			m_pbHasCache1	=	new bool[m_nLength];
			if( !m_pbHasCache1 )	return false;
			for( size_t i=0; i<m_nLength; i++ )
				m_pbHasCache1[i]	=	false;
		}
		if( NULL == m_pbHasCache1 || NULL == m_pdCache1 )
			return false;
	}
	if( pValue2 )
	{
		if( !m_pdCache2 )
			m_pdCache2		=	new double[m_nLength];
		if( !m_pbHasCache2 )
		{
			m_pbHasCache2	=	new bool[m_nLength];
			if( !m_pbHasCache2 )	return false;
			for( size_t i=0; i<m_nLength; i++ )
				m_pbHasCache2[i]	=	false;
		}
		if( NULL == m_pbHasCache2 || NULL == m_pdCache2 )
			return false;
	}
	if( pValue3 )
	{
		if( !m_pdCache3 )
			m_pdCache3		=	new double[m_nLength];
		if( !m_pbHasCache3 )
		{
			m_pbHasCache3	=	new bool[m_nLength];
			if( !m_pbHasCache3 )	return false;
			for( size_t i=0; i<m_nLength; i++ )
				m_pbHasCache3[i]	=	false;
		}
		if( NULL == m_pbHasCache3 || NULL == m_pdCache3 )
			return false;
	}
	if( pValue4 )
	{
		if( !m_pdCache4 )
			m_pdCache4		=	new double[m_nLength];
		if( !m_pbHasCache4 )
		{
			m_pbHasCache4	=	new bool[m_nLength];
			if( !m_pbHasCache4 )	return false;
			for( size_t i=0; i<m_nLength; i++ )
				m_pbHasCache4[i]	=	false;
		}
		if( NULL == m_pbHasCache4 || NULL == m_pdCache4 )
			return false;
	}
	if( pValue5 )
	{
		if( !m_pdCache5 )
			m_pdCache5		=	new double[m_nLength];
		if( !m_pbHasCache5 )
		{
			m_pbHasCache5	=	new bool[m_nLength];
			if( !m_pbHasCache5 )	return false;
			for( size_t i=0; i<m_nLength; i++ )
				m_pbHasCache5[i]	=	false;
		}
		if( NULL == m_pbHasCache5 || NULL == m_pdCache5 )
			return false;
	}

	if( nIndex < m_nIndexStart || nIndex-m_nIndexStart>=m_nLength )
		return false;

	// Store	
	if( pValue1 && m_pdCache1 && m_pbHasCache1 )
	{
		m_pdCache1[nIndex-m_nIndexStart]		=	*pValue1;
		m_pbHasCache1[nIndex-m_nIndexStart]		=	true;
	}
	if( pValue2 && m_pdCache2 && m_pbHasCache2 )
	{
		m_pdCache2[nIndex-m_nIndexStart]		=	*pValue2;
		m_pbHasCache2[nIndex-m_nIndexStart]		=	true;
	}
	if( pValue3 && m_pdCache3 && m_pbHasCache3 )
	{
		m_pdCache3[nIndex-m_nIndexStart]		=	*pValue3;
		m_pbHasCache3[nIndex-m_nIndexStart]		=	true;
	}
	if( pValue4 && m_pdCache4 && m_pbHasCache4 )
	{
		m_pdCache4[nIndex-m_nIndexStart]		=	*pValue4;
		m_pbHasCache4[nIndex-m_nIndexStart]		=	true;
	}
	if( pValue5 && m_pdCache5 && m_pbHasCache5 )
	{
		m_pdCache5[nIndex-m_nIndexStart]		=	*pValue5;
		m_pbHasCache5[nIndex-m_nIndexStart]		=	true;
	}
	return true;
}

// 准备数据，得到较大值和较小值;
bool TechnicalIndicator::intensity_prepare( size_t nIndex, uint32_t * pnCode, size_t nMaxDays, size_t nMinMaxDays, double *pdLiminalLow, double *pdLiminalHigh, double dGateLow /*= 0.3*/, double dGateHigh /*= 0.7 */ )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;

	if( nIndex < nMaxDays + nMinMaxDays )
		return false;
	int	nStart	=	nIndex - nMinMaxDays + 1;
	if( nStart < 0 )
		return false;

	double	dMin = 0, dMax = 0;
	if( !min_max_info( nStart, nIndex, &dMin, &dMax ) )
		return false;

	if( pdLiminalLow )	*pdLiminalLow	=	dMin + (dMax-dMin)*dGateLow;
	if( pdLiminalHigh )	*pdLiminalHigh	=	dMin + (dMax-dMin)*dGateHigh;
	return true;
}

// 准备数据，得到价格较大值和较小值;
bool TechnicalIndicator::IntensityPreparePrice( size_t nIndex, uint32_t * pnCode, size_t nMaxDays, size_t nMinMaxDays, double *pdLiminalLow, double *pdLiminalHigh, double dGateLow, double dGateHigh )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;

	if( nIndex < nMaxDays + nMinMaxDays )
		return false;
	int	nStart	=	nIndex - nMinMaxDays + 1;
	if( nStart < 0 )
		return false;

	float	fMin = 0, fMax = 0;
	if( !m_pKData || !m_pKData->GetMinMaxInfo( nStart, nIndex, (double*)&fMin, (double*)&fMax ) )
		return false;

	if( pdLiminalLow )	*pdLiminalLow	=	(fMin + (fMax-fMin)*dGateLow);
	if( pdLiminalHigh )	*pdLiminalHigh	=	(fMin + (fMax-fMin)*dGateHigh);
	return true;
}

// 是否创新高或者创新低;
bool TechnicalIndicator::IsNewValue( size_t nIndex, bool bTopOrBottom, size_t nDays, double * pdValue1, double * pdValue2 )
{
	assert( nDays > 3 );
	if( nDays < 3 )
		return false;
	assert( NULL != pdValue1 || NULL != pdValue2 );
	if( NULL == pdValue1 && NULL == pdValue2 )
		return false;
	if( m_nLength <= 0 || m_nIndexStart < 0
		|| nIndex < m_nIndexStart || nIndex-m_nIndexStart>=m_nLength
		|| nIndex-m_nIndexStart < nDays-1 )
		return false;

	bool	bFirst = true;
	double	dLast1 = 0, dLast2 = 0;
	size_t		nLast1 = 0, nLast2 = 0;
	double	dNow1 = 0, dNow2 = 0;
	if( NULL != pdValue1 )
		dNow1	=	pdValue1[nIndex-m_nIndexStart];
	if( NULL != pdValue2 )
		dNow2	=	pdValue2[nIndex-m_nIndexStart];
	for( size_t k=nIndex-m_nIndexStart-1; k>=0 && k>nIndex-m_nIndexStart-nDays; k-- )
	{
		if( bFirst )
		{
			if( pdValue1 )	dLast1	=	pdValue1[k];
			if( pdValue2 )	dLast2	=	pdValue2[k];
			nLast1	=	nLast2	=	k;
			bFirst	=	false;
		}

		if( bTopOrBottom )
		{
			if( pdValue1 && pdValue1[k] > dNow1 )
				return false;
			if( pdValue2 && pdValue2[k] > dNow2 )
				return false;

			if( pdValue1 && pdValue1[k] > dLast1 )
			{
				dLast1	=	pdValue1[k];
				nLast1	=	k;
			}
			if( pdValue2 && pdValue2[k] > dLast2 )
			{
				dLast2	=	pdValue2[k];
				nLast2	=	k;
			}
		}
		else
		{
			if( pdValue1 && pdValue1[k] < dNow1 )
				return false;
			if( pdValue2 && pdValue2[k] < dNow2 )
				return false;

			if( pdValue1 && pdValue1[k] < dLast1 )
			{
				dLast1	=	pdValue1[k];
				nLast1	=	k;
			}
			if( pdValue2 && pdValue2[k] < dLast2 )
			{
				dLast2	=	pdValue2[k];
				nLast2	=	k;
			}
		}
	}

	if( bFirst )
		return false;

	if( ( !pdValue1 || (nLast1 >= nIndex-30 && nLast1 <= nIndex-5) )
		&& ( !pdValue2 || (nLast2 >= nIndex-30 && nLast2 <= nIndex-5) ) )
		return true;

	return false;
}

// 是否底背离;
bool TechnicalIndicator::is_deviate_on_bottom( size_t nIndex, double * pdValue1, double * pdValue2 )
{
	//STT_ASSERT_CALCULATE1( m_pKData, nIndex );

	if( !m_pKData->IsNewValue( nIndex, false, ITS_DAYS_DEVIATE ) )
		return false;

	if( IsNewValue( nIndex, false, ITS_DAYS_DEVIATE, pdValue1, pdValue2 ) )
		return false;

	return true;
}

// 是否顶背离;
bool TechnicalIndicator::is_deviate_on_top( size_t nIndex, double * pdValue1, double * pdValue2 )
{
	STT_ASSERT_CALCULATE1( m_pKData, nIndex );

	if( !m_pKData->IsNewValue( nIndex, true, ITS_DAYS_DEVIATE ) )
		return false;

	if( IsNewValue( nIndex, true, ITS_DAYS_DEVIATE, pdValue1, pdValue2 ) )
		return false;

	return true;
}

// 是否金叉;
bool TechnicalIndicator::is_golden_fork( size_t nIndex, double * pdValue1, double * pdValue2 )
{
	assert( NULL != pdValue1 && NULL != pdValue2 );
	if( NULL == pdValue1 || NULL == pdValue2 )
		return false;
	if( m_nLength <= 0 || m_nIndexStart < 0
		|| nIndex < m_nIndexStart || nIndex-m_nIndexStart>=m_nLength
		|| nIndex-m_nIndexStart < 1 )
		return false;

	int	k	=	nIndex-m_nIndexStart;
	if( pdValue1[k-1] <= pdValue2[k-1] && pdValue1[k] > pdValue2[k]
	&& pdValue1[k] >= pdValue1[k-1] && pdValue2[k] >= pdValue2[k-1] )
		return true;
	return false;
}

// 是否死叉;
bool TechnicalIndicator::is_dead_fork( size_t nIndex, double * pdValue1, double * pdValue2 )
{
	assert( NULL != pdValue1 && NULL != pdValue2 );
	if( NULL == pdValue1 || NULL == pdValue2 )
		return false;
	if( m_nLength <= 0 || m_nIndexStart < 0
		|| nIndex < m_nIndexStart || nIndex-m_nIndexStart>=m_nLength
		|| nIndex-m_nIndexStart < 1 )
		return false;

	int	k	=	nIndex-m_nIndexStart;
	if( pdValue1[k-1] >= pdValue2[k-1] && pdValue1[k] < pdValue2[k]
	&& pdValue1[k] <= pdValue1[k-1] && pdValue2[k] <= pdValue2[k-1] )
		return true;
	return false;
}

// 得到金叉或者死叉信号;
int TechnicalIndicator::GetForkSignal(size_t nIndex, std::vector<uint32_t> & adwDays,
                                 int itsGoldenFork, int itsDeadFork, uint32_t * pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	if( nIndex <= 0 )
		return ITS_NOTHING;

	for( std::size_t k=1; k<adwDays.size(); k++ )
	{
		double	dMANow1 = 0, dMANow2 = 0, dMALast1 = 0, dMALast2 = 0;
		if( !calc( &dMANow1, nIndex, min(adwDays[k-1],adwDays[k]), false )
			|| !calc( &dMANow2, nIndex, max(adwDays[k-1],adwDays[k]), false )
			|| !calc( &dMALast1, nIndex-1, min(adwDays[k-1],adwDays[k]), false )
			|| !calc( &dMALast2, nIndex-1, max(adwDays[k-1],adwDays[k]), false ) )
			return ITS_NOTHING;
		
		if( dMALast1 <= dMALast2 && dMANow1 > dMANow2
			&& dMANow1 >= dMALast1 && dMANow2 >= dMALast2 )
		{
			if( adwDays.size()-1 == k )
			{
				if( pnCode )	
					*pnCode	=	ITSC_GOLDENFORK;
				return itsGoldenFork;
			}
			for( std::size_t l=k+1; l<adwDays.size(); l++ )
			{
				double	dMANow3 = 0, dMALast3 = 0;
				if( !calc( &dMANow3, nIndex, adwDays[l], false )
					|| !calc( &dMALast3, nIndex-1, adwDays[l], false ) )
					return ITS_NOTHING;
				if( dMANow3 >= dMALast3 )
				{
					if( pnCode )	*pnCode	=	ITSC_GOLDENFORK;
					return itsGoldenFork;
				}
			}
		}
		if( dMALast1 >= dMALast2 && dMANow1 < dMANow2
			&& dMANow1 <= dMALast1 && dMANow2 <= dMALast2 )
		{
			if( adwDays.size()-1 == k )
			{
				if( pnCode )	*pnCode	=	ITSC_DEADFORK;
				return itsDeadFork;
			}
			for( size_t l=k+1; l<adwDays.size(); l++ )
			{
				double	dMANow3 = 0, dMALast3 = 0;
				if( !calc( &dMANow3, nIndex, adwDays[l], false )
					|| !calc( &dMALast3, nIndex-1, adwDays[l], false ) )
					return ITS_NOTHING;
				if( dMANow3 <= dMALast3 )
				{
					if( pnCode )	*pnCode	=	ITSC_DEADFORK;
					return itsDeadFork;
				}
			}
		}
	}
	return ITS_NOTHING;
}

// 得到金叉或者死叉信号;
int TechnicalIndicator::GetForkSignal( size_t nIndex, int itsGoldenFork, int itsDeadFork, uint32_t *pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	if( nIndex <= 0 )
		return ITS_NOTHING;

	double dLast1 = 0, dLast2 = 0, dNow1 = 0, dNow2 = 0;
	if( !calc( &dLast1, &dLast2, nIndex-1, false )
		|| !calc( &dNow1, &dNow2, nIndex, false ) )
		return ITS_NOTHING;

	if( dLast1 <= dLast2 && dNow1 > dNow2
		&& dNow1 >= dLast1 && dNow2 >= dLast2 )
	{
		if( pnCode )	*pnCode	=	ITSC_GOLDENFORK;
		return itsGoldenFork;
	}
	if( dLast1 >= dLast2 && dNow1 < dNow2
		&& dNow1 <= dLast1 && dNow2 <= dLast2 )
	{
		if( pnCode )	*pnCode	=	ITSC_DEADFORK;
		return itsDeadFork;
	}

	return ITS_NOTHING;
}

//得到趋势信号;
int TechnicalIndicator::GetTrendIntensity(size_t nIndex, std::vector<uint32_t> & adwDays,
                                  int itsLong, int itsShort, uint32_t * pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	if( nIndex <= 0 )
		return ITS_NOTHING;

	int	nRet	=	ITS_NOTHING;
	for( size_t k=1; k<adwDays.size(); k++ )
	{
		double	dMALast1 = 0, dMALast2 = 0, dMANow1 = 0, dMANow2 = 0;
		if( !calc( &dMALast1, nIndex-1, min(adwDays[k-1],adwDays[k]), false )
			|| !calc( &dMALast2, nIndex-1, max(adwDays[k-1],adwDays[k]), false )
			|| !calc( &dMANow1, nIndex, min(adwDays[k-1],adwDays[k]), false )
			|| !calc( &dMANow2, nIndex, max(adwDays[k-1],adwDays[k]), false ) )
			return ITS_NOTHING;

		if( dMANow1 >= dMALast1 && dMANow2 >= dMALast2
			&& dMANow1 > dMANow2 && (dMANow1-dMANow2)>=(dMALast1-dMALast2)
			&& (ITS_ISBUY(nRet) || 1==k) )
		{
			if( pnCode )	*pnCode	=	ITSC_LONG;
			nRet	=	itsLong;
		}
		else if( dMANow1 <= dMALast1 && dMANow2 <= dMALast2
			&& dMANow1 < dMANow2 && (dMANow1-dMANow2)<=(dMALast1-dMALast2)
			&& (ITS_ISSELL(nRet) || 1==k) )
		{
			if( pnCode )	*pnCode	=	ITSC_SHORT;
			nRet	=	itsShort;
		}
		else
		{
			if( pnCode )	*pnCode	=	ITSC_NOTHING;
			return ITS_NOTHING;
		}
	}
	return nRet;
}

// 得到趋势信号;
int TechnicalIndicator::GetTrendIntensity1( size_t nIndex, int itsLong, int itsShort, uint32_t *pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	if( nIndex <= 0 )
		return ITS_NOTHING;

	double	dLast = 0, dNow = 0;
	if( !calc( &dLast, nIndex-1, false )
		|| !calc( &dNow, nIndex, false ) )
		return ITS_NOTHING;

	if( dNow > dLast )
	{
		if( pnCode )	*pnCode	=	ITSC_LONG;
		return itsLong;
	}
	if( dNow < dLast )
	{
		if( pnCode )	*pnCode	=	ITSC_SHORT;
		return itsShort;
	}
	return ITS_NOTHING;
}

// 得到趋势信号;
int TechnicalIndicator::GetTrendIntensity2( size_t nIndex, int itsLong, int itsShort, uint32_t *pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	if( nIndex <= 0 )
		return ITS_NOTHING;

	double	dLast1 = 0, dLast2 = 0, dNow1 = 0, dNow2 = 0;
	if( calc( &dLast1, &dLast2, nIndex-1, false )
		&& calc( &dNow1, &dNow2, nIndex, false ) )
	{
		if( dNow1 >= dLast1 && dNow2 >= dLast2
			&& dNow1 > dNow2 && (dNow1-dNow2)>=(dLast1-dLast2) )
		{
			if( pnCode )	*pnCode	=	ITSC_LONG;
			return itsLong;
		}
		if( dNow1 <= dLast1 && dNow2 <= dLast2
			&& dNow1 < dNow2 && (dNow1-dNow2)<=(dLast1-dLast2) )
		{
			if( pnCode )	*pnCode	=	ITSC_SHORT;
			return itsShort;
		}
	}
	return ITS_NOTHING;
}

// 得到趋势信号;
int TechnicalIndicator::GetTrendIntensity(size_t nIndex, double * pdValue1, double * pdValue2,
                                  int itsLong, int itsShort, uint32_t *pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;

	assert( NULL != pdValue1 && NULL != pdValue2 );
	if( NULL == pdValue1 || NULL == pdValue2 )
		return false;
	if( m_nLength <= 0 || m_nIndexStart < 0
		|| nIndex < m_nIndexStart || nIndex-m_nIndexStart>=m_nLength
		|| nIndex-m_nIndexStart < 1 )
		return false;

	int	k	=	nIndex-m_nIndexStart;
	if( pdValue1[k] >= pdValue1[k-1] && pdValue2[k] >= pdValue2[k-1]
	&& pdValue1[k] > pdValue2[k] && (pdValue1[k]-pdValue2[k])>=(pdValue1[k-1]-pdValue2[k-1]) )
	{
		if( pnCode )	*pnCode	=	ITSC_LONG;
		return itsLong;
	}
	if( pdValue1[k] <= pdValue1[k-1] && pdValue2[k] <= pdValue2[k-1]
	&& pdValue1[k] < pdValue2[k] && (pdValue1[k]-pdValue2[k])<=(pdValue1[k-1]-pdValue2[k-1]) )
	{
		if( pnCode )	*pnCode	=	ITSC_SHORT;
		return itsShort;
	}
	return ITS_NOTHING;
}

void TechnicalIndicator::SetDefaultParameters( )
{
	assert( false );
}

void TechnicalIndicator::attach( TechnicalIndicator & src )
{
	assert( false );
}

bool TechnicalIndicator::IsValidParameters( )
{
	assert( false );
	return false;
}

// 创建新的技术指标对象;
TechnicalIndicator * TechnicalIndicator::create( uint32_t nTech, KdataContainer * pKData )
{
	TechnicalIndicator	*	pTech	=	NULL;
	switch( nTech )
	{
	case STT_MA:		pTech	=	new CMA( pKData );		break;
	//case STT_BBI:		pTech	=	new CBBI( pKData );		break;
	case STT_BOLL:		pTech	=	new CBOLL( pKData );	break;
	//case STT_PV:		pTech	=	new CPV( pKData );		break;
	//case STT_SAR:		pTech	=	new CSAR( pKData );		break;
	//case STT_DJ:		pTech	=	new CDJ( pKData );		break;
	//case STT_CW:		pTech	=	new CCW( pKData );		break;

	case STT_MACD:		pTech	=	new CMACD( pKData );	break;
	//case STT_MIKE:		pTech	=	new CMIKE( pKData );	break;
	//case STT_PSY:		pTech	=	new CPSY( pKData );		break;
	//case STT_VOLUME:	pTech	=	new CVOLUME( pKData );	break;
	//case STT_NVI:		pTech	=	new CNVI( pKData );		break;
	//case STT_PVI:		pTech	=	new CPVI( pKData );		break;
	//case STT_VR:		pTech	=	new CVR( pKData );		break;
	//case STT_VROC:		pTech	=	new CVROC( pKData );	break;
	//case STT_OBV:		pTech	=	new COBV( pKData );		break;
	//case STT_MOBV:		pTech	=	new CMOBV( pKData );	break;
	//case STT_MFI:		pTech	=	new CMFI( pKData );		break;
	//case STT_VMACD:		pTech	=	new CVMACD( pKData );	break;
	//case STT_WVAD:		pTech	=	new CWVAD( pKData );	break;
	//case STT_EMV:		pTech	=	new CEMV( pKData );		break;
	//case STT_VRSI:		pTech	=	new CVRSI( pKData );	break;
	//case STT_NVRSI:		pTech	=	new CNVRSI( pKData );	break;
	//case STT_AD:		pTech	=	new CAD( pKData );		break;
	//case STT_CI:		pTech	=	new CCI( pKData );		break;
	//	// 
	case STT_KDJ:		pTech	=	new CKDJ( pKData );		break;
	/*case STT_R:			pTech	=	new CR( pKData );		break;
	case STT_RSI:		pTech	=	new CRSI( pKData );		break;
	case STT_BIAS:		pTech	=	new CBIAS( pKData );	break;
	case STT_MTM:		pTech	=	new CMTM( pKData );		break;
	case STT_DMI:		pTech	=	new CDMI( pKData );		break;
	case STT_ROC:		pTech	=	new CROC( pKData );		break;
	case STT_CCI:		pTech	=	new CCCI( pKData );		break;
	case STT_CV:		pTech	=	new CCV( pKData );		break;
	case STT_ARBR:		pTech	=	new CARBR( pKData );	break;
	case STT_CR:		pTech	=	new CCR( pKData );		break;
	case STT_OSC:		pTech	=	new COSC( pKData );		break;
	case STT_UOS:		pTech	=	new CUOS( pKData );		break;
	case STT_MAOSC:		pTech	=	new CMAOSC( pKData );	break;
	case STT_36BIAS:	pTech	=	new C36BIAS( pKData );	break;
	case STT_DPO:		pTech	=	new CDPO( pKData );		break;
	case STT_KST:		pTech	=	new CKST( pKData );		break;

	case STT_REI:		pTech	=	new CREI( pKData );		break;
	case STT_DMKI:		pTech	=	new CDMKI( pKData );	break;
	case STT_PCNT:		pTech	=	new CPCNT( pKData );	break;
	case STT_HLC:		pTech	=	new CHLC( pKData );		break;
	case STT_CDP:		pTech	=	new CCDP( pKData );		break;
	case STT_ASI:		pTech	=	new CASI( pKData );		break;
	case STT_ATR:		pTech	=	new CATR( pKData );		break;

	case STT_CYO:		pTech	=	new CCYO( pKData );		break;
	case STT_DCYO:		pTech	=	new CDCYO( pKData );	break;
	case STT_HSL:		pTech	=	new CHSL( pKData );		break;
        */
	case STT_DPER:		pTech	=	new CDPER( pKData );	break;

	//default:
	//	assert( false );
	}
	return pTech;
}

// 得到买卖信号;
int TechnicalIndicator::signal( size_t nIndex, uint32_t * pnCode /*= NULL */ )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	return	ITS_NOTHING;
}

// 得到趋势信号;
int TechnicalIndicator::intensity( size_t nIndex, uint32_t * pnCode /*= NULL*/, bool bTraceBack /*= true */ )
{
    uint32_t	nCode	=	ITSC_NOTHING;
	int	nSignal	=	signal( nIndex, &nCode );
	if( ITS_NOTHING != nSignal )
	{
		m_nLastIntensity		=	nSignal;
		m_nLastIntensityCode	=	nCode;
		if( pnCode )	*pnCode	=	nCode;
		return nSignal;
	}

	if( m_nLastIntensity > 1 )
		m_nLastIntensity	--;
	if( m_nLastIntensity < -1  )
		m_nLastIntensity	++;

	if( bTraceBack && ITS_NOTHING == m_nLastIntensity )
		m_nLastIntensity		=	intensity_traceback( nIndex, &m_nLastIntensityCode );

	if( pnCode )	*pnCode	=	m_nLastIntensityCode;
	return m_nLastIntensity;
}

int TechnicalIndicator::intensity_traceback( size_t nIndex, uint32_t * pnCode /*= NULL */ )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;

	int		nIntensity	=	ITS_NOTHING;
    uint32_t	nCode		=	ITSC_NOTHING;
	size_t k;
	for( k=nIndex; k>=0; k -- )
	{
		nIntensity	=	signal(k,&nCode);
		if( ITS_NOTHING != nIntensity )
			break;
	}
	if( k < 0 )
		return ITS_NOTHING;
	for( k=k+1; k<=nIndex; k++ )
	{
		if( nIntensity > 1 )
			nIntensity	--;
		else if( nIntensity < -1  )
			nIntensity	++;
		else
			break;
	}
	if( pnCode )	*pnCode	=	nCode;
	return nIntensity;
}

bool TechnicalIndicator::min_max_info( size_t nStart, size_t nEnd, double *pdMin, double *pdMax )
{
	assert( false );
	return false;
}

bool TechnicalIndicator::calc( double * pValue, size_t nIndex, size_t nDays, bool bUseLast )
{
	assert( false );
	return false;
}

bool TechnicalIndicator::calc( double * pValue, size_t nIndex, bool bUseLast )
{
	assert( false );
	return false;
}

bool TechnicalIndicator::calc( double * pValue1, double * pValue2, size_t nIndex, bool bUseLast )
{
	assert( false );
	return false;
}

bool TechnicalIndicator::calc( double * pValue1, double * pValue2, double * pValue3, size_t nIndex, bool bUseLast )
{
	assert( false );
	return false;
}

bool TechnicalIndicator::calc( double * pValue1, double * pValue2, double * pValue3, double *pValue4, size_t nIndex, bool bUseLast )
{
	assert( false );
	return false;
}

bool TechnicalIndicator::calc( double * pValue1, double * pValue2, double * pValue3, double *pValue4, double *pValue5, size_t nIndex, bool bUseLast )
{
	//assert( false );
	return false;
}

// 计算指标值和均值
bool TechnicalIndicator::calc_ma( double * pValue, double * pMA, size_t nIndex, bool bUseLast, size_t nMADays )
{
	STT_ASSERT_CALCULATE1( m_pKData, nIndex );

	if( nMADays > nIndex+1 )
		return false;

	if( load_from_cache( nIndex, pValue, pMA ) )
		return true;

	double	dValue = 0, dMA = 0;
	int	nCount	=	0;
	for( int k=nIndex; k>=0; k-- )
	{
		double	dTemp = 0;
		if( calc( &dTemp, k, false ) )
		{
			if( nIndex == k )
				dValue	=	dTemp;
			dMA	+=	dTemp;

			nCount	++;
			if( nCount == nMADays )
			{
				dMA	=	dMA / nMADays;
				if( pValue )	*pValue	=	dValue;
				if( pMA )		*pMA	=	dMA;
				store_to_cache( nIndex, pValue, pMA );
				return true;
			}
		}
	}
	return false;
}
