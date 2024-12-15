#include "TechLib.h"
#include "Technique.h"
#include <math.h>

//////////////////////////////////////////////////////////////////
// GetMinMaxInfo
// 一些反复使用的代码，用函数代替，得到技术指标在指定范围内的最大最小值
// 技术指标有不同日期作为参数;
bool AfxGetMinMaxInfo(int nStart, int nEnd, float *pdMin,float *pdMax,CTechnique  * pTech, std::vector<uint32_t> & adwDays)
{
	assert( pTech );
	if( nStart < 0 || nEnd < 0 || nStart > nEnd || !pTech )
		return false;

	float	dMin = 0, dMax = 0;
	float	dValue = 0;
	bool	bFirst	=	true;
	for( size_t i=0; i<adwDays.size(); i++ )
	{
		for( int k=nStart; k<=nEnd; k++ )
		{
			if( pTech->calc( &dValue, k, adwDays[i], !bFirst ) )
			{
				if( bFirst || dValue < dMin )	dMin	=	dValue;
				if( bFirst || dValue > dMax )	dMax	=	dValue;
				bFirst	=	false;
			}
		}
	}
    //dMin	-=	fabs(dMin)*0.02f;
    //dMax	+=	fabs(dMax)*0.02f;
    if ((dMax - dMin) < 0.03f )
        dMax	=	dMin + 0.05f;
    if ( pdMin )		*pdMin	=	dMin;
    if ( pdMax )		*pdMax	=	dMax;
	
	return !bFirst;
}

// 技术指标有1个最终值
bool AfxGetMinMaxInfo1(int nStart, int nEnd,float *pdMin, float *pdMax, CTechnique* pTech)
{
	assert( pTech );
	if( nStart < 0 || nEnd < 0 || nStart > nEnd || !pTech )
		return false;

	float	dMin = 0, dMax = 0;
	float	dValue = 0;
	bool	bFirst	=	true;
	for (int k = nStart; k <= nEnd; k++)
	{
		if( pTech->calc( &dValue, k, !bFirst ) )	// 技术指标有1个最终值
		{
			if( bFirst || dValue < dMin )	dMin	=	dValue;
			if( bFirst || dValue > dMax )	dMax	=	dValue;
			bFirst	=	false;
		}
	}
    //dMin	-=	fabs(dMin)*0.02f;
    //dMax	+=	fabs(dMax)*0.02f;
    if ( (dMax - dMin) < 0.03f )
        dMax	=	dMin + 0.05f;
    if ( pdMin )		*pdMin	=	dMin;
    if ( pdMax )		*pdMax	=	dMax;

	return !bFirst;
}

// 技术指标有2个最终值
bool AfxGetMinMaxInfo2(int nStart,int nEnd, float *pdMin, float *pdMax, CTechnique * pTech)
{
	assert( pTech );
	if( nStart < 0 || nEnd < 0 || nStart > nEnd || !pTech )
		return false;

	float	dMin = 0, dMax = 0;
	float	dValue1 = 0, dValue2 = 0;
	bool	bFirst	=	true;
	for (int k = nStart; k <= nEnd; k++)
	{
		if( pTech->calc( &dValue1, &dValue2, k, !bFirst ) )	// 技术指标有2个最终值
		{
			if( bFirst || dValue1 < dMin )	dMin	=	dValue1;
			if( bFirst || dValue1 > dMax )	dMax	=	dValue1;
			if( dValue2 < dMin )	dMin	=	dValue2;
			if( dValue2 > dMax )	dMax	=	dValue2;
			bFirst	=	false;
		}
	}
    //dMin	-=	fabs(dMin)*0.02f;
    //dMax	+=	fabs(dMax)*0.02f;
    if ((dMax - dMin) < 0.03f )
        dMax	=	dMin + 0.05f;
    if ( pdMin )		*pdMin	=	dMin;
    if ( pdMax )		*pdMax	=	dMax;
	
	return !bFirst;
}

// 技术指标有3个最终值
bool AfxGetMinMaxInfo3(int nStart, int nEnd, float *pdMin, float*pdMax, CTechnique* pTech)
{
	assert( pTech );
	if( nStart < 0 || nEnd < 0 || nStart > nEnd || !pTech )
		return false;

	float	dMin = 0, dMax = 0;
	float	dValue1 = 0, dValue2 = 0, dValue3 = 0;
	bool	bFirst	=	true;
	for (int k = nStart; k <= nEnd; k++)
	{
		if( pTech->calc( &dValue1, &dValue2, &dValue3, k, !bFirst ) )	// 技术指标有3个最终值
		{
			if( bFirst || dValue1 < dMin )	dMin	=	dValue1;
			if( bFirst || dValue1 > dMax )	dMax	=	dValue1;
			if( dValue2 < dMin )	dMin	=	dValue2;
			if( dValue2 > dMax )	dMax	=	dValue2;
			if( dValue3 < dMin )	dMin	=	dValue3;
			if( dValue3 > dMax )	dMax	=	dValue3;
			bFirst	=	false;
		}
	}
    //dMin	-=	fabs(dMin)*0.02;
    //dMax	+=	fabs(dMax)*0.02;
    if ((dMax - dMin) < 0.03f )
        dMax	=	dMin + 0.05f;
    if ( pdMin )		*pdMin	=	dMin;
    if ( pdMax )		*pdMax	=	dMax;

	return !bFirst;
}

// 技术指标有4个最终值
bool AfxGetMinMaxInfo4(int nStart, int nEnd, float *pdMin,float *pdMax, CTechnique * pTech)
{
	assert( pTech );
	if( nStart < 0 || nEnd < 0 || nStart > nEnd || !pTech )
		return false;

	float	dMin = 0, dMax = 0;
	float	dValue1 = 0, dValue2 = 0, dValue3 = 0, dValue4 = 0;
	bool	bFirst	=	true;
	for (int k = nStart; k <= nEnd; k++)
	{
		if( pTech->calc( &dValue1, &dValue2, &dValue3, &dValue4, k, !bFirst ) )	// 技术指标有4个最终值
		{
			if( bFirst || dValue1 < dMin )	dMin	=	dValue1;
			if( bFirst || dValue1 > dMax )	dMax	=	dValue1;
			if( dValue2 < dMin )	dMin	=	dValue2;
			if( dValue2 > dMax )	dMax	=	dValue2;
			if( dValue3 < dMin )	dMin	=	dValue3;
			if( dValue3 > dMax )	dMax	=	dValue3;
			if( dValue4 < dMin )	dMin	=	dValue4;
			if( dValue4 > dMax )	dMax	=	dValue4;
			bFirst	=	false;
		}
	}
    //dMin	-=	fabs(dMin)*0.02f;
    //dMax	+=	fabs(dMax)*0.02f;
    if ((dMax - dMin) < 0.03f )
        dMax	=	dMin + 0.05f;
    if (pdMin )		*pdMin	=	dMin;
    if (pdMax )		*pdMax	=	dMax;

	return !bFirst;
}

// 技术指标有5个最终值
bool AfxGetMinMaxInfo5(int nStart, int nEnd, float *pdMin, float *pdMax,CTechnique * pTech)
{
	assert( pTech );
	if( nStart < 0 || nEnd < 0 || nStart > nEnd || !pTech )
		return false;

	float	dMin = 0, dMax = 0;
	float	dValue1 = 0, dValue2 = 0, dValue3 = 0, dValue4 = 0, dValue5 = 0;
	bool	bFirst	=	true;
	for( int k=nStart; k<=nEnd; k++ )
	{
		if( pTech->calc( &dValue1, &dValue2, &dValue3, &dValue4, &dValue5, k, !bFirst ) )	// 技术指标有5个最终值
		{
			if( bFirst || dValue1 < dMin )	dMin	=	dValue1;
			if( bFirst || dValue1 > dMax )	dMax	=	dValue1;
			if( dValue2 < dMin )	dMin	=	dValue2;
			if( dValue2 > dMax )	dMax	=	dValue2;
			if( dValue3 < dMin )	dMin	=	dValue3;
			if( dValue3 > dMax )	dMax	=	dValue3;
			if( dValue4 < dMin )	dMin	=	dValue4;
			if( dValue4 > dMax )	dMax	=	dValue4;
			if( dValue5 < dMin )	dMin	=	dValue5;
			if( dValue5 > dMax )	dMax	=	dValue5;
			bFirst	=	false;
		}
	}
    //dMin	-=	fabs(dMin)*0.02f;
    //dMax	+=	fabs(dMax)*0.02f;
    if ((dMax - dMin)< 0.03f )
        dMax	=	dMin + 0.05f;
    if ( pdMin )		*pdMin	=	dMin;
    if ( pdMax )		*pdMax	=	dMax;

	return !bFirst;
}
#if 0
uint32_t	AfxGetMaxDays(std::vector<uint32_t> & adw)
{
	uint32_t	dwMax = 0;
	for (size_t i = 0; i<adw.size(); ++i)
	{
		if( adw[i] > dwMax )
			dwMax	=	adw[i];
	}
	return dwMax;
}
#endif


