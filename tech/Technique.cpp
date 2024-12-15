#include "Technique.h"
#include "KDJ.h"
#include "BOLL.h"
#include "MACD.h"
#include "MA.h"
#include "VOLUME.h"
#include "TechLib.h"

uint32_t	AfxGetMaxDays( std::vector<uint32_t>& adw )
{
	uint32_t	dwMax	=	0;
    for( size_t i=0; i<adw.size(); i++ )
	{
		if( adw.at(i) > dwMax )
			dwMax	=	adw.at(i);
	}
	return dwMax;
}

//////////////////////////////////////////////////////////////////////
//	CTechnique

CTechnique::CTechnique( )
{
    m_pKData		=	nullptr;

    m_pdCache1		=	nullptr;
    m_pdCache2		=	nullptr;
    m_pdCache3		=	nullptr;
    m_pdCache4		=	nullptr;
    m_pdCache5		=	nullptr;
    m_pbHasCache1	=	nullptr;
    m_pbHasCache2	=	nullptr;
    m_pbHasCache3	=	nullptr;
    m_pbHasCache4	=	nullptr;
    m_pbHasCache5	=	nullptr;
	m_nLength		=	0;
	m_nIndexStart	=	-1;

	m_nLastIntensity		=	ITS_NOTHING;
	m_nLastIntensityCode	=	ITSC_NOTHING;

	m_nTech        =   -1;
}

CTechnique::CTechnique( CKData * pKData )
{
	m_pKData	=	pKData;

    m_pdCache1		=	nullptr;
    m_pdCache2		=	nullptr;
    m_pdCache3		=	nullptr;
    m_pdCache4		=	nullptr;
    m_pdCache5		=	nullptr;
    m_pbHasCache1	=	nullptr;
    m_pbHasCache2	=	nullptr;
    m_pbHasCache3	=	nullptr;
    m_pbHasCache4	=	nullptr;
    m_pbHasCache5	=	nullptr;
	m_nLength		=	0;
	m_nIndexStart	=	-1;

	m_nLastIntensity		=	ITS_NOTHING;
	m_nLastIntensityCode	=	ITSC_NOTHING;
}

CTechnique::~CTechnique()
{
	Clear( );
}

void CTechnique::set_kdata( CKData * pKData )
{
	m_pKData	=	pKData;
	Clear( );
}

CKData * CTechnique::get_kdata( )
{
	return m_pKData;
}

void CTechnique::Clear( )
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

    m_pdCache1		=	nullptr;
    m_pdCache2		=	nullptr;
    m_pdCache3		=	nullptr;
    m_pdCache4		=	nullptr;
    m_pdCache5		=	nullptr;
    m_pbHasCache1	=	nullptr;
    m_pbHasCache2	=	nullptr;
    m_pbHasCache3	=	nullptr;
    m_pbHasCache4	=	nullptr;
    m_pbHasCache5	=	nullptr;
	m_nLength		=	0;
	m_nIndexStart	=	-1;

	m_nLastIntensity		=	ITS_NOTHING;
	m_nLastIntensityCode	=	ITSC_NOTHING;
}

void CTechnique::clear_last_intensity( )
{
	m_nLastIntensity		=	ITS_NOTHING;
	m_nLastIntensityCode	=	ITSC_NOTHING;
}

// 准备缓冲区
bool CTechnique::prepare_cache( int nStart, int nEnd, bool bForceRecalculate )
{
	assert( m_pKData );
    if( nullptr == m_pKData )
		return false;
	if(  nEnd < 0 )
		nEnd	=	(int)m_pKData->size()-1;
    if( nStart < 0 || nEnd < nStart || nEnd >= (int)m_pKData->size() )
		return false;

	if( bForceRecalculate )
		Clear( );

    return min_max_info( nStart, nEnd, nullptr, nullptr );
}

// 从缓冲区中读取计算好的数据
bool CTechnique::load_from_cache( int  nIndex, float * pValue1, float * pValue2, float * pValue3, float * pValue4, float * pValue5 )
{
	if( m_nLength <= 0 || m_nIndexStart < 0
        || nIndex < m_nIndexStart || (nIndex-m_nIndexStart)>=m_nLength )
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

// 将计算好的数据保存至缓冲区
bool CTechnique::store_to_cache( int  nIndex, float * pValue1, float *pValue2, float *pValue3, float * pValue4, float * pValue5 )
{
	STT_ASSERT_CALCULATE1( m_pKData, nIndex );

	if( m_nLength <= 0 || m_nIndexStart < 0 )
	{
        m_nLength		=	( nullptr!=m_pKData ? m_pKData->size() : 0 );
		if( m_nLength > 0 )
			m_nIndexStart	=	0;
	}
	if( m_nLength <= 0 || m_nIndexStart <  0 )
		return false;
	
	// Realocate
	if( pValue1 )
	{
		if( !m_pdCache1 )
			m_pdCache1		=	new float[m_nLength];
		if( !m_pbHasCache1 )
		{
			m_pbHasCache1	=	new bool[m_nLength];
			if( !m_pbHasCache1 )	return false;
			for( int i=0; i<m_nLength; i++ )
				m_pbHasCache1[i]	=	false;
		}
        if( nullptr == m_pbHasCache1 || nullptr == m_pdCache1 )
			return false;
	}
	if( pValue2 )
	{
		if( !m_pdCache2 )
			m_pdCache2		=	new float[m_nLength];
		if( !m_pbHasCache2 )
		{
			m_pbHasCache2	=	new bool[m_nLength];
			if( !m_pbHasCache2 )	return false;
			for( int i=0; i<m_nLength; i++ )
				m_pbHasCache2[i]	=	false;
		}
        if( nullptr == m_pbHasCache2 || nullptr == m_pdCache2 )
			return false;
	}
	if( pValue3 )
	{
		if( !m_pdCache3 )
			m_pdCache3		=	new float[m_nLength];
		if( !m_pbHasCache3 )
		{
			m_pbHasCache3	=	new bool[m_nLength];
			if( !m_pbHasCache3 )	return false;
			for( int i=0; i<m_nLength; i++ )
				m_pbHasCache3[i]	=	false;
		}
        if( nullptr == m_pbHasCache3 || nullptr == m_pdCache3 )
			return false;
	}
	if( pValue4 )
	{
		if( !m_pdCache4 )
			m_pdCache4		=	new float[m_nLength];
		if( !m_pbHasCache4 )
		{
			m_pbHasCache4	=	new bool[m_nLength];
			if( !m_pbHasCache4 )	return false;
			for( int i=0; i<m_nLength; i++ )
				m_pbHasCache4[i]	=	false;
		}
        if( nullptr == m_pbHasCache4 || nullptr == m_pdCache4 )
			return false;
	}
	if( pValue5 )
	{
		if( !m_pdCache5 )
			m_pdCache5		=	new float[m_nLength];
		if( !m_pbHasCache5 )
		{
			m_pbHasCache5	=	new bool[m_nLength];
			if( !m_pbHasCache5 )	return false;
			for( int  i=0; i<m_nLength; i++ )
				m_pbHasCache5[i]	=	false;
		}
        if( nullptr == m_pbHasCache5 || nullptr == m_pdCache5 )
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

// 准备数据，得到较大值和较小值
bool CTechnique::intensity_prepare( int nIndex,uint32_t* pnCode, int nMaxDays, int nMinMaxDays, float *pdLiminalLow, float *pdLiminalHigh, float dGateLow, float dGateHigh )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;

	if( nIndex < nMaxDays + nMinMaxDays )
		return false;
	int	nStart	=	nIndex - nMinMaxDays + 1;
	if( nStart < 0 )
		return false;

	float	dMin = 0, dMax = 0;
	if( !min_max_info( nStart, nIndex, &dMin, &dMax ) )
		return false;

	if( pdLiminalLow )	*pdLiminalLow	=	dMin + (dMax-dMin)*dGateLow;
	if( pdLiminalHigh )	*pdLiminalHigh	=	dMin + (dMax-dMin)*dGateHigh;
	return true;
}

// 准备数据，得到价格较大值和较小值
bool CTechnique::IntensityPreparePrice(int nIndex, uint32_t * pnCode, int nMaxDays, int nMinMaxDays, float *pdLiminalLow, float *pdLiminalHigh, float dGateLow, float dGateHigh )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;

	if( nIndex < nMaxDays + nMinMaxDays )
		return false;
	int	nStart	=	nIndex - nMinMaxDays + 1;
	if( nStart < 0 )
		return false;

	float	fMin = 0, fMax = 0;
	if( !m_pKData || !m_pKData->GetMinMaxInfo( nStart, nIndex, &fMin, &fMax ) )
		return false;

	if( pdLiminalLow )	*pdLiminalLow	=	(fMin + (fMax-fMin)*dGateLow);
	if( pdLiminalHigh )	*pdLiminalHigh	=	(fMin + (fMax-fMin)*dGateHigh);
	return true;
}

// 是否创新高或者创新低
bool CTechnique::IsNewValue(int nIndex, bool bTopOrBottom, int nDays, float * pdValue1, float * pdValue2 )
{
	assert( nDays > 3 );
	if( nDays < 3 )
		return false;
    assert( nullptr != pdValue1 || nullptr != pdValue2 );
    if( nullptr == pdValue1 && nullptr == pdValue2 )
		return false;
	if( m_nLength <= 0 || m_nIndexStart < 0
		|| nIndex < m_nIndexStart || nIndex-m_nIndexStart>=m_nLength
		|| nIndex-m_nIndexStart < nDays-1 )
		return false;

	bool	bFirst = true;
	float	dLast1 = 0, dLast2 = 0;
	int		nLast1 = 0, nLast2 = 0;
	float	dNow1 = 0, dNow2 = 0;
    if( nullptr != pdValue1 )
		dNow1	=	pdValue1[nIndex-m_nIndexStart];
    if( nullptr != pdValue2 )
		dNow2	=	pdValue2[nIndex-m_nIndexStart];
	for( int k=nIndex-m_nIndexStart-1; k>=0 && k>nIndex-m_nIndexStart-nDays; k-- )
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

// 是否底背离
bool CTechnique::is_deviate_on_bottom( int  nIndex, float * pdValue1, float * pdValue2 )
{
	STT_ASSERT_CALCULATE1( m_pKData, nIndex );

	if( !m_pKData->IsNewValue( nIndex, false, ITS_DAYS_DEVIATE ) )
		return false;

	if( IsNewValue( nIndex, false, ITS_DAYS_DEVIATE, pdValue1, pdValue2 ) )
		return false;

	return true;
}

// 是否顶背离
bool CTechnique::is_deviate_on_top( int nIndex, float * pdValue1, float * pdValue2 )
{
	STT_ASSERT_CALCULATE1( m_pKData, nIndex );

	if( !m_pKData->IsNewValue( nIndex, true, ITS_DAYS_DEVIATE ) )
		return false;

	if( IsNewValue( nIndex, true, ITS_DAYS_DEVIATE, pdValue1, pdValue2 ) )
		return false;

	return true;
}

// 是否金叉
bool CTechnique::is_golden_fork( int  nIndex, float * pdValue1, float * pdValue2 )
{
    assert( nullptr != pdValue1 && nullptr != pdValue2 );
    if( nullptr == pdValue1 || nullptr == pdValue2 )
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

// 是否死叉
bool CTechnique::is_dead_fork( int  nIndex, float * pdValue1, float * pdValue2 )
{
    assert( nullptr != pdValue1 && nullptr != pdValue2 );
    if( nullptr == pdValue1 || nullptr == pdValue2 )
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

// 得到金叉或者死叉信号
int CTechnique::GetForkSignal(int nIndex, std::vector<uint32_t> & adwDays,
                                 int itsGoldenFork, int itsDeadFork, uint32_t * pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	if( nIndex <= 0 )
		return ITS_NOTHING;

	for( std::size_t k=1; k<adwDays.size(); k++ )
	{
		float	dMANow1, dMANow2, dMALast1, dMALast2;
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
				if( pnCode )	*pnCode	=	ITSC_GOLDENFORK;
				return itsGoldenFork;
			}
			for( std::size_t l=k+1; l<adwDays.size(); l++ )
			{
				float	dMANow3, dMALast3;
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
				float	dMANow3, dMALast3;
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

// 得到金叉或者死叉信号
int CTechnique::GetForkSignal( int nIndex, int itsGoldenFork, int itsDeadFork, uint32_t *pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	if( nIndex <= 0 )
		return ITS_NOTHING;

	float dLast1 = 0, dLast2 = 0, dNow1 = 0, dNow2 = 0;
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

// 得到金叉或者死叉信号
int CTechnique::GetForkSignal(int nIndex, float * pdValue1, float * pdValue2,
							  int itsGoldenFork, int itsDeadFork, uint32_t *pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;

	if( is_golden_fork( nIndex, pdValue1, pdValue2 ) )
	{
		if( pnCode )	*pnCode	=	ITSC_GOLDENFORK;
		return itsGoldenFork;
	}
	if( is_dead_fork( nIndex, pdValue1, pdValue2 ) )
	{
		if( pnCode )	*pnCode	=	ITSC_DEADFORK;
		return itsDeadFork;
	}
	return ITS_NOTHING;
}

// 得到趋势信号
int CTechnique::GetTrendIntensity(int nIndex, std::vector<uint32_t> & adwDays,
                                  int itsLong, int itsShort, uint32_t * pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	if( nIndex <= 0 )
		return ITS_NOTHING;

	int	nRet	=	ITS_NOTHING;
	for( size_t k=1; k<adwDays.size(); k++ )
	{
		float	dMALast1, dMALast2, dMANow1, dMANow2;
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

// 得到趋势信号
int CTechnique::GetTrendIntensity1( int nIndex, int itsLong, int itsShort, uint32_t *pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	if( nIndex <= 0 )
		return ITS_NOTHING;

	float	dLast = 0, dNow = 0;
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

// 得到趋势信号
int CTechnique::GetTrendIntensity2( int nIndex, int itsLong, int itsShort, uint32_t *pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	if( nIndex <= 0 )
		return ITS_NOTHING;

	float	dLast1 = 0, dLast2 = 0, dNow1 = 0, dNow2 = 0;
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

// 得到趋势信号
int CTechnique::GetTrendIntensity(int nIndex, float * pdValue1, float * pdValue2,
                                  int itsLong, int itsShort, uint32_t *pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;

    assert( nullptr != pdValue1 && nullptr != pdValue2 );
    if( nullptr == pdValue1 || nullptr == pdValue2 )
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

void CTechnique::SetDefaultParameters( )
{
	assert( false );
}

void CTechnique::attach( CTechnique & src )
{
	assert( false );
}

bool CTechnique::IsValidParameters( )
{
	assert( false );
	return false;
}

// 创建新的技术指标对象
CTechnique * CTechnique::create( int  nTech, CKData * pKData )
{
    CTechnique	*	pTech	=nullptr;
	
	switch( nTech )
	{
		case STT_MA:		pTech	=	new CMA( pKData );		break;

		case STT_BOLL:		pTech	=	new CBOLL( pKData );	break;
		
		case STT_MACD:		pTech	=	new CMACD( pKData );	break;

        case STT_VOLUME:	pTech	=	new CVOLUME( pKData );	break;
		case STT_KDJ:		pTech	=	new CKDJ( pKData );		break;
		default:
			break;
	}
	return pTech;
}

// 得到买卖信号
int CTechnique::signal( int nIndex, uint32_t * pnCode /*= NULL */ )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	return	ITS_NOTHING;
}

// 得到趋势信号
int CTechnique::intensity( int nIndex, uint32_t * pnCode /*= NULL*/, bool bTraceBack /*= true */ )
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

int CTechnique::intensity_traceback(int nIndex, uint32_t * pnCode /*= NULL */ )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;

	int		nIntensity	=	ITS_NOTHING;
    uint32_t	nCode		=	ITSC_NOTHING;
	int k;
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

bool CTechnique::min_max_info( int nStart, int  nEnd, float *pdMin, float *pdMax )
{
    Q_UNUSED(nStart);
    Q_UNUSED(nEnd);
    Q_UNUSED(pdMin);
    Q_UNUSED(pdMax);
	assert( false );
	return false;
}

int CTechnique::GetCrossValue( float *pdValue1, float *pdValue2 ,int nIndex,float &fCross)
{
    if (pdValue1 == nullptr || pdValue2 == nullptr || nIndex == 0 )  return (CROSS_NONE);
	
	float  fDif = pdValue1[ nIndex - 1 ] - pdValue2[ nIndex - 1];
	
	float  fDifEx = pdValue2[ nIndex ] - pdValue1[ nIndex ];
	
	float  fRatio = (fDif + fDifEx) ? fDif / (fDif + fDifEx) : 0.0f;
	
	fCross = (float)(pdValue1[nIndex-1] + ( pdValue1[nIndex] - pdValue1[nIndex - 1] ) * fRatio);

	if (fDif < 0 && fDifEx < 0 )	return (CROSS_UP);
	if (fDif > 0 && fDifEx > 0 )	return (CROSS_DOWN);

	return (CROSS_NONE);
}

bool CTechnique::calc( float * pValue, int nIndex, int nDays, bool bUseLast )
{
	assert( false );
	return false;
}

bool CTechnique::calc( float * pValue, int nIndex, bool bUseLast )
{
	assert( false );
	return false;
}

bool CTechnique::calc( float * pValue1, float * pValue2, int nIndex, bool bUseLast )
{
	assert( false );
	return false;
}

bool CTechnique::calc( float * pValue1, float * pValue2, float * pValue3, int nIndex, bool bUseLast )
{
	assert( false );
	return false;
}

bool CTechnique::calc( float * pValue1, float * pValue2, float * pValue3, float *pValue4, int nIndex, bool bUseLast )
{
	assert( false );
	return false;
}

bool CTechnique::calc( float * pValue1, float * pValue2, float * pValue3, float *pValue4, float *pValue5, int nIndex, bool bUseLast )
{
	//assert( false );
	return false;
}

// 计算指标值和均值
bool CTechnique::calc_ma( float * pValue, float * pMA, int nIndex, bool bUseLast, int nMADays )
{
	STT_ASSERT_CALCULATE1( m_pKData, nIndex );

	if( nMADays > nIndex+1 )
		return false;

	if( load_from_cache( nIndex, pValue, pMA ) )
		return true;

	float	dValue = 0, dMA = 0;
	int	nCount	=	0;
	for(int k=nIndex; k>=0; k-- )
	{
		float	dTemp = 0;
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

