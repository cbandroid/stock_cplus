#pragma once

//#include <map>
#include <string>
//#include <memory>
//#include <set>
#include <vector>
#include <float.h>
//#include <cstddef>
#include <cstdint>

#include "define.h"

using namespace std;

#define VOLSTICK                    1
#define STICK_VOL_FS                2
#define CIRCLE		                3
#define LINESTICK                   4
#define CROSSDOT                    5
#define STICK                       6
#define STICK_LINGXIAN_FS		    7
#define DOT                         8
#define iszero(x) x == 0

namespace CL {

struct LooseValue
{
	int nFoot;
	float fVal;
	float fExtent1;
	float fExtent2;
	float fExtent3;
	float fExtent4;
	std::wstring StrExtraData;
	LooseValue() :nFoot(0), fVal(0.0f), fExtent1(0.0f), fExtent2(0.0f), fExtent3(0.0f), fExtent4(0.0f) {}
};
typedef std::vector<LooseValue> looseValueArray;


class ARRAY_BE
{
public:
	float* line;
	int b;
	int e;
	float* lineWidth;

	enum Kind
	{
		Normal = 0,
		DrawLine,
		DrawPolyLine,
		DrawStickLine,
		DrawStickLine_R,
		DrawStickLine3D,
		DrawIcon,
		StringData,//DrawString
		DrawKLine,
		DrawNum,
		DrawBand,
		StringDataEx,//	
		StringDataFix,//
		DrawConditionColorStickLine,	//	        ֵ  ʵϰ        K  ɫ
		DrawPartLine,
		DrawRectRel,
		DrawTransRect,
		DrawRectBox,
	};

	Kind kind;//0 is normal,1 is draw line,3 is line data of right box,2 is string
	std::int16_t nLineWidth;
	std::int16_t isVirtualLine;
	std::wstring s;
	int iKlineDrawType;		// drawkline2ʱ    K ߵķ ʽ 0-  ͨ       1-ʵ     2-      
	std::vector<LooseValue> looseArr;


	ARRAY_BE();
		    
    virtual ~ARRAY_BE();
	
    void Reset();
	
    ARRAY_BE& operator=(ARRAY_BE& in);
	
    bool Add(ARRAY_BE& in1, ARRAY_BE& in2);
	
    bool Add(ARRAY_BE& in1, float fIn2);
	
    bool Sub(ARRAY_BE& in1, ARRAY_BE& in2);
	
    bool Sub(ARRAY_BE& in1, float fIn2);
	
    bool Sub(float fIn2, ARRAY_BE& in1);
	
    bool Mult(ARRAY_BE& in1, ARRAY_BE& in2);
	
    bool Mult(ARRAY_BE& in1, float fIn2);
	
    bool Div(ARRAY_BE& in1, ARRAY_BE& in2);
	
    bool Div(ARRAY_BE& in1, float fIn2);
	
    bool Div(float fIn2, ARRAY_BE& in1);
	
};


typedef struct Kline
{
    int     date;		// ʱ
	float	open;		//    ̼ 
	float   high;		//   ߼ 
	float   low;		//   ͼ 
	float   close;		//    ̼ 
	float   vol;		//  ɽ
    float   amount;		//  ɽ
    float   pctChg;
    float   turn;
    float	position;	//  ֲ
	union
	{
		struct
		{
			std::int16_t    advance;//   Ǽ   
			std::int16_t    decline;// µ     
		};
		float   volPositive;//        
	};
} Kline;
}
