#include "TechLib.h"
#include "KData.h"
#include <math.h>

#include "BOLL.h"
//////////////////////////////////////////////////////////////////////
//	CBOLL
CBOLL::CBOLL( )
{
   CBOLL::SetDefaultParameters( );
}

CBOLL::CBOLL( CKData * pKData )
: CTechnique( pKData )
{
   CBOLL::SetDefaultParameters( );
}

CBOLL::~CBOLL()
{
   CBOLL::Clear( );
}

void CBOLL::SetDefaultParameters( )
{
	m_dMultiUp		=	2;
	m_dMultiDown	=	2;
	m_nMADays		=	20;
	m_itsSupport	=	ITS_BUY;
	m_itsResistance	=	ITS_SELL;
}

void CBOLL::attach( CBOLL & src )
{
	m_dMultiUp		=	src.m_dMultiUp;
	m_dMultiDown	=	src.m_dMultiDown;
	m_nMADays		=	src.m_nMADays;
	m_itsSupport	=	src.m_itsSupport;
	m_itsResistance	=	src.m_itsResistance;
}

bool CBOLL::IsValidParameters( )
{
	return ( m_dMultiUp > 0 && m_dMultiDown > 0 && VALID_DAYS( m_nMADays )
		&& VALID_ITS(m_itsSupport) && VALID_ITS(m_itsResistance) );
}

void CBOLL::Clear( )
{
	CTechnique::Clear( );
}

int CBOLL::signal( int nIndex, uint32_t * pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;

	float	dMA = 0, dUp = 0, dDown = 0;
	if( !calc( &dMA, &dUp, &dDown, nIndex, false ) )
		return ITS_NOTHING;

	float	dClose	=	m_pKData->at(nIndex).close;

	if( dClose < dDown )
	{	//     ֧  λ;
		if( pnCode )	*pnCode	=	ITSC_SUPPORT;
		return m_itsSupport;
	}
	if( dClose > dUp )
	{	//  ǹ     λ;
		if( pnCode )	*pnCode	=	ITSC_RESISTANCE;
		return m_itsResistance;
	}
	return	ITS_NOTHING;
}

bool CBOLL::min_max_info(int nStart, int nEnd,
						  float *pdMin, float *pdMax )
{
	return AfxGetMinMaxInfo3( nStart, nEnd, pdMin, pdMax, this );
}

/***
   ִ    Թɼ ƽ    MAΪ     ߣ  Ϸ       MA+  Sn   · ֧    MA-  Sn֮  Ĵ ״    
     SnΪn     ̼۵ı ׼  
*/
bool CBOLL::calc( float * pdMA, float * pdUp, float * pdDown, int nIndex, bool bUseLast )
{
	STT_ASSERT_CALCULATE1( m_pKData, nIndex );

	if( m_nMADays < 2 )
		return false;

	if( load_from_cache( nIndex, pdMA, pdUp, pdDown ) )
		return true;

	float	dMA = 0, dUp = 0, dDown = 0, dS = 0;

	if( !m_pKData->GetMA( &dMA, nIndex, m_nMADays ) )
		return false;

	int	nCount	=	0;
	for( int k=nIndex; k>=0; k-- )
	{
		dS	+=	(m_pKData->MaindataAt(k) - dMA) * (m_pKData->MaindataAt(k) - dMA);
		nCount	++;
		if( nCount == m_nMADays )
			break;
	}

	dS	=	sqrt( dS / (m_nMADays-1) );

	dUp		=	dMA + m_dMultiUp * dS;
	dDown	=	dMA - m_dMultiDown * dS;
	if( pdMA )		*pdMA	=	dMA;
	if( pdUp )		*pdUp	=	dUp;
	if( pdDown )	*pdDown	=	dDown;

	store_to_cache( nIndex, pdMA, pdUp, pdDown );
	return true;
}
