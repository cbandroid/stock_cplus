#pragma once

#include <cstddef>
#include <cstdint>
#include <cmath>

const int BEGIN_NUMBER1 = 100;

const int REGEX_NUM = 8;

const int PARAM_NUM = 64;					// huhe* 加到16个参数设置 txl chang from 16 to 64
const int EXTRA_Y_NUM = 4;					// 额外的Y轴坐标数
const int NUM_LINE_TOT = 64;

const int MAXB = 2000;
const int MAXB_ALL = 2000;
const int MIDB = 10000;
const int MIDE = 10199;

const int RLINEB = 11000;
const int RLINEE = 11000 + NUM_LINE_TOT;

const int CEFB = 12000;    //浮点数开始
const int CEFE = 12299;	 //浮点数结束

const int CEIB = 13000;    //整数开始
const int CEIE = 13299;	//整数结束

const int CEEXPB = 13300;    //表达式开始
const int CEEXPE = 13330;	//表达式结束

const int CECB = 13500;    //字符串开始
const int CECE = 13580;	//字符串结束	//txl change from 13520 to 13580

const int FuncB = 8000;	//函数开始
const int FuncDllB = 8400;	//函数开始
const int FuncDllE = 8700;	//函数结束
const int FuncSysDllB = 8780;		//	系统DLL函数开始
const int FuncSysDllE = 8789;		//	系统DL函数结束
const int FuncUserDllB = 8790;		//	用户DLL函数开始
const int FuncUserDllE = 8799;		//	用户DL函数结束
const int FuncE = 8899;	//函数结束

const int PCMB = 14000;	//中间变量开始
const int PCME = 14199;	//中间变量结束

const int QUB = 8800;  //引用开始
const int QUE = 8899;  //引用终止

const int TOZERO = 9009;  //
const int CONSB = 9010;  //线型常数开始
const int CONSCOLORB1 = 9100;  //color常数开始
const int CONSCOLORB = 9200;  //color常数开始
const int CONSCOLORE = 9280;  //color常数开始
const int CONSLINETHICKB = 9300;  //
const int CONSLINETHICKE = 9320;  //
const int CONSLINEDOT = 9350;  //
const int CONSE = 9498;  //线型常数终止


const int CONSQUOTE = 9521;  //	行情常数
const int CONSREFPERIODB = 9650;	//	行情周期引用常数
const int CONSREFPERIODE = 9670;	//	行情周期引用常数
const int CONSEXPB = 9670;					//	行情表达式常数
const int CONSEXPE = 9680;					//	行情表达式常数

const int CONSEXTINB = 9700;			//	行情扩展常数
const int CONSEXTINE = 9800;			//	行情扩展常数

const int PAM = 9900;  // 系统参数的最大值
const int LEN_BYTE = 240 + 136;				// huhe* 加到16个参数设置

const int MANY_DAY_K_LINE = 15;
const int HS_K_LINE = 25;
const int MINUTE_1_K_LINE = 35;
const int NUM_MANY_DAY = 50;
const int PRICE_ARRAY_SIZE = 12; // 12档
//const bool FALSE = false;
//const int MAX_PATH = 256;
const float INVALID = 256;
const int KLINE_WIDTH = -1;
// const void* INVALID_HANDLE_VALUE = reinterpret_cast<void*>(-1);

#undef NULL
const std::nullptr_t NULL = nullptr;

//using COLORREF = std::int32_t;
using uint32 = std::uint32_t;
using uint = std::uint32_t;
using uint64 = std::uint64_t;
using int32 = std::int32_t;
using bool32 = std::int32_t;

// using CMapStringToPtr = std::unordered_map<std::wstring, void*>;


#define EPSINON (0.00001)
#define equ_than_0(f) ((f) >= -EPSINON && (f) <= EPSINON)
#define greater_than_0(f) ((f) > EPSINON)
#define greater_equ_than_0(f) ((f) >= -EPSINON)
#define less_than_0(f) ((f) < -EPSINON)
#define less_equ_than_0(f) ((f) <= EPSINON)
//#define iszero(f) equ_than_0((f))
#define lesszero(f) less_than_0((f))
#define greatezero(f) greater_than_0((f))
#define float_true(f) !equ_than_0((f))
#define	MEMSET_ARRAY(arr, val) memset((void*)arr, 0, sizeof(arr))
#define ZERO_ARRAY(arr) MEMSET_ARRAY(arr, 0)
#define ARRAY_SIZE(arr) sizeof(arr)/sizeof(arr[0])
#define LESS_ARRARY_INDEX(arr, index) ((index) >= 0 && (index) < ARRAY_SIZE(arr))
#define LESS_INDEX(index, total) ((index)>=0 && (index)<(total))
//#define MAKEWORD(a, b)      ((std::int16_t)(((std::uint8_t)(((std::uint32_t)(a)) & 0xff)) | ((std::int16_t)((std::uint8_t)(((std::uint32_t)(b)) & 0xff))) << 8))
//#define MAKELONG(a, b)      ((std::int32_t)(((std::int16_t)(((std::uint32_t)(a)) & 0xffff)) | ((std::uint32_t)((std::int16_t)(((std::uint32_t)(b)) & 0xffff))) << 16))
//#define LOWORD(l)           ((std::int16_t)(((std::uint32_t)(l)) & 0xffff))
//#define HIWORD(l)           ((std::int16_t)((((std::uint32_t)(l)) >> 16) & 0xffff))
//#define LOBYTE(w)           ((std::uint8_t)(((std::uint32_t)(w)) & 0xff))
//#define HIBYTE(w)           ((std::uint8_t)((((std::uint32_t)(w)) >> 8) & 0xff))
#define CATCH_EXCEPTION_EXP(exp) try{ exp;} catch(...) {;}
#define _T(x) L##x
#define ASSERT(x)
#define GetRValue(rgb)      (LOBYTE(rgb))
//#define GetGValue(rgb)      (LOBYTE(((std::int16_t)(rgb)) >> 8))
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
// 比较f1,f2，f1<f2返回－１；f1>f2 返回1  f1=f2 返回0
inline int float_cmp(float f1, float f2, int nPrecision = 2) {
	float s = f1 - f2;
	float gaps = std::pow(0.1, nPrecision);
	if (fabs(s) < gaps) {
		// f1 = f2
		return 0;
	}
	if (s <= -gaps) {
		// f1 < f2
		return -1;
	}
	// f1 > f2
	return 1;
}

