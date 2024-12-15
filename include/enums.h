#pragma once
#ifndef _ENUMS_H_
#define _ENUMS_H_

#include "StructKlineView.h"
//#include "define.h"
namespace CL {

enum LineType
{
    LINE_UNKNOW=0,	//未知
    LINE_UP=1,		//向上
    LINE_DOWN=-1	//向下
};

enum LineZhongshuType
{
    LINE_ZS_UNKNOW=0,	//未知
    LINE_ZS_UP=1,		//向上
    LINE_ZS_DOWN=-1	//向下
};
//	指标关注数据类型
enum FormularDataType : uint64_t {
	kFormularOffline = 0x01,
	kFormularPlugin = 0x02,
	kFormularFinanace = 0x04,
	kFormularSym = 0x08,
	kFormularSymIndex = 0x10,
	kFormularIndexKline = 0x20,
	kFormularQuote = 0x40,
	kFormularBlock = 0x80,
	kFormularCloud = 0x100,		//	云指标数据
};

enum E_NodeTimeInterval
{
	ENTIMinute = 0,			// 分钟线	
	ENTIDay,				// 日线
	ENTIMinute3,
	ENTIMinute5,
	ENTIMinute15,
	ENTIMinute30,
	ENTIMinute60,
	ENTIMinuteUser,			// 用户自定义数目分钟线	
	ENTIDayUser,			// 用户自定义数目日线
	ENTIWeek,				// 周线
	ENTIMonth,				// 月线
	ENTIQuarter,			// 季度
	ENTIYear,				// 一年
	ENTIMinute90,
	ENTIMinute120,
	ENTICount
};

enum E_KLineTypeBase	// 基本K线类型, 其他的K线类型或周期均由以下基础K线组合而来
{
	EKTBMinute = 0,		// 分钟线
	EKTBDay,			// 日线
	EKTB5Min,			// 5分钟线
	EKTBHour,			// 小时线
	EKTBMonth,			// 月线
	EKTBCount			// 基本类型数量
};

// 所有表头种类
enum E_ReportType
{
	ERTFuturesCn = 0,			// 国内期货	
	ERTStockCn,					// 国内证券
	ERTMony,					// 外汇
	ERTExp,						// 指数
	ERTStockHk,					// 香港股票
	ERTFuturesForeign,			// 国外期货

	// 
	ERTWarrantCn,				// 国内权证
	ERTWarrantHk,				// 香港权证

	//
	ERTFutureRelaMonth,			// 相对月期货
	ERTFutureSpot,				// 现货
	ERTExpForeign,				// 全球指数
	ERTFutureGold,				// 黄金期货
	ERTOptions,					// 期权

	ERTUSStock,					// 美股
	ERTCustom,					// 自定义
	ERTUser,					// 自选股
	ERTNone,					// 无意义(空报价表) 所有在此之后新加的必须小心栏目设置对话框里面的各tab item必须跳过该表头	
	ERTMagicShark,				// 魔术鲨港股板块
	ERTCBBCHk,					// 香港牛熊证
	ERTFutureHK,                // 香港期货
	ERTIndexStocks,             // 指数成份股
	ERTChooseStock,				// 选股池集合

	ERTConvertibleBond,			// 可转债

	ERTBKZJ = 23,				// 板块资金
	ERTBKZS = 24,				// 板块指数
	ERTLHB = 25,				// 龙虎榜
	ERTJJ = 26,					// 基金
	ERTJYXG = 27,				// 绩优选股
	ERTBKCFG = 28,				// 板块成分股
	ERTZDFB = 29,				// 涨跌幅榜" 
	ERTZSB = 30,				// 涨速榜" 
	ERTZJ = 31,					// 债券
	ERTRMQL = 32,				// 热门强龙

	ERTFixGrid = 33,			// 霸财k线版面显示特殊字段的表头
	ERTNGLD = 34,				// 牛股雷达涨停板

	// by csc 2021/12/09 排行榜表头
	ERTMutiRankTodayRise = 35,				// 今日涨幅排行榜
	ERTMutiRankTodayFall = 36,				// 今日跌幅排行榜
	ERTMutiRank5MinRise = 37,				// 今日5分钟涨幅排行榜
	ERTMutiRank5MinFall = 38,				// 今日5分钟跌幅排行榜
	ERTMutiRankVolRate = 39,				// 今日量比排行榜
	ERTMutiRankAmount = 40,				    // 今日成交额排行榜
	ERTCompositeRank = 41,				    // 组合排行
	ERTCount
};

// 快捷键
typedef enum E_HotKeyType
{
	EHKTMerch = 0,		// 商品快捷键
	EHKTShortCut,		// 快捷键
	EHKTBlock,			// 板块快捷键
	EHKTIndex,			// 指标快捷键

	EHKTLogicBlock,		// 逻辑板块列表
	EHKTBroker,			// 经纪人列表
	//
	EHKTCount
}E_HotKeyType;


// 有效标志
enum E_SymFinanceResUpdateFlag
{
	enSFRUFCurrencyCode = 0,
	enSFRUFShowUnit,

	enSFRUFReportDate,
	enSFRUFYearEnd,

	enSFRUFIndexName,
	enSFRUFIndexWeight,
	enSFRUFIndexSp,

	enSFRUFMarketCap,
	enSFRUFAllCapital,
	enSFRUFCircCapital,
	enSFRUFAllAsset,
	enSFRUFFlowDebt,

	enSFRUFPerFund,
	enSFRUFBusinessProfit,
	enSFRUFPerNoDistribute,
	enSFRUFPerIncome,

	enSFRUFPerPureAsset,
	enSFRUFChPerPureAsset,
	enSFRUFDorRightRate,
	enSFRUFIncomeSeason,

	enSFRUFYearHigh,
	enSFRUFYearLow,
	enSFRUFWeek52High,

	enSFRUFWeek52Low,
	enSFRUFYearPriceChange,
	enSFRUFYearMarketChange,
	enSFRUFAvgVolume,

	enSFRUFAvgTurnover,
	enSFRUFMa10,
	enSFRUFMa20,
	enSFRUFMa50,
	enSFRUFMa90,
	enSFRUFMa250,
	enSFRUFRSI10,

	enSFRUFOSC10,

	enSFRUFLastPE,
	enSFRUFYearHighPE,
	enSFRUFYearLowPE,
	enSFRUFYearYield,

	enSFRUFYearHighYield,
	enSFRUFYearLowYield,

	enSFRUFLastPrice2PA,
	enSFRUFLastPrice2CashPS,
	enSFRUFLastPrice2DebtPS,
	enSFRUFLastEPSGrowth,

	enSFRUFDividendPS,
	enSFRUFCashPS,
	enSFRUFDebtPS,

	enSFRUFCashFlowPS,
	enSFRUFTurnover,
	enSFRUFPecialItem,
	enSFRUFProfitBeforeTax,

	enSFRUFTax,
	enSFRUFShareHolderToProfit,
	enSFRUFDepreciation,
	enSFRUFInterestOnLoans,
	enSFRUFInterestCap,

	enSFRUFFixAsset,
	enSFRUFInvest,
	enSFRUFCurrentAsset,
	enSFRUFCash,

	enSFRUFInventory,
	enSFRUFShortDebt,
	enSFRUFLongDebt,
	enSFRUFShareHolderEquity,

	enSFRUFCurrentRatio,
	enSFRUFQuickRatio,
	enSFRUFLongDebt2Equity,
	enSFRUFAllDebt2Equity,

	enSFRUFAllDebt2Capex,
	enSFRUFNetDebt2Equity,
	enSFRUFReturnOnEquity,
	enSFRUFReturnOnTotalAsset,

	enSFRUFOperatingMargin,
	enSFRUFProfitMargin,
	enSFRUFInventoryTurnoverRate,
	enSFRUFDividendPayoutRatio,

	// 权证
	enSFRUFSubscriptionRate,
	enSFRUFLeverageRatio,
	enSFRUFPremium,
	enSFRUFWarrantInMarket,
	enSFRUFWarrantInMarketPer,

	// 银行股数据
	enSFRUFCapitalAdequacyRatio,
	enSFRUFCost2Income,
	enSFRUFLoanGrowth,
	enSFRUFDepositGrowth,
	enSFRUFCurAsset2Deposit,

	enSFRUFLoan2Deposit,
	enSFRUFLoan2Equity,
	enSFRUFLoan2TotalAsset,
	enSFRUFDeposit2Equity,
	enSFRUFDeposit2TotalAsset,

	enSFRUFROAvgLoans,
	enSFRUFROAvgDeposits,

	enSFRUFInterestIncome,
	enSFRUFOtherIncome,
	enSFRUFOperatExpense,

	enSFRUFProvisionForBadDebt,
	enSFRUFDerivativeAsset,
	enSFRUFLoans,
	enSFRUFCashAndShortTermFund,
	enSFRUFOtherAsset,

	enSFRUFDeposits,
	enSFRUFOtherLiab,

	enSFRUFLastTradeDay,
	enSFRUFLeverage,
	enSFRUFDelta,
	enSFRUFBreakEven,
	enSFRUFStretchBand,
};

// 公式类型
enum E_FormularType
{
	EFTNormal = 0,                          // 普通技术指标
	EFTClrKLine,                            // 五彩 K 线
	EFTCdtChose,                            // 条件选股
	EFTTradeChose,                          // 交易选股

	EFTCount,
};

// 指标与股票关联类型
enum E_IndexStockType
{
	EIST_None = 0,                          // 无类型限制
	EIST_Index = 1,                         // 指数指标，仅显示于指数
	EIST_Stock = 2,                         // 个股型指标，仅显示于个股
};

enum TraitStatus
{
	Trait_None = 0,	// 无
	Trait_Summit,	// 顶形成
	Trait_Base,		// 底形成
	Trait_Down,		// 向下
	Trait_Up,		// 向上
	Trait_Strong_Base, // 强势底形成
	Trait_Strong_Summit, // 强势顶形成
	Trait_Relay_Base_3, //底形成且中继大于等于3
	Trait_Relay_Summit_3, //顶形成且中继大于等于3
	Trait_Pivot_Up, // 笔中枢向上
	Trait_Pivot_Down, // 笔中枢向下
	Trait_Plain_Up,//上盘整  
	Trait_Plain_Down,//下盘整
	Trait_Typing_Up,//分型笔 底部抬高
	Trait_Typing_Down,//分型笔 顶部降低
	Trait_Buy_st,		// 一买
	Trait_Buy_nd,		// 二买
	Trait_Buy_rd,		// 三买
	Trait_Sell_st,		// 一卖
	Trait_Sell_nd,		// 二卖
	Trait_Sell_rd,		// 三卖
	Trait_Power_Summit, // 动力顶
	Trait_Power_Base,	// 动力底
	Trait_Power_SD, // 动力顶背离
	Trait_Power_BD,	// 动力底背离
	Trait_Segment_Summit,	// 顶形成(段)
	Trait_Segment_Base,		// 底形成(段)
	Trait_Trend_Up,			// 向上中枢趋势
	Trait_Trend_Down,		// 向下中枢趋势


};

enum Trait 
{
	NEWLY = 11, //新笔
	OLDEN = 12, //老笔
	DIVISION = 51,//分型笔
    NEWLY_EXACT = 21, //严格定义的新笔
	OLDEN_EXACT = 22, //严格定义的老笔
    NEWLY_MISTY = 31, //模糊新笔
    OLDEN_MISTY = 32,  //模糊老笔
};

struct Pivot {
	int index;
	float highly = 0;
	float lowly = 0;
	float zg = 0;
	float zd = 0;
    float future_zd;
    float future_zg;
    
    int type;
    int entry; //进入笔/线段
    int leave;//离开笔/线段
    int b; //原K线进入点
    int e; //原K线离开点
    int level;
    int trend;
	bool up = false;    
    
    int enter_d;//#
    float aft_l_price = 0;
    int aft_l_time;// # time for third type buy or sell point   
    float  end_price ;
    int start_index;
    int end_index;//# should be updated after growing
    // #lines[self.end_index] is the leaving seg
    float finished;
    float enter_force;
    float leave_force;//# should be updated after growing
    int size;//#should be updated
    float mean;
    int start_time;
    int leave_start_time;//# should be updated after growing
    int leave_end_time;// # should be updated after growing
    int leave_d;// # should be updated after growing
    float leave_end_price;//# should be updated after growing
    float leave_start_price;
    float prev2_force;
    float prev1_force;
    float prev2_end_price;
};

struct P {
	int i = 0;
	bool up = false;
};

struct include_klines {
    Kline kd;
    int include_head;	// 起始
    int include_tail;	// 结束
    int k_index;	// 原K线位置
    bool isUp;	// 是否向上
};

struct TypingData{
	int index;
    int i; //newBars的下标
    int up;
    //bool up;
    float FXValue;
    int k_index;	// 原K线最高或者最低位置
    Kline extreme_vertex;
};

struct StrokeData{
   int index;
   int type;
   bool up; //笔方向
   int  entry;//笔对应的分型开始结束索引
   int  leave;
   float value;
   float entryPrice;
   float leavePrice;
   //笔对应的k线开始结束索引
   int entryKLineIndex;
   int leaveKLineIndex;
   //确定该笔的k线索引
   int k_index;
   float cachePrice;
   float high;
   float low;
};

struct BS {
	bool b = false;
	int i = 0;
	int type = 0;
};

struct Segment {
    int index;
    float value;
	int  start; //笔进入的索引
	int  end;   //笔离开的索引
	bool up = false;
	bool is_sure = false;	// 是否被确认
	int d;
    int entry; //newBars的索引 
    int leave; 
    float entryPrice;
    float leavePrice;
    float high;
    float low;
};


struct TailData {
	int entryTime;
	float entryPrice;
	int leaveTime;
	float leavePrice;
	int up;
};

struct segextreme {
	int d;
	float value;
	float cur_extreme;
};

struct segpoint {
	int entry;
	float entry_value;
	int leave;
	float leave_value;
};
} // namespace
#endif
