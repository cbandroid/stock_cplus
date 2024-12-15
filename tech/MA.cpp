#include "MA.h"
#include "TechLib.h"
#include "KData.h"
//////////////////////////////////////////////////////////////////////
//	CMA
CMA::CMA( )
{
    CMA::SetDefaultParameters( );
}

CMA::CMA(CKData * pKData )
	: CTechnique( pKData )
{
    CMA::SetDefaultParameters( );
}

CMA::~CMA()
{
    CMA::Clear( );
}

void CMA::SetDefaultParameters( )
{
	m_nType	=	typeMA;
	m_adwMADays.clear();
	m_adwMADays.push_back( 5 );
	m_adwMADays.push_back( 10 );
	m_adwMADays.push_back( 20 );
	m_itsGoldenFork	=	ITS_BUYINTENSE;
	m_itsDeadFork	=	ITS_SELLINTENSE;
	m_itsLong		=	ITS_BUY;
	m_itsShort		=	ITS_SELL;
	m_dWeight = 1;
}

void CMA::attach( CMA & src )
{
	m_nType	=	src.m_nType;
	m_adwMADays=src.m_adwMADays;
	m_itsGoldenFork	=	src.m_itsGoldenFork;
	m_itsDeadFork	=	src.m_itsDeadFork;
	m_itsLong		=	src.m_itsLong;
	m_itsShort		=	src.m_itsShort;
	m_dWeight = src.m_dWeight;
}

bool CMA::IsValidParameters( )
{
	//STT_VALID_DAYSARRAY( m_adwMADays );
	return ((typeMA == m_nType || typeEXPMA == m_nType || typeSMA == m_nType || typeDMA == m_nType)
		&& VALID_ITS(m_itsGoldenFork) && VALID_ITS(m_itsDeadFork)
		&& VALID_ITS(m_itsLong) && VALID_ITS(m_itsShort) );
}

void CMA::Clear( )
{
	CTechnique::Clear( );
}

int CMA::signal(int nIndex, uint32_t * pnCode)
{
	//           ;
	int	nSignal	=	GetForkSignal( nIndex, m_adwMADays, m_itsGoldenFork, m_itsDeadFork, pnCode );
	if( ITS_ISBUY(nSignal) || ITS_ISSELL(nSignal) )
		return nSignal;
	//    ;

	return GetTrendIntensity( nIndex, m_adwMADays, m_itsLong, m_itsShort, pnCode );
}

bool CMA::min_max_info(int nStart, int nEnd,
				   float *pdMin, float *pdMax )
{
	return AfxGetMinMaxInfo( nStart, nEnd, pdMin, pdMax, this, m_adwMADays );
}

/***
	   ֣ 
	1. MA
	    MA = n     ̼۵ ƽ  ֵ;
	2. EXPMA
	    EXPMA(1) = CLOSE(1);
		EXPMA(i) = (1-  )EXPMA(i-1) +   CLOSE(i);
		        = 2 / (n+1);
*/
bool CMA::calc( float * pValue, int nIndex, int nDays, bool bUseLast )
{
	STT_ASSERT_CALCULATE( m_pKData, nIndex, nDays );

	int	nCount	=	0;
	if( nDays > nIndex+1 )
		return false;

	float	dResult	=	0;
	int	k = 0;
	switch( m_nType )
	{
	case typeMA:
		return m_pKData->GetMA( pValue, nIndex, nDays );
		break;
	case typeEXPMA:
		if( bUseLast && pValue )
		{
			if( 0 == nIndex )
				dResult	=	m_pKData->MaindataAt(nIndex);
			else
				dResult	=	(*pValue)*(nDays-1)/(nDays+1) + m_pKData->MaindataAt(nIndex) * 2./(nDays+1);
		}
		else
		{
			for( k=0; k<=nIndex; k++ )
			{
				if( 0 == k )
					dResult	=	m_pKData->MaindataAt(k);
				else
					dResult	=	dResult*(nDays-1)/(nDays+1) + m_pKData->MaindataAt(k) * 2./(nDays+1);
			}
		}
		if( pValue )
			*pValue	=	dResult;
		break;
	case typeSMA:
	{
		if (m_dWeight > nDays)
		{
			return false;
		}
		if (bUseLast && pValue)
		{
			if (0 == nIndex)
				dResult = m_pKData->MaindataAt(nIndex);
			else
				dResult = (*pValue)*(nDays -m_dWeight) / nDays + m_pKData->MaindataAt(nIndex) * m_dWeight/nDays;
		}
		else
		{
			for (k = 0; k <= nIndex; k++)
			{
				if (0 == k)
					dResult = m_pKData->MaindataAt(k);
				else
					dResult = dResult*(nDays - m_dWeight) / nDays + m_pKData->MaindataAt(k) * m_dWeight / nDays;
			}
		}
		if (pValue)
			*pValue = dResult;
	}
	break;
	case typeDMA:
	{
		if (m_dWeight > 1)
		{
			return false;
		}
		if (bUseLast && pValue)
		{
			if (0 == nIndex)
				dResult = m_pKData->MaindataAt(nIndex);
			else
				dResult = (*pValue) * (1-m_dWeight) + m_pKData->MaindataAt(nIndex) * m_dWeight;
		}
		else
		{
			for (k = 0; k <= nIndex; k++)
			{
				if (0 == k)
					dResult = m_pKData->MaindataAt(k);
				else
					dResult = dResult*(1 - m_dWeight) + m_pKData->MaindataAt(k) * m_dWeight;
			}
		}
		if (pValue)
			*pValue = dResult;
	}
	break;
	default:
		assert( false );
	}
	return true;
}
