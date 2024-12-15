#ifndef _TECHLIB_H_
#define _TECHLIB_H_
#include <cassert>
#include <vector>
#include <cmath>
#include <stdint.h>

#include <./include/StructKlineView.h>

class	CTechnique;

/***
	一些宏定义，主要是技术指标计算中常用的一些代码;
*/

//assert( m_pKData && nStart >= 0 && nStart <(int) m_pKData->size() && nEnd >= 0 && nEnd < (int)m_pKData->size() );	
#define	STT_ASSERT_GETMINMAXINFO( m_pKData, nStart, nEnd )	\
    if(!m_pKData || nStart < 0 || nStart >=(int)m_pKData->size() || nEnd < 0 || nEnd >=(int)m_pKData->size() )							\
    return false;

//assert( m_pKData && nIndex >= 0 && nIndex < (int)m_pKData->size() && nDays > 0 );	
#define	STT_ASSERT_CALCULATE( m_pKData, nIndex, nDays )		\
    if( !m_pKData || nIndex < 0 || nIndex >=(int)m_pKData->size() || nDays <= 0 )			\
    return false;																	\
    if( !IsValidParameters( ) )															\
    SetDefaultParameters( );

//assert( m_pKData && nIndex >= 0 && nIndex < (int)m_pKData->size() );	
#define	STT_ASSERT_CALCULATE1( m_pKData, nIndex )			\
    if( !m_pKData || nIndex < 0 || nIndex >=(int)m_pKData->size() )			\
    return false;														\
    if( !IsValidParameters( ) )												\
    SetDefaultParameters( );


#define	VALID_DAYS( x )		(x>=1&&x<=1024)

#define	STT_VALID_DAYSARRAY( a )						\
if( a.size() <= 0 )								\
        return false;									\
    for( size_t k=0; k<a.size(); k++ )					\
    if( a[k] < 1 || a[k] > 1024 )					\
    return false;

#define	VALID_ITS( x )		(x>=ITS_MIN&&x<=ITS_MAX)


/***
	一些反复使用的代码，用函数代替，得到技术指标在指定范围内的最大最小值
*/


 extern bool AfxGetMinMaxInfo(int nStart, int nEnd, float *pdMin, float *pdMax, CTechnique * pTech, std::vector<uint32_t> & adwDays);	// 技术指标有不同日期作为参数
 extern bool AfxGetMinMaxInfo1(int nStart, int nEnd, float *pdMin, float *pdMax, CTechnique * pTech);	// 技术指标有1个最终值
 extern bool AfxGetMinMaxInfo2(int nStart, int nEnd, float *pdMin, float *pdMax, CTechnique * pTech);	// 技术指标有2个最终值
 extern bool AfxGetMinMaxInfo3(int nStart, int nEnd, float *pdMin, float *pdMax, CTechnique * pTech);	// 技术指标有3个最终值
 extern bool AfxGetMinMaxInfo4(int nStart, int nEnd, float *pdMin, float *pdMax, CTechnique * pTech);	// 技术指标有4个最终值
 extern bool AfxGetMinMaxInfo5(int nStart, int nEnd, float *pdMin, float *pdMax, CTechnique * pTech);	// 技术指标有5个最终值
 extern uint32_t AfxGetMaxDays(std::vector<uint32_t> & adw);	// 数组adw的最大值


enum	sg_drawtech	{
	drawtechtype_line	=	0x01,	// 技术曲线使用实线
	drawtechtype_dot	=	0x02,	// 技术曲线使用虚线
};
enum{
	strInstrumentID=0,
	strInstrumentName=1,
	strExchangeEName=2,
	strExchangeInstName=3,
	strExchangeName=4,
	strExchangeProperty=5,
	strm_time=6,
	strm_date=7,
	strProductEName=8,
	strProductClass=9,
	strDeliveryYear=10,
	strDeliveryMonth=11,
	strMaxMarketOrderVolume=12,
	strMinMarketOrderVolume=13,

	strBSRatio=124,//"委比" 
	strBSDiff=123,//"委差" 
	strSell5=122,//"卖⑤" 
	strSell4=121,//"卖④" 
	strSell3=120,//"卖③" 
	strSell2=119,//"卖②" 
	strSell1=118,//"卖①" 
	strBuy1=117,//"买①" 
	strBuy2=116,//"买②" 
	strBuy3=115,//"买③" 
	strBuy4=114,//"买④" 
	strBuy5=113,//"买⑤" 
	strPriceNow=112,//"成交" 
	strPriceAve=111,//"均价" 
	strPriceDiff=110,//"涨跌" 
	strPriceOpen=109,//"开盘" 
	strPriceDiffPercent=108,//"幅度" 
	strPriceHigh=107,//"最高" 
	strVolSum=106,//"总手" 
	strPriceLow=105,//"最低" 
	strVolNow=104,//"现手" 
	strVolRatio=103,//"量比" 
	strVolOuter=102,//"外盘" 
	strVolInner=101,//"内盘" 
	strAdvance=100, //涨家数
	strDecline=99,//跌家数
	strSortAsc=98,
	strSortDesc=97,
	strName=84,
	strPriceDetail=83,

	strNoInstrumentData=90,
	strTurnover=125,
	strVolume=126,
	strPrice=127,
	strRealTimetitle=128
};

#pragma pack(1)

typedef struct techuser_info_t{
	int m_nStructSize;
	char m_szName[64];
	char m_szShortName[16];
	int m_nLineCount;
	char m_aszLineName[5][8];
	int m_bAutoMA;
	int m_nMADays;
	int m_itsGoldenFork;
	int m_itsDeadFork;
}TECHUSER_INFO,*PTECHUSER_INFO;


//class InstrumentData;
//class InstrumentInfo;


typedef struct calculate_info_t{
	int m_nStructSize;
//	InstrumentData* m_pInstrument;
//	InstrumentInfo* m_pInstrumentInfo;
    CL::Kline* m_pKData;
	int m_nIndex;
	int m_bUseLast;

	double m_dValue1;
	double m_dValue2;
	double m_dValue3;
	double m_dValue4;
	double m_dValue5;
}CALCULATE_INFO,*PCALCULATE_INFO;

#pragma pack()
#endif


