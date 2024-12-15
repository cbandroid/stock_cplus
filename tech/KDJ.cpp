#include "KDJ.h"
#include "TechLib.h"
#include "KData.h"
//////////////////////////////////////////////////////////////////////
//	CKDJ
CKDJ::CKDJ( )
{
    CKDJ::SetDefaultParameters( );
}

CKDJ::CKDJ( CKData * pKData )
: CTechnique( pKData )
{
    CKDJ::SetDefaultParameters( );
}

CKDJ::~CKDJ()
{
    CKDJ::Clear( );
}

void CKDJ::SetDefaultParameters( )
{
	m_nRSVDays	=	9;
	m_nKDays	=	3;
	m_nDDays	=	3;
	m_nJ		=	1;
	m_KDTrustValue.first = 25;
	m_KDTrustValue.second = 75;
    m_itsGoldenFork	=	ITS_BUY;
    m_itsDeadFork	=	ITS_SELL;
}

void CKDJ::attach( CKDJ & src )
{
	m_nRSVDays	=	src.m_nRSVDays;
	m_nKDays	=	src.m_nKDays;
	m_nDDays	=	src.m_nDDays;
	m_nJ		=	src.m_nJ;
	m_itsGoldenFork	=	src.m_itsGoldenFork;
	m_itsDeadFork	=	src.m_itsDeadFork;
}

bool CKDJ::IsValidParameters( )
{
	return ( VALID_DAYS(m_nRSVDays) && VALID_DAYS(m_nKDays)
		&& VALID_DAYS(m_nDDays) && VALID_DAYS(m_nJ)
		&& VALID_ITS(m_itsGoldenFork) && VALID_ITS(m_itsDeadFork) );
}

void CKDJ::Clear( )
{
   CTechnique::Clear();
}

// 计算RSV值
bool CKDJ::CalculateRSV(float * pValue, int nIndex)
{
	//STT_ASSERT_CALCULATE1( m_pKData, nIndex );

	float	dH = 0, dL = 0, dRSV = 0;
	int	nCount	=	0;
	for(  int  k=nIndex; k>=0; k-- )
	{
		if( nIndex == k )
		{
			dH	=	m_pKData->at(k).high;
			dL	=	m_pKData->at(k).low;
		}
		if( dH < m_pKData->at(k).high )
			dH	=	m_pKData->at(k).high;
		if( dL > m_pKData->at(k).low )
			dL	=	m_pKData->at(k).low;

		nCount	++;
		if( nCount == m_nRSVDays )
		{
			if( dH-dL < 1e-4 )
				dRSV	=	100;
			else
				dRSV	=	(m_pKData->at(nIndex).close- dL) * 100 / (dH - dL);
			if( pValue )	*pValue	=	dRSV;
			return true;
		}
	}
	return false;
}

int CKDJ::signal( int nIndex, uint32_t * pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	if( !prepare_cache( 0, -1, false ) )
		return ITS_NOTHING;

	float	dK, dD, dJ;
	if( !calc( &dK, &dD, &dJ, nIndex, false ) )
		return ITS_NOTHING;

	if (dK < m_KDTrustValue.first && dD < m_KDTrustValue.first && is_golden_fork(nIndex, m_pdCache1, m_pdCache2))
	{	// 低位金叉;
		if( pnCode )	*pnCode	=	ITSC_GOLDENFORK;
		return m_itsGoldenFork;
	}
	if (dK > m_KDTrustValue.second && dD > m_KDTrustValue.second && is_dead_fork(nIndex, m_pdCache1, m_pdCache2))
	{	// 高位死叉;
		if( pnCode )	*pnCode	=	ITSC_DEADFORK;
		return m_itsDeadFork;
	}
	return ITS_NOTHING;
}

bool CKDJ::min_max_info(int nStart, int nEnd, float *pdMin,float *pdMax)
{
	return AfxGetMinMaxInfo3( nStart, nEnd, pdMin, pdMax, this );
}

/***
AX = 今天的收盘价 - N天中的最低价
BX = N天中的最高价 - N天中的最低价
RSV = （AX ÷ BX）× 100%
NK = K计算天数，一般取3
ND = D计算天数，一般取3
K = 前一日K×(NK-1)/NK + RSV×1/NK
D = 前一日D×(ND-1)/ND + K×1/3
J = 3D － 2K （或 J = 3K - 2D）
第一次计算时，前一日的K、D值皆以50代替。
*/
bool CKDJ::calc(float *pValue1, float *pValue2, float *pValue3, int nIndex, bool bUseLast)
{
	STT_ASSERT_CALCULATE1( m_pKData, nIndex );

	if( m_nRSVDays > nIndex+1 )
		return false;

	if( load_from_cache( nIndex, pValue1, pValue2, pValue3 ) )
		return true;

	float	dRSV = 0;
	float	dK = 50, dD = 50, dJ = 50;
	if( bUseLast && pValue1 && pValue2 )
	{
		if( CalculateRSV( &dRSV, nIndex ) )
		{
			dK	=	(m_nKDays-1)*(*pValue1)/m_nKDays + dRSV/m_nKDays;
			dD	=	(m_nDDays-1)*(*pValue2)/m_nDDays + dK/m_nDDays;
			if( mode3K2D == m_nJ )
				dJ	=	3 * dK - 2 * dD;
			else
				dJ	=	3 * dD - 2 * dK;
			if( pValue1 )	*pValue1	=	dK;
			if( pValue2 )	*pValue2	=	dD;
			if( pValue3 )	*pValue3	=	dJ;
			store_to_cache( nIndex, pValue1, pValue2, pValue3 );
			return true;
		}
	}
	else
	{
		double	factor1 = 1, factor2 = 1;
		int k;
		for( k=nIndex; k > 0; k-- )
		{
			factor1		*=	((double)(m_nKDays-1))/m_nKDays;
			factor2		*=	((double)(m_nDDays-1))/m_nDDays;
            if( factor1 < 0.001f && factor2 < 0.001f )
				break;
		}
		for( ; k<=nIndex; k++ )
		{
			if( CalculateRSV( &dRSV, k ) )
			{
				dK	=	(m_nKDays-1)*(dK)/m_nKDays + dRSV/m_nKDays;
				dD	=	(m_nDDays-1)*(dD)/m_nDDays + dK/m_nDDays;
				if( mode3K2D == m_nJ )
					dJ	=	3 * dK - 2 * dD;
				else
					dJ	=	3 * dD - 2 * dK;
				if( nIndex == k )
				{
					if( pValue1 )	*pValue1	=	dK;
					if( pValue2 )	*pValue2	=	dD;
					if( pValue3 )	*pValue3	=	dJ;
					store_to_cache( nIndex, pValue1, pValue2, pValue3 );
					return true;
				}
			}
		}
	}

	return false;
}
