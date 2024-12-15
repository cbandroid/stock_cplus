#ifndef _TECHNIQUE_H_
#define _TECHNIQUE_H_
//#include <PlatformStruct.h>
#include <vector>
#include <cassert>
#include <stdint.h>

#include "TechLib.h"
//#include <./include/StructKlineView.h>


/**
	技术指标基类;
*/
class KdataContainer;

class TechnicalIndicator
{
public:
	// Constructors
	TechnicalIndicator( );
	TechnicalIndicator(KdataContainer * pKData );
	virtual ~TechnicalIndicator();

	void set_kdata( KdataContainer  * pKData );	//	设定K线数据
	KdataContainer * get_kdata( );
	virtual	void	clear( );			//	清除
	virtual	void	clear_last_intensity( );	//	清除上次买卖信号
	virtual bool prepare_cache( size_t nStart, size_t nEnd, bool bForceRecalculate );	//	计算好数据，放入缓冲区

protected:
	bool load_from_cache( size_t nIndex, double * pValue1, double * pValue2 = NULL, double * pValue3 = NULL, double * pValue4 = NULL, double * pValue5 = NULL );	// 从缓冲区读入结果
	bool store_to_cache( size_t nIndex, double * pValue1, double * pValue2 = NULL, double * pValue3 = NULL, double * pValue4 = NULL, double * pValue5 = NULL );	// 将结果保存至缓冲区

	// Intensity Utility
    bool intensity_prepare( size_t nIndex, uint32_t * pnCode, size_t nMaxDays, size_t nMinMaxDays, double *pdLiminalLow, double *pdLiminalHigh, double dGateLow = 0.3, double dGateHigh = 0.7 );		// 准备数据，得到较大值和较小值
    bool	IntensityPreparePrice( size_t nIndex, uint32_t * pnCode, size_t nMaxDays, size_t nMinMaxDays, double *pdLiminalLow, double *pdLiminalHigh, double dGateLow = 0.3, double dGateHigh = 0.7 );	// 准备数据，得到价格较大值和较小值
    bool	IsNewValue( size_t nIndex, bool bTopOrBottom, size_t nDays, double * pdValue1, double * pdValue2 );	// 是否创新高或者创新低
	bool	is_deviate_on_bottom( size_t nIndex, double * pdValue1, double * pdValue2 );	// 是否底背离
	bool	is_deviate_on_top( size_t nIndex, double * pdValue1, double * pdValue2 );		// 是否顶背离
	bool	is_golden_fork( size_t nIndex, double * pdValue1, double * pdValue2 );		// 是否金叉
	bool	is_dead_fork( size_t nIndex, double * pdValue1, double * pdValue2 );			// 是否死叉


    int		GetForkSignal( size_t nIndex, std::vector<uint32_t> & adwDays, int itsGoldenFork, int itsDeadFork,  uint32_t * pnCode );	// 得到金叉或者死叉信号;
	int		GetForkSignal(size_t nIndex, int itsGoldenFork, int itsDeadFork, uint32_t *pnCode);		// 得到金叉或者死叉信号;


	int		GetForkSignal(size_t nIndex, double * pdValue1, double * pdValue2, int itsGoldenFork, int itsDeadFork, uint32_t *pnCode);	// 得到金叉死叉信号;
	int		GetTrendIntensity(size_t nIndex, std::vector<uint32_t> & adwDays, int itsLong, int itsShort, uint32_t * pnCode);	// 得到趋势信号;
	int		GetTrendIntensity1(size_t nIndex, int itsLong, int itsShort, uint32_t *pnCode);	// 得到趋势信号
	int		GetTrendIntensity2(size_t nIndex, int itsLong, int itsShort, uint32_t *pnCode);	// 得到趋势信号
	int		GetTrendIntensity(size_t nIndex, double * pdValue1, double * pdValue2, int itsLong, int itsShort, uint32_t *pnCode);	// 得到趋势信号;

public:
	// Attributes
	virtual	void	SetDefaultParameters( );		// 设定缺省参数
	void	attach( TechnicalIndicator & src );	// 指定参数与src相同
	virtual	bool	IsValidParameters( );			// 参数是否合法

	// Creations
	static	TechnicalIndicator *	create( uint32_t nTech, KdataContainer * pKData );	// 创建technical_indicator对象
	
	// Operations
	virtual int signal( size_t nIndex, uint32_t * pnCode = NULL );	// 得到买卖信号
    virtual int		intensity( size_t nIndex, uint32_t * pnCode = NULL, bool bTraceBack = true );	// 得到买卖趋势信号
    virtual int		intensity_traceback( size_t nIndex, uint32_t * pnCode = NULL );					// 得到买卖趋势信号
	virtual bool	min_max_info( size_t nStart, size_t nEnd, double *pdMin, double *pdMax );		// 得到最大最小值
	virtual	bool	calc( double * pValue, size_t nIndex, size_t nDays, bool bUseLast );			// 计算指标值
	virtual	bool	calc( double * pValue, size_t nIndex, bool bUseLast );		// 计算指标值
	virtual	bool	calc( double * pValue1, double * pValue2, size_t nIndex, bool bUseLast );		// 计算指标值
	virtual	bool	calc( double * pValue1, double * pValue2, double * pValue3, size_t nIndex, bool bUseLast );		// 计算指标值
	virtual	bool	calc( double * pValue1, double * pValue2, double * pValue3, double * pValue4, size_t nIndex, bool bUseLast );		// 计算指标值
	virtual	bool	calc( double * pValue1, double * pValue2, double * pValue3, double * pValue4, double * pValue5, size_t nIndex, bool bUseLast );		// 计算指标值
	virtual bool	calc_ma( double * pValue, double * pMA, size_t nIndex, bool bUseLast, size_t nMADays );		// 计算指标值和均值;

protected:
	KdataContainer*	m_pKData;

	double	*	m_pdCache1;
	double	*	m_pdCache2;
	double	*	m_pdCache3;
	double	*	m_pdCache4;
	double	*	m_pdCache5;
	bool	*	m_pbHasCache1;
	bool	*	m_pbHasCache2;
	bool	*	m_pbHasCache3;
	bool	*	m_pbHasCache4;
	bool	*	m_pbHasCache5;
	size_t			m_nLength;
	size_t			m_nIndexStart;

	int			m_nLastIntensity;
    uint32_t		m_nLastIntensityCode;
};

#endif
