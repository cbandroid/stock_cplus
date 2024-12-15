#ifndef _TECH_PARAMETER_H_
#define _TECH_PARAMETER_H_

/*#include "MA.h"
#include "MACD.h"
#include "VOLUME.h"
#include "PSY.h"
#include "MIKE.h"
#include "ARBR.h"
#include "WR.h"
#include "RSI.h"
#include "SAR.h"
#include "KDJ.h"
#include "CCI.h"
#include "CR.h"
#include "DMI.h"
#include "BOLL.h"
#include "BIAS.h"
#include "OBV.h"
#include "VR.h"
#include "OSC.h"
#include "36BIAS.h"
#include "ASI.H"
#include "WVAD.h"
#include "ROC.h"
#include "EMV.h"
#include "MTM.h"
#include "BBI.h"
#include "PV.h"
#include "CV.h"
#include "CW.h"
#include "DJ.h"
#include "DPER.h"
#include "NVI.h"
#include "PVI.h"
#include "VROC.h"
#include "MOBV.h"
#include "MFI.h"
#include "VMACD.h"
#include "VRSI.h"
#include "NVRSI.h"
#include "AD.h"
#include "CI.h"
#include "UOS.h"
#include "MAOSC.h"
#include "36BIAS.h"
#include "DPO.h"
#include "KST.h"
#include "REI.h"
#include "DMKI.h"
#include "PCNT.h"
#include "HLC.h"
#include "CDP.h"
#include "ATR.h"
#include "CYO.h"
#include "DCYO.h"
#include "HSL.h"*/
/////////////////////////////////////////////////////////////////////////////
// CTechParameters
#include <map>

#include <./include/StructKlineView.h>
#include "Technique.h"
    
using namespace std;
class KdataContainer;
class TechnicalIndicator;

class CTechParameters/*:public CObject*/
{
public:
	CTechParameters( );
public:
	static CTechParameters& GetInstance()
	{
		static CTechParameters g_Instance;
		return g_Instance;
	}
	void	Clear( );
	void	SetKData( KdataContainer * pKData );
	void	SetDefaultParametersAll( );
	bool	FindParameters( int nTech, TechnicalIndicator * pTech );
	TechnicalIndicator* operator[](size_t indicator_index);

	//CMA		ma;
 	//CBBI	bbi;
 /*	CBOLL	boll;
 	//CPV		pv;
 	//CSAR	sar;
	//CDJ		dj;
	//CCW		cw;
 	CMACD	macd;
 	//CMIKE	mike;
 	CPSY	psy;
  	CVOLUME	volume;
 	CNVI	nvi;
 	CPVI	pvi;
 	CVR		vr;
 	CVROC	vroc;
 	COBV	obv;
 	CMOBV	mobv;
 	CMFI	mfi;
 	CVMACD	vmacd;
 	CWVAD	wvad;
 	CEMV	emv;
 	CVRSI	vrsi;
 	CNVRSI	nvrsi;
 	CAD		ad;
 	CCI		ci;
// 
 	CKDJ	kdj;*/
 	/*CR		r;
 	CRSI	rsi;
 	CBIAS	bias;
 	CMTM	mtm;
 	CDMI	dmi;
 	CROC	roc;
 	CCCI	cci;
 	CCV		cv;
 	CARBR	arbr;
 	CCR		cr;
 	COSC	osc;
 	CUOS	uos;
 	CMAOSC	maosc;
 	C36BIAS	bias36;
 	CDPO	dpo;
 	CKST	kst;
// 
 	CREI	rei;
 	CDMKI	dmki;
 	CPCNT	pcnt;
 	CHLC	hlc;
 	CCDP	cdp;
 	CASI	asi;
 	CATR	atr;
// 
 	CCYO	cyo;
 	CDCYO	dcyo;
 	CHSL	hsl;
 	CDPER	dper;*/
protected:
	std::map<size_t,TechnicalIndicator* > m_mapIndicators;
};
#endif
