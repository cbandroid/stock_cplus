
#if	!defined( __STKLIB_TECHNIQUE_H__ )

#define	__STKLIB_TECHNIQUE_H__

#include <QList>
#include <QString>
#include "KData.h"

/* The Folowing Is Stock Technique Analysis Identify Number */
/* 技术指标唯一编号 */
#define	STT_MIN			1
//	K线属性类
#define	STT_KLINE_MIN	1
#define	STT_MA			1		//	移动平均线MA
#define STT_EXPMA       2      //  平滑移动平均线
#define	STT_BBI			3		//	多空指标BBI
#define	STT_BOLL		4		//	布林带BOLL
#define	STT_PV			5		//	汇证均价线P/V
#define	STT_SAR			6		//	停损点转向指标SAR
#define	STT_DJ			7		//	叠加图DJ
#define	STT_CW			8		//	筹码分布图CW
#define	STT_KLINE_MAX	8

//	趋势类
#define	STT_TREND_MIN	9
#define	STT_MACD		9		//	指数平滑异同移动平均线MACD
#define	STT_MIKE		10		//	麦克指标MIKE
#define	STT_PSY			11		//	心理线PSY
#define	STT_TREND_MAX	11

//	能量类
#define	STT_ENERGY_MIN	12
#define	STT_VOLUME		12		//	成交量
#define	STT_NVI			13		//	负成交量指标NVI
#define	STT_PVI			14		//	正成交量指标PVI
#define	STT_VR			15		//	容量比率VR
#define	STT_VROC		16		//	成交量变动率指标VROC
#define	STT_OBV			17		//	能量线OBV
#define	STT_MOBV		18		//	主力进出MOBV
#define	STT_MFI			19		//	资金流动指数MFI
#define	STT_VMACD		20		//	量指数平滑异同移动平均线VMACD
#define	STT_WVAD		21		//	威廉变异离散量WVAD
#define	STT_EMV			22		//	简易波动指标EMV
#define	STT_VRSI		23		//	量相对强弱指标VRSI
#define	STT_NVRSI		24		//	新量相对强弱指标VRSI
#define	STT_AD			25		//	聚/散指标A/D
#define	STT_CI			26		//	累积指数CI
#define	STT_ENERGY_MAX	26

//	摆动类
#define	STT_SWING_MIN	27
#define	STT_KDJ			27		//	随机指标KDJ
#define	STT_R			28		//	威廉指标R
#define	STT_RSI			29		//	相对强弱指标
#define	STT_BIAS		30		//	乖离率BIAS
#define	STT_MTM			31		//	动量指标MTM
#define	STT_DMI			32		//	动向指标DMI
#define	STT_ROC			33		//	变动率指标ROC
#define	STT_CCI			34		//	顺势通道指标CCI
#define	STT_CV			35		//	佳庆指标CV
#define	STT_ARBR		36		//	人气意愿指标ARBR
#define	STT_CR			37		//	中间意愿指标CR
#define	STT_OSC			38		//	摆动量OSC
#define	STT_UOS			39		//	极摆动指标UOS
#define	STT_MAOSC		40		//	移动平均摆动量MAOSC
#define	STT_36BIAS		41		//	3减6日乖离3-6BIAS
#define	STT_DPO			42		//	去势价格摆动指数DPO
#define	STT_KST			43		//	确然指标KST
#define	STT_SKDJ	    44      // 慢速随机指标
#define	STT_SWING_MAX	44

//	其他类
#define	STT_OTHER_MIN	45
#define	STT_REI			45		//	范围膨胀指数REI
#define	STT_DMKI		46		//	迪马克尔指标DMKI
#define	STT_PCNT		47		//	幅度比PCNT
#define	STT_HLC			48		//	幅度分析HLC
#define	STT_CDP			49		//	逆势操作指标CDP
#define	STT_ASI			50		//	累积摆动指标ASI
#define	STT_ATR			51		//	均幅指标ATR
#define	STT_OTHER_MAX	51

//#ifdef  _ACTIVE_STT_USER

  #define	STT_CLK_MIN		52

  #define	STT_CYO			52		//	周期摆动量CYO
  #define	STT_DCYO		53		//	去噪周期摆动量DCYO
  #define	STT_HSL			54		//	相对换手率HSL
  #define	STT_DPER		55		//	去势百分比指标DPER

  #define	STT_CLK_MAX		STT_DPER    // 53

  #define	STT_MAX			STT_CLK_MAX 

//#else
//  #define	STT_MAX			STT_OTHER_MAX // 53
//#endif


#define	STT_USER_MIN	10000

#define	STT_INVALID		-1

#define  CROSS_NONE         0
#define  CROSS_UP           1
#define  CROSS_DOWN         2

//#define  _TECH_LIMIT_NONE_
#define  _TECH_CALC_LAST1_
#define  _TECH_CALC_LAST2_

#define   TECH_DATA_USEDLAST        false
#define   TECH_DATA_UNUSEDLAST      false

// 指标类别
#define	STT_CLASS_MIN		1
#define	STT_CLASS_KLINE		1	// K线叠加类
#define	STT_CLASS_TREND		2	// 趋势类
#define	STT_CLASS_ENERGY	3	// 能量类
#define	STT_CLASS_SWING		4	// 摆动类
#define	STT_CLASS_OTHER		5	// 其它类
#define	STT_CLASS_CLK		6	// 策略为王类
#define	STT_CLASS_USER		7	// 外挂指标类
#define	STT_CLASS_MAX		7

// 买卖信号
#define	ITS_MIN				-15
#define	ITS_SELLINTENSE		-15	//	强烈卖出
#define	ITSG_SELLINTENSE	-13	//	门限
#define	ITS_SELL			-10	//	卖出
#define	ITSG_SELL			-8	//	门限
#define	ITS_SELLFEEBLE		-5	//	微弱卖出
#define	ITSG_SELLFEEBLE		-3	//	门限
#define	ITS_NOTHING			0	//	无
#define	ITSG_BUYFEEBLE		3	//	门限
#define	ITS_BUYFEEBLE		5	//	微弱买进
#define	ITSG_BUY			8	//	门限
#define	ITS_BUY				10	//	买进
#define	ITSG_BUYINTENSE		13	//	门限
#define	ITS_BUYINTENSE		15	//	强烈买进
#define	ITS_MAX				15

// 买卖信号理由
#define	ITSC_MIN				1
#define	ITSC_NOTHING			1	//	无
#define	ITSC_DEVIATEONBOTTOM	2	//	底背离
#define	ITSC_DEVIATEONTOP		3	//	顶背离
#define	ITSC_GOLDENFORK			4	//	金叉
#define	ITSC_DEADFORK			5	//	死叉
#define	ITSC_LONG				6	//	多头
#define	ITSC_SHORT				7	//	空头
#define	ITSC_OVERBOUGHT			8	//	超买
#define	ITSC_OVERSOLD			9	//	超卖
#define	ITSC_SUPPORT			10	//	支撑位
#define	ITSC_RESISTANCE			11	//	阻力位
#define	ITSC_MAX				11

#define	ITS_ISSELL(x)			(x<0)
#define	ITS_ISBUY(x)			(x>0)
#define	ITS_ISSELLORNOTHING(x)	(x<=0)
#define	ITS_ISBUYORNOTHING(x)	(x>=0)

#define	ITS_GETMINMAXDAYRANGE	120
#define	ITS_DAYS_DEVIATE		115

/**
	技术指标基类
*/
class CTechnique
{
public:
	// Constructors
	CTechnique( );
	CTechnique( CKData * pKData );
	virtual ~CTechnique();

	void set_kdata( CKData * pKData );	//	设定K线数据
	CKData * get_kdata( );
	virtual	void	Clear( );			//	清除
	virtual	void	clear_last_intensity( );	//	清除上次买卖信号
	virtual	bool prepare_cache( int nStart, int nEnd, bool bForceRecalculate );	//	计算好数据，放入缓冲区

protected:
	bool	load_from_cache( int nIndex, float * pValue1,
                float * pValue2 = nullptr, float * pValue3 = nullptr, float * pValue4 = nullptr, float * pValue5 = nullptr );	// 从缓冲区读入结果
	bool	store_to_cache( int nIndex, float * pValue1,
                float * pValue2 = nullptr, float * pValue3 = nullptr, float * pValue4 = nullptr, float * pValue5 = nullptr );	// 将结果保存至缓冲区

	// Intensity Utility
	bool	intensity_prepare(int nIndex, uint32_t * pnCode, int nMaxDays,int nMinMaxDays, float *pdLiminalLow, float *pdLiminalHigh, float dGateLow = 0.3, float dGateHigh = 0.7 );		// 准备数据，得到较大值和较小值
	bool	IntensityPreparePrice( int nIndex, uint32_t * pnCode, int nMaxDays, int nMinMaxDays, float *pdLiminalLow, float *pdLiminalHigh, float dGateLow = 0.3, float dGateHigh = 0.7 );	// 准备数据，得到价格较大值和较小值
	bool	IsNewValue(int nIndex, bool bTopOrBottom, int nDays, float * pdValue1, float * pdValue2 );	// 是否创新高或者创新低
	bool	is_deviate_on_bottom( int nIndex, float * pdValue1, float * pdValue2 );	// 是否底背离
	bool	is_deviate_on_top(int nIndex, float * pdValue1, float * pdValue2 );		// 是否顶背离
	bool	is_golden_fork(int nIndex, float * pdValue1, float * pdValue2 );		// 是否金叉
	bool	is_dead_fork(int nIndex, float * pdValue1, float * pdValue2 );			// 是否死叉
	int		GetForkSignal( int nIndex, std::vector<uint32_t> & adwDays, int itsGoldenFork, int itsDeadFork, uint32_t* pnCode );	// 得到金叉或者死叉信号
	int		GetForkSignal( int nIndex, int itsGoldenFork, int itsDeadFork, uint32_t *pnCode );		// 得到金叉或者死叉信号
	int		GetForkSignal(int nIndex, float * pdValue1, float * pdValue2, int itsGoldenFork, int itsDeadFork,uint32_t *pnCode );	// 得到金叉死叉信号
	int		GetTrendIntensity(int nIndex, std::vector<uint32_t> & adwDays, int itsLong, int itsShort, uint32_t * pnCode );	// 得到趋势信号
	int		GetTrendIntensity1(int nIndex, int itsLong, int itsShort, uint32_t *pnCode );	// 得到趋势信号
	int		GetTrendIntensity2(int nIndex, int itsLong, int itsShort, uint32_t *pnCode );	// 得到趋势信号
	int		GetTrendIntensity(int nIndex, float * pdValue1, float * pdValue2, int itsLong, int itsShort, uint32_t *pnCode );	// 得到趋势信号
public:
	// Attributes
	virtual	void	SetDefaultParameters( );		// 设定缺省参数
	void	attach( CTechnique & src );	// 指定参数与src相同
	virtual	bool	IsValidParameters( );			// 参数是否合法

	// Creations
	static	CTechnique *create( int nTech, CKData * pKData );	// 创建CTechnique对象
	
	// Operations
    virtual	int		signal( int nIndex, uint32_t * pnCode = nullptr );	// 得到买卖信号
    virtual	int		intensity(int nIndex, uint32_t * pnCode = nullptr, bool bTraceBack = true );	// 得到买卖趋势信号
    virtual	int		intensity_traceback( int nIndex, uint32_t * pnCode = nullptr );					// 得到买卖趋势信号
	virtual	bool	min_max_info( int nStart, int nEnd, float *pdMin, float *pdMax );		// 得到最大最小值
	virtual	bool	calc( float * pValue,int nIndex, int nDays, bool bUseLast );			// 计算指标值
	virtual	bool	calc( float * pValue, int nIndex, bool bUseLast );		// 计算指标值
	virtual	bool	calc( float * pValue1, float * pValue2, int nIndex, bool bUseLast );		// 计算指标值
	virtual	bool	calc( float * pValue1, float * pValue2, float * pValue3, int nIndex, bool bUseLast );		// 计算指标值
	virtual	bool	calc( float * pValue1, float * pValue2, float * pValue3, float * pValue4, int nIndex, bool bUseLast );		// 计算指标值
	virtual	bool	calc( float * pValue1, float * pValue2, float * pValue3, float * pValue4, float * pValue5, int nIndex, bool bUseLast );		// 计算指标值
	virtual	bool	calc_ma( float * pValue, float * pMA, int nIndex, bool bUseLast, int nMADays );		// 计算指标值和均值

	// 判断是不是在nIndex(最后一个数据)处交叉 1:上穿 2:下穿
	int    GetCrossValue( float *pdValue1, float *pdValue2 ,int nIndex,float &fCross);
	int    GetTechIndex() { return  m_nTech;}
	void    SetTechIndex(int nTech) { m_nTech  =  nTech;}
   
protected:
    CKData *m_pKData;

	float	*m_pdCache1;
	float	*m_pdCache2;
	float	*m_pdCache3;
	float	*m_pdCache4;
	float	*m_pdCache5;
	bool	*m_pbHasCache1;
	bool	*m_pbHasCache2;
	bool	*m_pbHasCache3;
	bool	*m_pbHasCache4;
	bool	*m_pbHasCache5;
	int 	m_nLength;
    int	    m_nIndexStart;

	int		m_nLastIntensity;
	uint32_t    m_nLastIntensityCode;

	int    m_nTech;
};

// K线
class CKLine : public CTechnique
{
public:
	// Constructors
	CKLine( );
	CKLine( CKData * pKData );
	virtual ~CKLine();

public:
	virtual	void Clear( );

	// Attributes
	virtual	void	SetDefaultParameters( );
	void	AttachParameters( CKLine & src );
	virtual	bool	IsValidParameters( );

	// Operations
	virtual	bool	GetMinMaxInfo( int nStart, int nEnd, float *pdMin, float *pdMax );
};

#endif	//__STKLIB_TECHNIQUE_H__
