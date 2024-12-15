#include "VOLUME.h"
#include "TechLib.h"
#include "KData.h"


//////////////////////////////////////////////////////////////////////
//	CVOLUME
CVOLUME::CVOLUME( )
{
    CVOLUME::SetDefaultParameters( );
}

CVOLUME::CVOLUME( CKData* pKData )
    : CTechnique( pKData )
{
    CVOLUME::SetDefaultParameters( );
}

CVOLUME::~CVOLUME()
{
    CVOLUME::Clear( );
}

void CVOLUME::SetDefaultParameters( )
{
	m_adwMADays.clear();
	m_adwMADays.push_back( 5 );
	m_adwMADays.push_back( 10 );
	m_itsDeviateOnBottom	=	ITS_BUYINTENSE;
	m_itsDeviateOnTop		=	ITS_SELLINTENSE;
	m_itsLong				=	ITS_BUY;
	m_itsShort				=	ITS_SELL;
}

void CVOLUME::attach( CVOLUME & src )
{
	//m_adwMADays.Copy( src.m_adwMADays );
	//std::copy(m_adwMADays.begin(),m_adwMADays.end(),src.m_adwMADays);
	m_adwMADays=src.m_adwMADays;
	m_itsDeviateOnBottom	=	src.m_itsDeviateOnBottom;
	m_itsDeviateOnTop		=	src.m_itsDeviateOnTop;
	m_itsLong				=	src.m_itsLong;
	m_itsShort				=	src.m_itsShort;
}

bool CVOLUME::IsValidParameters( )
{
	STT_VALID_DAYSARRAY( m_adwMADays );
	return ( VALID_ITS(m_itsDeviateOnBottom) && VALID_ITS(m_itsDeviateOnTop)
		&& VALID_ITS(m_itsLong) && VALID_ITS(m_itsShort) );
}

void CVOLUME::Clear( )
{
    CTechnique::Clear( );
}

int CVOLUME::signal( int nIndex, uint32_t * pnCode )
{
	if( pnCode )	*pnCode	=	ITSC_NOTHING;
    if( !m_pKData || nIndex < 0 || nIndex >= (int)m_pKData->size() )
		return ITS_NOTHING;

	int	nIntensity	=	GetTrendIntensity( nIndex, m_adwMADays, m_itsLong, m_itsShort, pnCode );
	if( ITS_BUY == nIntensity
		&& m_pKData->IsNewValue( nIndex, false, ITS_DAYS_DEVIATE ) )
	{	//  ױ  룬 ɼ۴  µͲ  ҳɽ           
		if( pnCode )	*pnCode	=	ITSC_DEVIATEONBOTTOM;
		return m_itsDeviateOnBottom;
	}
	if( ITS_SELL == nIntensity
		&& m_pKData->IsNewValue( nIndex, true, ITS_DAYS_DEVIATE ) )
	{	//      룬 ɼ۴  ¸߲  ҳɽ           
		if( pnCode )	*pnCode	=	ITSC_DEVIATEONTOP;
		return m_itsDeviateOnTop;
	}

	return nIntensity;
}

bool CVOLUME::min_max_info(int nStart, int nEnd, float *pdMin, float *pdMax )
{
	STT_ASSERT_GETMINMAXINFO( m_pKData, nStart, nEnd );

    float	dMin	=	0;
    float	dMax	=	1;
    for( int k=nStart; k<=nEnd; k++ )
	{
        CL::Kline	& kd	=	m_pKData->at(k);
        if( dMax < kd.vol )
            dMax	=	(double)kd.vol;
	}

	dMax	=	dMax + 1;
	if( dMax - dMin < 3 )
		dMax	=	dMin + 3;

	if( pdMin )		*pdMin	=	dMin;

	if( pdMax )		*pdMax	=	dMax;
	return true;
}

/***
	    nDays  ƽ   ɽ   
*/
bool CVOLUME::calc( float * pValue, int nIndex, int nDays, bool bUseLast )
{
	STT_ASSERT_CALCULATE( m_pKData, nIndex, nDays );

	int	nCount	=	0;
	if( nDays > nIndex+1 )
		return false;
    float	dResult	=	0;
	for( int k=nIndex; k>=0; k-- )
	{
        dResult	+=	m_pKData->at(k).vol;
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
