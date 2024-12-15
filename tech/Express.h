#ifndef _EXPRESS_H_
#define _EXPRESS_H_


#define	SLH_USERDEFINE_BEGIN		10000
#define	SLH_WIDTH_MIN				60
#define	SLMAX_SLHTEXTLEN			256
#define MP_MaxVarNameLen            256

typedef	struct	slh_data_t{
	int		slh_id;
	char	string[SLMAX_SLHTEXTLEN+1];
	int		weight;
	char	varname[MP_MaxVarNameLen];
}SLH_DATA;


//	K线属性类
// 
// // 指标类别
#define	STT_CLASS_MIN		1
#define	STT_CLASS_KLINE		1	// K线叠加类
#define	STT_CLASS_TREND		2	// 趋势类
#define	STT_CLASS_ENERGY	3	// 能量类
#define	STT_CLASS_SWING		4	// 摆动类
#define	STT_CLASS_OTHER		5	// 其它类
#define	STT_CLASS_CLK		6	// 策略为王类
#define	STT_CLASS_USER		7	// 外挂指标类
#define	STT_CLASS_MAX		7
// 
// // 买卖信号
enum EnumITS
{
	ITS_MIN				=-15,
	ITS_SELLINTENSE		=-15,	//	强烈卖出
	ITSG_SELLINTENSE	=-13,	//	门限
	ITS_SELL			=-10,	//	卖出
	ITSG_SELL			=-8,    //	门限
	ITS_SELLFEEBLE		=-5,	//	微弱卖出
	ITSG_SELLFEEBLE		=-3,	//	门限
	ITS_NOTHING			=0,	//	无
	ITSG_BUYFEEBLE		=3,	//	门限
	ITS_BUYFEEBLE		=5,	//	微弱买进
	ITSG_BUY			=8,	//	门限
	ITS_BUY				=10,	//	买进
	ITSG_BUYINTENSE		=13,	//	门限
	ITS_BUYINTENSE		=15,	//	强烈买进
	ITS_MAX				=15
};


// // 买卖信号理由
enum EnumITSC
{
		ITSC_MIN				=1,
		ITSC_NOTHING			=1,	//	无
		ITSC_DEVIATEONBOTTOM	=2,	//	底背离
		ITSC_DEVIATEONTOP		=3,	//	顶背离
		ITSC_GOLDENFORK			=4,	//	金叉
		ITSC_DEADFORK			=5,	//	死叉
		ITSC_LONG				=6,	//	多头
		ITSC_SHORT				=7,	//	空头
		ITSC_OVERBOUGHT			=8,	//	超买
		ITSC_OVERSOLD			=9,	//	超卖
		ITSC_SUPPORT			=10,	//	支撑位
		ITSC_RESISTANCE			=11,	//	阻力位
		ITSC_MAX				=11
};


#define	ITS_ISSELL(x)			(x<0)
#define	ITS_ISBUY(x)			(x>0)
#define	ITS_ISSELLORNOTHING(x)	(x<=0)
#define	ITS_ISBUYORNOTHING(x)	(x>=0)

#define	ITS_GETMINMAXDAYRANGE	120
#define	ITS_DAYS_DEVIATE		115
//////////////////////////////////////////////////////////////////////
//属性索引
enum SLH{
	SLH_NONE=					0,
	SLH_MIN=					1,
	SLH_CODE=					1,	//	合约代码
	SLH_NAME=					2,	//	股票名称
	SLH_EXCHANGEID=				96,
	//	技术数据
	SLH_DATE=					3,	//	日期
	SLH_PRECLOSE=				4,	//	昨收
	SLH_OPEN=					5,	//	今开
	SLH_CLOSE=					6,	//	收盘价
	SLH_HIGH=					7,	//	最高价
	SLH_LOW=					8,	//	最低价
	SLH_AVERAGE=				9,	//	均价
	SLH_DIFF=					10,	//	涨跌
	SLH_DIFFPERCENT=			11,	//	涨幅%
	SLH_SCOPE=					12,	//	震幅%
	SLH_VOLUME=					13,	//	成交量
	SLH_TURNOVER=				14,	//	成交额
	SLH_VOLUP=					15,	//	外盘
	SLH_VOLDOWN=				16,	//	内盘
	SLH_DIFFPERCENT_MIN5=		17,	//	五分钟涨幅%
	SLH_SELLBUYRATIO=			18,	//	委比%
	SLH_SELLBUYDIFF=			19,	//	委量差
	SLH_BUYPRICE3=				20,	//	买价三
	SLH_BUYPRICE2=				21,	//	买价二
	SLH_BUYPRICE1=				22,	//	买价一
	SLH_SELLPRICE1=				23,	//	卖价一
	SLH_SELLPRICE2=				24,	//	卖价二
	SLH_SELLPRICE3=				25,	//	卖价三
	SLH_BUYVOLUME3=				26,	//	买量三
	SLH_BUYVOLUME2=				27,	//	买量二
	SLH_BUYVOLUME1=				28,	//	买量一
	SLH_SELLVOLUME1=			29,	//	卖量一
	SLH_SELLVOLUME2=			30,	//	卖量二
	SLH_SELLVOLUME3=			31,	//	卖量三

	//	主要基本指标
	SLH_LASTPRICE=				32, //最新价
	
	SLH_PE=						33,	//	市盈率
	SLH_PNETASSET=				34,	//	市净率
	SLH_PMAININCOME=			35,	//	市销率
	SLH_RATIO_PCASH	=			36,	//	价格净现金比
	SLH_RATIO_CURRENCY=			37,	//	流通率
	SLH_RATIO_CHANGEHAND=		38,	//	换手率
	SLH_RATIO_VOLUME=			39,	//	量比
	SLH_RS=						40,	//	相对强度
	SLH_MARKETVALUE=			41,	//	总市值
	SLH_MARKETVALUEA=			42,	//	A股市值
	SLH_MARKETVALUEB=			43,	//	B股市值
	// ★偿债能力
	SLH_RATIO_LIQUIDITY	=		44,	//	流动比率
	SLH_RATIO_QUICK	=			45,	//	速动比率
	SLH_VELOCITY_RECEIVABLES=	46,	//	应收帐款周率
	// ★经营能力
	SLH_VELOCITY_MERCHANDISE=	47,	// 存货周转率
	SLH_MAIN_INCOME	=			48,	// 主营业务收入
	SLH_CASH_PS		=			49,	// 每股净现金流量
	// ★盈利能力
	SLH_PROFIT_MARGIN	=		50,	// 主营业务利润率
	SLH_NETASSET_YIELD	=		51,	// 净资产收益率
	// ★资本结构
	SLH_DATE_BEGIN	=			52,	//	上市日期
	SLH_SHARE_COUNT_TOTAL=		53,	//	总股本
	SLH_SHARE_COUNT_A	=		54,	//	流通A股
	SLH_SHARE_COUNT_B	=		55,	//	流通B股
	SLH_SHARE_COUNT_H	=		56,	//	流通H股
	SLH_SHARE_COUNT_NATIONAL=	57,	//	国有股
	SLH_SHARE_COUNT_CORP=		58,	//	法人股
	SLH_PROFIT_PSUD		=		59,	// 每股未分配利润
	SLH_ASSET=					60,	// 总资产
	SLH_RATIO_HOLDERRIGHT=		61,	// 股东权益比率
	SLH_RATIO_LONGDEBT	=		62,	// 长期负债率
	SLH_RATIO_DEBT		=		63,	// 资产负债率
	// ★投资收益能力
	SLH_NETASSET_PS	=			64,	// 每股净资产
	SLH_NETASSET_PS_REGULATE=	65,	// 调整每股净资产
	SLH_EPS=						66,	// 每股收益
	SLH_EPS_DEDUCT	=			67,	// 扣除后每股收益
	SLH_NET_PROFIT	=			68,	// 净利润
	SLH_MAIN_PROFIT	=			69,	// 主营业务利润
	SLH_TOTAL_PROFIT=			70,	// 利润总额
	SLH_PROFIT_INC	=			71,	// 主营利润增长率
	SLH_INCOME_INC	=			72,	// 主营收入增长率
	SLH_ASSET_INC	=			73,	// 总资产增长率
	SLH_ANNGAINS_AVERAGE=		74,	// 年平均收益率
	SLH_ANNGAINS_STDDEV	=		75,	// 收益标准差
	SLH_BETA=					76,	// β值
	SLH_SHARP=					77,	// 夏普风险指数
	SLH_TRADE=					78,	// 行业
	SLH_PROVINCE=				79,	// 省份
	SLH_PRODUCTID             =100,
	SLH_PRODUCTCLASS          =101,
	SLH_DELIVERYYEAR          =102,   //交割年份
	SLH_DELIVERYMONTH         =103,   //交割月
	SLH_MAXMARKETORDERVOLUME  =104,   //市价单最大下单量
	SLH_MINMARKETORDERVOLUME  =105,   //市价单最小下单量
	SLH_MAXLIMITORDERVOLUME   =106,
	SLH_MINLIMITORDERVOLUME   =107,
	SLH_VOLUMEMULTIPLE        =108,
	SLH_PRICETICK             =109,
	SLH_CREATEDATE            =110,
	SLH_OPENDATE              =111,
	SLH_EXPIREDATE            =112,
	SLH_STARTDELIVDATE        =113,
	SLH_ENDDELIVDATE          =114,
	SLH_INSTLIFEPHASE         =115,
	SLH_ISTRADING             =116,
	SLH_POSITIONTYPE          =117,
	SLH_POSITIONDATETYPE      =118,
	SLH_LONGMARGINRATIO       =119,
	SLH_SHORTMARGINRATIO      =120,//空头保证金率
	SLH_REPORTTYPE            =121,	//	报表类型
	
	SLH_SELLPRICE4 = 122,	//	卖量四
	SLH_SELLPRICE5 = 123,	//	卖量五

	SLH_BUYPRICE4 = 124,	//	卖量四
	SLH_BUYPRICE5 = 125,	//	卖量五

	SLH_SELLVOLUME4 = 126,	//	卖量四
	SLH_SELLVOLUME5 = 127,	//	卖量五

	SLH_BUYVOLUME4 = 128,	//	卖量四
	SLH_BUYVOLUME5 = 129,	//	卖量五

	SLH_MAX = 130,

	SLH_INVALID=			 (-1)
};
enum
{
	ACCE_MIN=1,
	ACCE_01=1,	// 热键F1
	ACCE_02=2,	// 热键F
	ACCE_03=3,	// 热键F3
	ACCE_04=4,	// 热键F4
	ACCE_05=5,	// 热键F5
	ACCE_06=6,	// 热键F6
	ACCE_07=7,	// 热键F7
	ACCE_08=8,	// 热键F8
	ACCE_09=9,	// 热键F9
	ACCE_10=10,	// 热键F10
	ACCE_1=11,	// 上A报价
	ACCE_2=12,	// 上B报价
	ACCE_3=13,	// 深A报价
	ACCE_4=14,	// 深B报价
	ACCE_5=15,	// 上债报价
	ACCE_6=16,	// 深债报价
	ACCE_61=17,	// 上A涨跌
	ACCE_62=18,	// 上B涨跌
	ACCE_63=19,	// 深A涨跌
	ACCE_64=20,	// 深B涨跌
	ACCE_65=21,	// 上债涨跌
	ACCE_66=22,	// 深债涨跌
	ACCE_71=23,	// 上证信息
	ACCE_72=24,	// 深证信息
	ACCE_73=25,	// 券商信息
	ACCE_81=26,	// 上A综合
	ACCE_82=27,	// 上B综合
	ACCE_83=28,	// 深A综合
	ACCE_84=29,	// 深B综合
	ACCE_85=30,	// 上债综合
	ACCE_86=31,	// 深债综合
	ACCE_LBDK=32,	// 显示分时行情量比多空指标
	ACCE_MMLD=33,	// 显示分时行情买卖力道指标
	ACCE_MAX=33,
};
/* 技术指标唯一编号 */
enum
{
	STT_MIN=1,
	//	K线属性类
	STT_KLINE_MIN=	1,
	STT_MA		=	1,		//	移动平均线MA
	STT_BBI		=	2,		//	多空指标BBI
	STT_BOLL	=	3,		//	布林带BOLL
	STT_PV		=	4,		//	汇证均价线P/V
	STT_SAR		=	5,		//	停损点转向指标SAR
	STT_DJ		=	6,		//	叠加图DJ
	STT_CW		=	7,		//	筹码分布图CW
	STT_KLINE_MAX=	7,

	//	趋势类
	STT_TREND_MIN=	8,
	STT_MACD	=	8,		//	指数平滑异同移动平均线MACD
	STT_MIKE	=	9,		//	麦克指标MIKE
	STT_PSY		=	10,		//	心理线PSY
	STT_TREND_MAX=	10,

	//	能量类
	STT_ENERGY_MIN=	11,
	STT_VOLUME	=	11,		//	成交量
	STT_NVI		=	12,		//	负成交量指标NVI
	STT_PVI		=	13	,	//	正成交量指标PVI
	STT_VR		=	14	,	//	容量比率VR
	STT_VROC	=	15	,	//	成交量变动率指标VROC
	STT_OBV		=	16	,	//	能量线OBV
	STT_MOBV	=	17	,	//	主力进出MOBV
	STT_MFI		=	18	,	//	资金流动指数MFI
	STT_VMACD	=	19	,	//	量指数平滑异同移动平均线VMACD
	STT_WVAD	=	20	,	//	威廉变异离散量WVAD
	STT_EMV		=	21	,	//	简易波动指标EMV
	STT_VRSI	=	22	,	//	量相对强弱指标VRSI
	STT_NVRSI	=	23	,	//	新量相对强弱指标VRSI
	STT_AD		=	24	,	//	聚/散指标A/D
	STT_CI		=	25	,	//	累积指数CI
	STT_ENERGY_MAX=	25,

	//	摆动类
	STT_SWING_MIN=	26,
	STT_KDJ		=	26	,	//	随机指标KDJ
	STT_R		=	27	,	//	威廉指标R
	STT_RSI		=	28	,	//	相对强弱指标
	STT_BIAS	=	29	,	//	乖离率BIAS
	STT_MTM		=	30	,	//	动量指标MTM
	STT_DMI		=	31	,	//	动向指标DMI
	STT_ROC		=	32	,	//	变动率指标ROC
	STT_CCI		=	33	,	//	顺势通道指标CCI
	STT_CV		=	34	,	//	佳庆指标CV
	STT_ARBR	=	35	,	//	人气意愿指标ARBR
	STT_CR		=	36	,	//	中间意愿指标CR
	STT_OSC		=	37	,	//	摆动量OSC
	STT_UOS		=	38	,	//	极摆动指标UOS
	STT_MAOSC	=	39	,	//	移动平均摆动量MAOSC
	STT_36BIAS	=	40	,	//	3减6日乖离3-6BIAS
	STT_DPO		=	41	,	//	去势价格摆动指数DPO
	STT_KST		=	42	,	//	确然指标KST
	STT_SWING_MAX=	42,

	//	其他类
	STT_OTHER_MIN=	43,
	STT_REI		=	43	,	//	范围膨胀指数REI
	STT_DMKI	=	44	,	//	迪马克尔指标DMKI
	STT_PCNT	=	45	,	//	幅度比PCNT
	STT_HLC		=	46	,	//	幅度分析HLC
	STT_CDP		=	47	,	//	逆势操作指标CDP
	STT_ASI		=	48	,	//	累积摆动指标ASI
	STT_ATR		=	49	,	//	均幅指标ATR
	STT_OTHER_MAX=	49,

	STT_CLK_MIN	=	50,
	STT_CYO		=	50	,	//	周期摆动量CYO
	STT_DCYO	=	51	,	//	去噪周期摆动量DCYO
	STT_HSL		=	52	,	//	相对换手率HSL
	STT_DPER	=	53	,	//	去势百分比指标DPER
	STT_CLK_MAX	=	53,

	STT_MAX		=	53,
	STT_USER_MIN=	10000
};

#endif

