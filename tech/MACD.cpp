#include "MACD.h"
#include <math.h>
#include "TechLib.h"
#include "KData.h"
//////////////////////////////////////////////////////////////////////
//	CMACD
CMACD::CMACD( )
{
    CMACD::SetDefaultParameters( );
}

CMACD::CMACD( CKData * pKData )
	: CTechnique( pKData )
{
    CMACD::SetDefaultParameters( );
}

CMACD::~CMACD()
{
    CMACD::Clear( );
}

void CMACD::SetDefaultParameters( )
{
	m_nEMA1Days	=	12;
	m_nEMA2Days	=	26;
	m_nDIFDays	=	9;

	m_itsDeviateOnBottom	=	ITS_BUYINTENSE;
	m_itsDeviateOnTop		=	ITS_SELLINTENSE;
	m_itsGoldenFork			=	ITS_BUY;
	m_itsDeadFork			=	ITS_SELL;
}

void CMACD::attach( CMACD & src )
{
	m_nEMA1Days	=	src.m_nEMA1Days;
	m_nEMA2Days	=	src.m_nEMA2Days;
	m_nDIFDays	=	src.m_nDIFDays;

	m_itsDeviateOnBottom	=	src.m_itsDeviateOnBottom;
	m_itsDeviateOnTop		=	src.m_itsDeviateOnTop;
	m_itsGoldenFork			=	src.m_itsGoldenFork;
	m_itsDeadFork			=	src.m_itsDeadFork;
}

bool CMACD::IsValidParameters( )
{
	return ( VALID_DAYS(m_nEMA1Days) && VALID_DAYS(m_nEMA2Days) && VALID_DAYS(m_nDIFDays)
		&& VALID_ITS(m_itsDeviateOnBottom) && VALID_ITS(m_itsDeviateOnTop)
		&& VALID_ITS(m_itsGoldenFork) && VALID_ITS(m_itsDeadFork) );
}

void CMACD::Clear( )
{
	 CTechnique::Clear( );
}

int CMACD::signal( int nIndex, uint32_t * pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
	prepare_cache( 0, -1, false );

	int	nMaxDays	=	max( max(m_nEMA1Days,m_nEMA2Days) , m_nDIFDays );
	float	dLiminalLow = 0, dLiminalHigh = 0;
	if( !intensity_prepare( nIndex, pnCode, nMaxDays, ITS_GETMINMAXDAYRANGE, &dLiminalLow, &dLiminalHigh, 0.309, 0.682 ) )
		return ITS_NOTHING;

	float	dEMA1, dEMA2, dDIF, dDEA;
	if( !calc( &dEMA1, &dEMA2, &dDIF, &dDEA, nIndex, false ) )
		return ITS_NOTHING;

	if( is_deviate_on_bottom( nIndex, m_pdCache3, m_pdCache4 ) )
	{	//  ױ   
		if( pnCode )	*pnCode	=	ITSC_DEVIATEONBOTTOM;
		return m_itsDeviateOnBottom;
	}
	if( is_deviate_on_top( nIndex, m_pdCache3, m_pdCache4 ) )
	{	//       
		if( pnCode )	*pnCode	=	ITSC_DEVIATEONTOP;
		return m_itsDeviateOnTop;
	}
	if( dDIF < dLiminalLow && dDEA < dLiminalLow && is_golden_fork( nIndex, m_pdCache3, m_pdCache4 ) )
	{	//   λ   
		if( pnCode )	*pnCode	=	ITSC_GOLDENFORK;
		return m_itsGoldenFork;
	}
	if( dDIF > dLiminalHigh && dDEA > dLiminalHigh && is_dead_fork( nIndex, m_pdCache3, m_pdCache4 ) )
	{	//   λ    
		if( pnCode )	*pnCode	=	ITSC_DEADFORK;
		return m_itsDeadFork;
	}
	if( dDIF < dLiminalLow && dDEA < dLiminalLow )
	{	//   λ
		if( pnCode )	*pnCode	=	ITSC_GOLDENFORK;
		return m_itsGoldenFork;
	}
	if( dDIF > dLiminalHigh && dDEA > dLiminalHigh )
	{	//   λ
		if( pnCode )	*pnCode	=	ITSC_DEADFORK;
		return m_itsDeadFork;
	}

	return	ITS_NOTHING;
}

/***
	EMA  =      ƶ   ֵ
	EMA2 =      ƶ   ֵ
	DIF  =      ƶ   ֵ -      ƶ   ֵ
	DEA  = DIF   ƶ ƽ  ֵ
	  ״  ֵ = DIF - DEA
*/
bool CMACD::min_max_info(int nStart, int nEnd, float *pdMin, float *pdMax )
{
	STT_ASSERT_GETMINMAXINFO( m_pKData, nStart, nEnd );

	float	dMin = 0, dMax = 0;
	//ǰһ յ       Ϊ0
	float	dEMA1 = 0, dEMA2 = 0, dDIF = 0, dDEA = 0;
	bool	bFirst	=	true;
	for( int k=nStart; k<=nEnd; k++ )
	{
		if( calc( &dEMA1, &dEMA2, &dDIF, &dDEA, k, !bFirst ) )
		{
			if( bFirst )	{	dMin	=	dDIF;	dMax	=	dDIF;	}
			
			if( dDIF < dMin )	dMin	=	dDIF;
			if( dDEA < dMin )	dMin	=	dDEA;
			if( 2*(dDIF-dDEA) < dMin )	dMin	=	2*(dDIF-dDEA); // WARNING: different

			if( dDIF > dMax )	dMax	=	dDIF;
			if( dDEA > dMax )	dMax	=	dDEA;
			if( 2*(dDIF-dDEA) > dMax )	dMax	=	2*(dDIF-dDEA); // WARNING: different
			bFirst	=	false;
		}
	}
	dMin	-=	fabs(dMin)*0.02;
	dMax	+=	fabs(dMax)*0.02;
	if( fabs(dMin) < 1e-4 )
		dMin	=	-0.01;
	if( fabs(dMax) < 1e-4 )
		dMax	=	0.01;
	if( dMax - dMin < 0.03 )
		dMax	=	dMin + 0.05;

	if( pdMin )		*pdMin	=	dMin;
	if( pdMax )		*pdMax	=	dMax;
	return true;
}

bool CMACD::calc( float *pdEMA1, float *pdEMA2, float *pdDIF, float *pdDEA,
					int nIndex, bool bUseLast )
{
	STT_ASSERT_CALCULATE1( m_pKData, nIndex );

	if( m_nEMA1Days > nIndex+1 || m_nEMA2Days > nIndex+1 || m_nDIFDays > nIndex+1 )
		return false;

	if( load_from_cache( nIndex, pdEMA1, pdEMA2, pdDIF, pdDEA ) )
		return true;

	// Calculate EMA1, EMA2, DIF, DEA
	//EMA1 Ĳ   Ϊ12  ,EMA2 Ĳ   Ϊ26  
	//12     ̼۵ EMA ļ  㹫ʽΪ:
	//EMA(12)=ǰһ  EMA(12)*(12-1)/(12+1)+       ̼ *2/(12+1)
	//26  EMA    ʽΪ
	//EMA  26  =ǰһ  EMA  26    25/27         ̼ۡ 2/27
	//       ֵ  DIF  
	//DIF=    EMA  12        EMA  26  
	//    MACD
	//    DEA  MACD  =ǰһ  DEA  8/10      DIF  2/10
	float	dEMA1New = 0, dEMA2New = 0, dDIFNew = 0, dDEANew = 0;
	if( bUseLast && pdEMA1 && pdEMA2 && pdDEA )
	{
		dEMA1New	=	(*pdEMA1)*(m_nEMA1Days-1)/(m_nEMA1Days+1) + 2 * m_pKData->MaindataAt(nIndex) /(m_nEMA1Days+1);
		dEMA2New	=	(*pdEMA2)*(m_nEMA2Days-1)/(m_nEMA2Days+1) + 2 * m_pKData->MaindataAt(nIndex) /(m_nEMA2Days+1);
		dDIFNew		=	dEMA1New-dEMA2New;
		dDEANew		=	(*pdDEA)*(m_nDIFDays-1)/(m_nDIFDays+1) + 2 * dDIFNew/(m_nDIFDays+1);
	}
	else
	{
		float	factor1 = 1, factor2 = 1;
		int k;
		for( k=nIndex; k > 0; k-- )
		{
			factor1		*=	((double)(m_nEMA1Days-1))/(m_nEMA1Days+1);
			factor2		*=	((double)(m_nEMA2Days-1))/(m_nEMA2Days+1);
			if( factor1 < 0.001 && factor2 < 0.001 )
				break;
		}
		dEMA1New	=	m_pKData->MaindataAt(k);
		dEMA2New	=	m_pKData->MaindataAt(k);
		dDIFNew		=	dEMA1New - dEMA2New;
		dDEANew		=	dDIFNew;
		for( ; k<=nIndex; k++ )
		{
			dEMA1New	=	dEMA1New * (m_nEMA1Days-1)/(m_nEMA1Days+1) + 2 * m_pKData->MaindataAt(k) /(m_nEMA1Days+1);
			dEMA2New	=	dEMA2New * (m_nEMA2Days-1)/(m_nEMA2Days+1) + 2 * m_pKData->MaindataAt(k) /(m_nEMA2Days+1);
			dDIFNew		=	dEMA1New - dEMA2New;
			dDEANew		=	dDEANew * (m_nDIFDays-1)/(m_nDIFDays+1) + 2 * dDIFNew / (m_nDIFDays+1);
		}
	}

	if( pdEMA1 )		*pdEMA1	=	dEMA1New;
	if( pdEMA2 )		*pdEMA2	=	dEMA2New;
	if( pdDIF )			*pdDIF	=	dDIFNew;
	if( pdDEA )			*pdDEA	=	dDEANew;
	store_to_cache( nIndex, pdEMA1, pdEMA2, pdDIF, pdDEA );
	return true;
}
