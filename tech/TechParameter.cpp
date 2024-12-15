#include "KData.h"
#include "TechParameter.h"


//////////////////////////////////////////////////////////////////////
// CTechParameters
CTechParameters::CTechParameters( )
{
	//PRINTF("CTechParameters::CTechParameters()-->Enter");
}

void CTechParameters::Clear( )
{
	//PRINTF("CTechParameters::Clear()-->Enter");
	ma.clear( );
	bbi.clear( );
	boll.clear( );
	pv.clear( );
	sar.clear( );
	dj.clear( );
	cw.clear( );

	macd.clear( );
	mike.clear( );
	psy.clear( );

 	volume.clear();
	// 	nvi.clear( );
	// 	pvi.clear( );
	vr.clear( );
	// 	vroc.clear( );
 	obv.clear( );
// 	// 	mobv.clear( );
// 	// 	mfi.clear( );
// 	// 	vmacd.clear( );
 	wvad.clear( );
 	emv.clear( );
	// 	vrsi.clear( );
	// 	nvrsi.clear( );
	// 	ad.clear( );
	// 	ci.clear( );
	// 
 	kdj.clear( );
 	r.clear( );
	rsi.clear( );
	bias.clear( );
	mtm.clear( );
	dmi.clear( );
	roc.clear( );
	cci.clear( );
	cv.clear( );
	arbr.clear( );
	cr.clear( );
	osc.clear( );
	// 	uos.clear( );
	// 	maosc.clear( );
	bias36.clear( );
	// 	dpo.clear( );
	// 	kst.clear( );
	// 
	// 	rei.clear( );
	// 	dmki.clear( );
	// 	pcnt.clear( );
	// 	hlc.clear( );
	// 	cdp.clear( );
	asi.clear( );
	// 	atr.clear( );
	// 
	// 	cyo.clear( );
	// 	dcyo.clear( );
	// 	hsl.clear( );
	// 	dper.clear( );
	//PRINTF("CTechParameters::Clear()-->Leave");
}

void CTechParameters::SetKData( KdataContainer * pKData )
{
 	ma.set_kdata( pKData );
 	bbi.set_kdata( pKData );
 	boll.set_kdata( pKData );
 	pv.set_kdata( pKData );
 	sar.set_kdata( pKData );
 	dj.set_kdata( pKData );
 	cw.set_kdata( pKData );

 	macd.set_kdata( pKData );
 	mike.set_kdata( pKData );
 	psy.set_kdata( pKData );

 	volume.set_kdata( pKData );
	// 	nvi.set_kdata( pKData );
	// 	pvi.set_kdata( pKData );
	vr.set_kdata( pKData );
	// 	vroc.set_kdata( pKData );
	obv.set_kdata( pKData );
	// 	mobv.set_kdata( pKData );
	// 	mfi.set_kdata( pKData );
	// 	vmacd.set_kdata( pKData );
 	wvad.set_kdata( pKData );
 	emv.set_kdata( pKData );
	// 	vrsi.set_kdata( pKData );
	// 	nvrsi.set_kdata( pKData );
	// 	ad.set_kdata( pKData );
	// 	ci.set_kdata( pKData );
	// 
 	kdj.set_kdata( pKData );
 	r.set_kdata( pKData );
 	rsi.set_kdata( pKData );
 	bias.set_kdata( pKData );
 	mtm.set_kdata( pKData );
 	dmi.set_kdata( pKData );
 	roc.set_kdata( pKData );
 	cci.set_kdata( pKData );
	cv.set_kdata( pKData );
 	arbr.set_kdata( pKData );
 	cr.set_kdata( pKData );
	osc.set_kdata( pKData );
	// 	uos.set_kdata( pKData );
	// 	maosc.set_kdata( pKData );
	bias36.set_kdata( pKData );
	// 	dpo.set_kdata( pKData );
	// 	kst.set_kdata( pKData );
	// 
	// 	rei.set_kdata( pKData );
	// 	dmki.set_kdata( pKData );
	// 	pcnt.set_kdata( pKData );
	// 	hlc.set_kdata( pKData );
	// 	cdp.set_kdata( pKData );
	 	asi.set_kdata( pKData );
	// 	atr.set_kdata( pKData );
	// 
	// 	cyo.set_kdata( pKData );
	// 	dcyo.set_kdata( pKData );
	// 	hsl.set_kdata( pKData );
	// 	dper.set_kdata( pKData );
}

void CTechParameters::SetDefaultParametersAll( )
{
 	ma.SetDefaultParameters( );
 	bbi.SetDefaultParameters( );
 	boll.SetDefaultParameters( );
 	pv.SetDefaultParameters( );
 	sar.SetDefaultParameters( );
	dj.SetDefaultParameters( );
	cw.SetDefaultParameters( );

 	macd.SetDefaultParameters( );
 	mike.SetDefaultParameters( );
 	psy.SetDefaultParameters( );

	volume.SetDefaultParameters( );
	// 	nvi.SetDefaultParameters( );
	// 	pvi.SetDefaultParameters( );
	vr.SetDefaultParameters( );
	// 	vroc.SetDefaultParameters( );
	obv.SetDefaultParameters( );
	// 	mobv.SetDefaultParameters( );
	// 	mfi.SetDefaultParameters( );
	// 	vmacd.SetDefaultParameters( );
 	wvad.SetDefaultParameters( );
 	emv.SetDefaultParameters( );
	// 	vrsi.SetDefaultParameters( );
	// 	nvrsi.SetDefaultParameters( );
	// 	ad.SetDefaultParameters( );
	// 	ci.SetDefaultParameters( );
	// 
 	kdj.SetDefaultParameters( );
 	r.SetDefaultParameters( );
 	rsi.SetDefaultParameters( );
 	bias.SetDefaultParameters( );
 	mtm.SetDefaultParameters( );
 	dmi.SetDefaultParameters( );
 	roc.SetDefaultParameters( );
 	cci.SetDefaultParameters( );
	cv.SetDefaultParameters( );
 	arbr.SetDefaultParameters( );
 	cr.SetDefaultParameters( );
	osc.SetDefaultParameters( );
	// 	uos.SetDefaultParameters( );
	// 	maosc.SetDefaultParameters( );
	bias36.SetDefaultParameters( );
	// 	dpo.SetDefaultParameters( );
	// 	kst.SetDefaultParameters( );
	// 
	// 	rei.SetDefaultParameters( );
	// 	dmki.SetDefaultParameters( );
	// 	pcnt.SetDefaultParameters( );
	// 	hlc.SetDefaultParameters( );
	// 	cdp.SetDefaultParameters( );
		asi.SetDefaultParameters( );
	// 	atr.SetDefaultParameters( );
	// 
	// 	cyo.SetDefaultParameters( );
	// 	dcyo.SetDefaultParameters( );
	// 	hsl.SetDefaultParameters( );
	// 	dper.SetDefaultParameters( );
	// 
	// 	AfxGetProfile().StoreProfile( );
}

bool CTechParameters::FindParameters( UINT nTech, TechnicalIndicator * pTech )
{
	switch( nTech )
	{
	case STT_MA:		((CMA *)pTech)->attach( ma );		break;
	case STT_BBI:		((CBBI *)pTech)->attach( bbi );		break;
	case STT_BOLL:		((CBOLL *)pTech)->attach( boll );		break;
	case STT_PV:		((CPV *)pTech)->attach( pv );		break;
	case STT_SAR:		((CSAR *)pTech)->attach( sar );		break;
	case STT_DJ:		((CDJ *)pTech)->attach( dj );		break;
	case STT_CW:		((CCW *)pTech)->attach( cw );		break;
// 
	case STT_MACD:		((CMACD *)pTech)->attach( macd );		break;
	case STT_MIKE:		((CMIKE *)pTech)->attach( mike );		break;
	case STT_PSY:		((CPSY *)pTech)->attach( psy );		break;
// 		// 
	case STT_VOLUME:	((CVOLUME *)pTech)->attach( volume );		break;
// 		// 	case STT_NVI:		((CNVI *)pTech)->attach( nvi );		break;
// 		// 	case STT_PVI:		((CPVI *)pTech)->attach( pvi );		break;
// 		// 	case STT_VR:		((CVR *)pTech)->attach( vr );		break;
// 		// 	case STT_VROC:		((CVROC *)pTech)->attach( vroc );		break;
	case STT_OBV:		((COBV *)pTech)->attach( obv );		break;
// 		// 	case STT_MOBV:		((CMOBV *)pTech)->attach( mobv );		break;
// 		// 	case STT_MFI:		((CMFI *)pTech)->attach( mfi );		break;
// 		// 	case STT_VMACD:		((CVMACD *)pTech)->attach( vmacd );		break;
	case STT_WVAD:		((CWVAD *)pTech)->attach( wvad );		break;
	case STT_EMV:		((CEMV *)pTech)->attach( emv );		break;
		// 	case STT_VRSI:		((CVRSI *)pTech)->attach( vrsi );		break;
		// 	case STT_NVRSI:		((CNVRSI *)pTech)->attach( nvrsi );		break;
		// 	case STT_AD:		((CAD *)pTech)->attach( ad );		break;
	//case STT_CI:		((CCI *)pTech)->attach( ci );		break;
		// 
	case STT_KDJ:		((CKDJ *)pTech)->attach( kdj );		break;
	case STT_R:			((CR *)pTech)->attach( r );		break;
	case STT_RSI:		((CRSI *)pTech)->attach( rsi );		break;
	case STT_BIAS:		((CBIAS *)pTech)->attach( bias );		break;
	case STT_MTM:		((CMTM *)pTech)->attach( mtm );		break;
	case STT_DMI:		((CDMI *)pTech)->attach( dmi );		break;
	case STT_ROC:		((CROC *)pTech)->attach( roc );		break;
	case STT_CCI:		((CCCI *)pTech)->attach( cci );		break;
	case STT_CV:		((CCV *)pTech)->attach( cv );		break;
	case STT_ARBR:		((CARBR *)pTech)->attach( arbr );		break;
	case STT_CR:		((CCR *)pTech)->attach( cr );		break;
	case STT_OSC:		((COSC *)pTech)->attach( osc );		break;
		// 	case STT_UOS:		((CUOS *)pTech)->attach( uos );		break;
		// 	case STT_MAOSC:		((CMAOSC *)pTech)->attach( maosc );		break;
	case STT_36BIAS:	((C36BIAS *)pTech)->attach( bias36 );		break;
		// 	case STT_DPO:		((CDPO *)pTech)->attach( dpo );		break;
		// 	case STT_KST:		((CKST *)pTech)->attach( kst );		break;
		// 
		// 	case STT_REI:		((CREI *)pTech)->attach( rei );		break;
		// 	case STT_DMKI:		((CDMKI *)pTech)->attach( dmki );		break;
		// 	case STT_PCNT:		((CPCNT *)pTech)->attach( pcnt );		break;
		// 	case STT_HLC:		((CHLC *)pTech)->attach( hlc );		break;
		// 	case STT_CDP:		((CCDP *)pTech)->attach( cdp );		break;
		// 	case STT_ASI:		((CASI *)pTech)->attach( asi );		break;
		// 	case STT_ATR:		((CATR *)pTech)->attach( atr );		break;
		// 
		// 	case STT_CYO:		((CCYO *)pTech)->attach( cyo );		break;
		// 	case STT_DCYO:		((CDCYO *)pTech)->attach( dcyo );		break;
		// 	case STT_HSL:		((CHSL *)pTech)->attach( hsl );		break;
		// 	case STT_DPER:		((CDPER *)pTech)->attach( dper );		break;
		// 
 	default:
		 	assert( false );
		 	return false;
	}
	return true;
}

TechnicalIndicator* CTechParameters::operator[]( size_t indicator_index )
{
	TechnicalIndicator* pIndictor=NULL;
	std::map<size_t,TechnicalIndicator*>::iterator tech_iter=m_mapIndicators.find(indicator_index);
	if (tech_iter!=m_mapIndicators.end())
	{
		return tech_iter->second;
	}
	else
	{
		pIndictor=TechnicalIndicator::create(indicator_index,NULL);
		m_mapIndicators[indicator_index]=pIndictor;
	}
	return pIndictor;
}

// void CTechParameters::Serialize(CArchive& ar)
// {
// 	if( ar.IsStoring() )
// 	{
// 		// CMA
// 		ar << ma.m_nType;
// 		ma.m_adwMADays.Serialize( ar );
// 		ar << ma.m_itsGoldenFork;
// 		ar << ma.m_itsDeadFork;
// 		ar << ma.m_itsLong;
// 		ar << ma.m_itsShort;
// 
// 		// CBBI
// 		ar << bbi.m_nMA1Days;
// 		ar << bbi.m_nMA2Days;
// 		ar << bbi.m_nMA3Days;
// 		ar << bbi.m_nMA4Days;
// 		ar << bbi.m_itsGoldenFork;
// 		ar << bbi.m_itsDeadFork;
// 
// 		// CBOLL
// 		ar << boll.m_dMultiUp;
// 		ar << boll.m_dMultiDown;
// 		ar << boll.m_nMADays;
// 		ar << boll.m_itsSupport;
// 		ar << boll.m_itsResistance;
// 
// 		// CPV
// 		
// 		// CSAR
// 		ar << sar.m_nInitDays;
// 		ar << sar.m_bFirstUp;
// 		ar << sar.m_dAFStep;
// 		ar << sar.m_dAFMax;
// 		ar << sar.m_itsBuy;
// 		ar << sar.m_itsSell;
// 
// 		// CDJ
// 		//ar << dj.m_strCodeSha;
// 		//ar << dj.m_strCodeSzn;
// 
// 		//// CCW
// 		//ar << cw.m_dChangeHand;
// 
// 		// CMACD
// 		ar << macd.m_nEMA1Days;
// 		ar << macd.m_nEMA2Days;
// 		ar << macd.m_nDIFDays;
// 		ar << macd.m_itsDeviateOnBottom;
// 		ar << macd.m_itsDeviateOnTop;
// 		ar << macd.m_itsGoldenFork;
// 		ar << macd.m_itsDeadFork;
// 
// 		// CMIKE
// 		ar << mike.m_nDays;
// 
// 		// CPSY
// 		ar << psy.m_nDays;
// 		ar << psy.m_itsSold;
// 		ar << psy.m_itsBought;
// 
// 		// CVOLUME
// 		volume.m_adwMADays.Serialize( ar );
// 		ar << volume.m_itsDeviateOnBottom;
// 		ar << volume.m_itsDeviateOnTop;
// 		ar << volume.m_itsLong;
// 		ar << volume.m_itsShort;
// 
// 		// CNVI
// 		//ar << nvi.m_nMADays;
// 		//ar << nvi.m_itsGoldenFork;
// 		//ar << nvi.m_itsDeadFork;
// 
// 		//// CPVI
// 		//ar << pvi.m_nMADays;
// 		//ar << pvi.m_itsGoldenFork;
// 		//ar << pvi.m_itsDeadFork;
// 
// 		// CVR
// 		ar << vr.m_nDays;
// 		ar << vr.m_itsLong;
// 		ar << vr.m_itsShort;
// 
// 		//// CVROC
// 		//ar << vroc.m_nDays;
// 		//ar << vroc.m_nMADays;
// 		//ar << vroc.m_itsDeviateOnBottom;
// 		//ar << vroc.m_itsDeviateOnTop;
// 
// 		// COBV
// 		ar << obv.m_itsDeviateOnBottom;
// 		ar << obv.m_itsDeviateOnTop;
// 
// 		// CMOBV
// 		//ar << mobv.m_nDays1;
// 		//ar << mobv.m_nDays2;
// 		//ar << mobv.m_itsGoldenFork;
// 		//ar << mobv.m_itsDeadFork;
// 
// 		//// CMFI
// 		//ar << mfi.m_nDays;
// 		//ar << mfi.m_itsLong;
// 		//ar << mfi.m_itsShort;
// 
// 		//// CVMACD
// 		//ar << vmacd.m_nEMA1Days;
// 		//ar << vmacd.m_nEMA2Days;
// 		//ar << vmacd.m_nDIFDays;
// 		//ar << vmacd.m_itsDeviateOnBottom;
// 		//ar << vmacd.m_itsDeviateOnTop;
// 		//ar << vmacd.m_itsGoldenFork;
// 		//ar << vmacd.m_itsDeadFork;
// 
// 		// CWVAD
// 		ar << wvad.m_nDays;
// 		ar << wvad.m_itsLong;
// 		ar << wvad.m_itsShort;
// 
// 		// CEMV
// 		ar << emv.m_nDays;
// 		ar << emv.m_nMADays;
// 		ar << emv.m_itsGoldenFork;
// 		ar << emv.m_itsDeadFork;
// 
// 		//// CVRSI
// 		//ar << vrsi.m_nDays;
// 		//ar << vrsi.m_itsDeviateOnBottom;
// 		//ar << vrsi.m_itsDeviateOnTop;
// 		//ar << vrsi.m_itsSold;
// 		//ar << vrsi.m_itsBought;
// 
// 		//// CNVRSI
// 		//ar << nvrsi.m_nDays;
// 		//ar << nvrsi.m_itsDeviateOnBottom;
// 		//ar << nvrsi.m_itsDeviateOnTop;
// 		//ar << nvrsi.m_itsSold;
// 		//ar << nvrsi.m_itsBought;
// 
// 		//// CAD
// 		//ar << ad.m_nDays;
// 
// 		// CCI
// 
// 		// CKDJ
// 		ar << kdj.m_nRSVDays;
// 		ar << kdj.m_nKDays;
// 		ar << kdj.m_nDDays;
// 		ar << kdj.m_nJ;
// 		ar << kdj.m_itsGoldenFork;
// 		ar << kdj.m_itsDeadFork;
// 
// 		// CR
// 		ar << r.m_nDays;
// 
// 		// CRSI
// 		rsi.m_adwDays.Serialize( ar );
// 		ar << rsi.m_itsSold;
// 		ar << rsi.m_itsGoldenFork;
// 		ar << rsi.m_itsDeadFork;
// 
// 		// CBIAS
// 		ar << bias.m_nDays;
// 		ar << bias.m_itsSold;
// 		ar << bias.m_itsBought;
// 
// 		// CMTM
// 		ar << mtm.m_nDays;
// 		ar << mtm.m_nMADays;
// 		ar << mtm.m_itsGoldenFork;
// 		ar << mtm.m_itsDeadFork;
// 
// 		// CDMI
// 		ar << dmi.m_nDays;
// 
// 		// CROC
// 		ar << roc.m_nDays;
// 		ar << roc.m_nMADays;
// 		ar << roc.m_itsGoldenFork;
// 		ar << roc.m_itsDeadFork;
// 
// 		// CCCI
// 		ar << cci.m_nDays;
// 		ar << cci.m_dQuotiety;
// 		ar << cci.m_nMADays;
// 		ar << cci.m_itsGoldenFork;
// 		ar << cci.m_itsDeadFork;
// 
// 		//// CCV
// 		//ar << cv.m_nMAHLDays;
// 		//ar << cv.m_nCVDays;
// 		//ar << cv.m_itsSold;
// 		//ar << cv.m_itsBought;
// 
// 		// CARBR
// 		ar << arbr.m_nDays;
// 		ar << arbr.m_itsGoldenFork;
// 		ar << arbr.m_itsDeadFork;
// 		ar << arbr.m_itsSold;
// 		ar << arbr.m_itsBought;
// 
// 		// CCR
// 		ar << cr.m_nDays;
// 		ar << cr.m_nMADaysA;
// 		ar << cr.m_nMADaysB;
// 		ar << cr.m_nMADaysC;
// 		ar << cr.m_nMADaysD;
// 		ar << cr.m_itsSold;
// 		ar << cr.m_itsBought;
// 
// 		// COSC
// 		ar << osc.m_nDays;
// 		ar << osc.m_nMADays;
// 		ar << osc.m_itsGoldenFork;
// 		ar << osc.m_itsDeadFork;
// 
// 		//// CUOS
// 		//ar << uos.m_nDays1;
// 		//ar << uos.m_nDays2;
// 		//ar << uos.m_nDays3;
// 		//ar << uos.m_nMADays;
// 		//ar << uos.m_itsGoldenFork;
// 		//ar << uos.m_itsDeadFork;
// 
// 		//// CMAOSC
// 		//ar << maosc.m_nDays1;
// 		//ar << maosc.m_nDays2;
// 		//ar << maosc.m_nMADays;
// 		//ar << maosc.m_itsGoldenFork;
// 		//ar << maosc.m_itsDeadFork;
// 
// 		// C36BIAS
// 		ar << bias36.m_itsSold;
// 		ar << bias36.m_itsBought;
// 
// 		//// CDPO
// 		//ar << dpo.m_nDays;
// 		//ar << dpo.m_nMADays;
// 		//ar << dpo.m_itsGoldenFork;
// 		//ar << dpo.m_itsDeadFork;
// 
// 		//// CKST
// 		//kst.m_adwROCDays.Serialize( ar );
// 		//ar << kst.m_nMADays;
// 		//ar << kst.m_itsGoldenFork;
// 		//ar << kst.m_itsDeadFork;
// 
// 		//// CREI
// 		//ar << rei.m_nDays;
// 		//ar << rei.m_itsLong;
// 		//ar << rei.m_itsShort;
// 
// 		//// CDMKI
// 		//ar << dmki.m_nDays;
// 		//ar << dmki.m_itsSold;
// 		//ar << dmki.m_itsBought;
// 
// 		//// CPCNT
// 		//ar << pcnt.m_nMADays;
// 		//ar << pcnt.m_itsGoldenFork;
// 		//ar << pcnt.m_itsDeadFork;
// 
// 		//// CHLC
// 		//ar << hlc.m_nDays;
// 		//ar << hlc.m_nMADays;
// 		//ar << hlc.m_itsGoldenFork;
// 		//ar << hlc.m_itsDeadFork;
// 
// 		// CCDP
// 		
// 		// CASI
// 		ar << asi.m_nDays;
// 		ar << asi.m_itsDeviateOnBottom;
// 		ar << asi.m_itsDeviateOnTop;
// 
// 		//// CATR
// 		//ar << atr.m_nDays;
// 		//ar << atr.m_itsSold;
// 		//ar << atr.m_itsBought;
// 		
// 		//// CCYO
// 		//cyo.m_adwMTMDays.Serialize( ar );
// 		//ar << cyo.m_nMADays;
// 		//ar << cyo.m_itsGoldenFork;
// 		//ar << cyo.m_itsDeadFork;
// 
// 		//// CDCYO
// 		//dcyo.m_adwMTMDays.Serialize( ar );
// 		//ar << dcyo.m_nMADays;
// 		//ar << dcyo.m_itsGoldenFork;
// 		//ar << dcyo.m_itsDeadFork;
// 
// 		//// CHSL
// 		//ar << hsl.m_nDays;
// 		//ar << hsl.m_nMADays;
// 		//ar << hsl.m_itsGoldenFork;
// 		//ar << hsl.m_itsDeadFork;
// 
// 		//// CDPER
// 		//ar << dper.m_nDays;
// 		//ar << dper.m_nMADays;
// 		//ar << dper.m_nDetrendDays;
// 		//ar << dper.m_itsGoldenFork;
// 		//ar << dper.m_itsDeadFork;
// 
// 		DWORD	dwTemp	=	(DWORD)(-1);
// 		for( int i=0; i<200; i++ )
// 			ar << dwTemp;
// 	}
// 	else
// 	{
// 		// CMA
// 		ar >> ma.m_nType;
// 		ma.m_adwMADays.Serialize( ar );
// 		ar >> ma.m_itsGoldenFork;
// 		ar >> ma.m_itsDeadFork;
// 		ar >> ma.m_itsLong;
// 		ar >> ma.m_itsShort;
// 
// 		//// CBBI
// 		ar >> bbi.m_nMA1Days;
// 		ar >> bbi.m_nMA2Days;
// 		ar >> bbi.m_nMA3Days;
// 		ar >> bbi.m_nMA4Days;
// 		ar >> bbi.m_itsGoldenFork;
// 		ar >> bbi.m_itsDeadFork;
// 
// 		// CBOLL
// 		ar >> boll.m_dMultiUp;
// 		ar >> boll.m_dMultiDown;
// 		ar >> boll.m_nMADays;
// 		ar >> boll.m_itsSupport;
// 		ar >> boll.m_itsResistance;
// 
// 		// CPV
// 		
// 		// CSAR
// 		ar >> sar.m_nInitDays;
// 		ar >> sar.m_bFirstUp;
// 		ar >> sar.m_dAFStep;
// 		ar >> sar.m_dAFMax;
// 		ar >> sar.m_itsBuy;
// 		ar >> sar.m_itsSell;
// 
// 		//// CDJ
// 		//ar >> dj.m_strCodeSha;
// 		//ar >> dj.m_strCodeSzn;
// 
// 		//// CCW
// 		//ar >> cw.m_dChangeHand;
// 
// 		// CMACD
// 		ar >> macd.m_nEMA1Days;
// 		ar >> macd.m_nEMA2Days;
// 		ar >> macd.m_nDIFDays;
// 		ar >> macd.m_itsDeviateOnBottom;
// 		ar >> macd.m_itsDeviateOnTop;
// 		ar >> macd.m_itsGoldenFork;
// 		ar >> macd.m_itsDeadFork;
// 
// 		// CMIKE
// 		ar >> mike.m_nDays;
// 
// 		// CPSY
// 		ar >> psy.m_nDays;
// 		ar >> psy.m_itsSold;
// 		ar >> psy.m_itsBought;
// 
// 		// CVOLUME
// 		volume.m_adwMADays.Serialize( ar );
// 		ar >> volume.m_itsDeviateOnBottom;
// 		ar >> volume.m_itsDeviateOnTop;
// 		ar >> volume.m_itsLong;
// 		ar >> volume.m_itsShort;
// 
// 		//// CNVI
// 		//ar >> nvi.m_nMADays;
// 		//ar >> nvi.m_itsGoldenFork;
// 		//ar >> nvi.m_itsDeadFork;
// 
// 		//// CPVI
// 		//ar >> pvi.m_nMADays;
// 		//ar >> pvi.m_itsGoldenFork;
// 		//ar >> pvi.m_itsDeadFork;
// 
// 		// CVR
// 		ar >> vr.m_nDays;
// 		ar >> vr.m_itsLong;
// 		ar >> vr.m_itsShort;
// 
// 		//// CVROC
// 		//ar >> vroc.m_nDays;
// 		//ar >> vroc.m_nMADays;
// 		//ar >> vroc.m_itsDeviateOnBottom;
// 		//ar >> vroc.m_itsDeviateOnTop;
// 
// 		// COBV
// 		ar >> obv.m_itsDeviateOnBottom;
// 		ar >> obv.m_itsDeviateOnTop;
// 
// 		// CMOBV
// 		//ar >> mobv.m_nDays1;
// 		//ar >> mobv.m_nDays2;
// 		//ar >> mobv.m_itsGoldenFork;
// 		//ar >> mobv.m_itsDeadFork;
// 
// 		//// CMFI
// 		//ar >> mfi.m_nDays;
// 		//ar >> mfi.m_itsLong;
// 		//ar >> mfi.m_itsShort;
// 
// 		//// CVMACD
// 		//ar >> vmacd.m_nEMA1Days;
// 		//ar >> vmacd.m_nEMA2Days;
// 		//ar >> vmacd.m_nDIFDays;
// 		//ar >> vmacd.m_itsDeviateOnBottom;
// 		//ar >> vmacd.m_itsDeviateOnTop;
// 		//ar >> vmacd.m_itsGoldenFork;
// 		//ar >> vmacd.m_itsDeadFork;
// 
// 		// CWVAD
// 		ar >> wvad.m_nDays;
// 		ar >> wvad.m_itsLong;
// 		ar >> wvad.m_itsShort;
// 
// 		// CEMV
// 		ar >> emv.m_nDays;
// 		ar >> emv.m_nMADays;
// 		ar >> emv.m_itsGoldenFork;
// 		ar >> emv.m_itsDeadFork;
// 
// 		//// CVRSI
// 		//ar >> vrsi.m_nDays;
// 		//ar >> vrsi.m_itsDeviateOnBottom;
// 		//ar >> vrsi.m_itsDeviateOnTop;
// 		//ar >> vrsi.m_itsSold;
// 		//ar >> vrsi.m_itsBought;
// 
// 		//// CNVRSI
// 		//ar >> nvrsi.m_nDays;
// 		//ar >> nvrsi.m_itsDeviateOnBottom;
// 		//ar >> nvrsi.m_itsDeviateOnTop;
// 		//ar >> nvrsi.m_itsSold;
// 		//ar >> nvrsi.m_itsBought;
// 
// 		//// CAD
// 		//ar >> ad.m_nDays;
// 
// 		// CCI
// 
// 		// CKDJ
// 		ar >> kdj.m_nRSVDays;
// 		ar >> kdj.m_nKDays;
// 		ar >> kdj.m_nDDays;
// 		ar >> kdj.m_nJ;
// 		ar >> kdj.m_itsGoldenFork;
// 		ar >> kdj.m_itsDeadFork;
// 
// 		// CR
// 		ar >> r.m_nDays;
// 
// 		// CRSI
// 		rsi.m_adwDays.Serialize( ar );
// 		ar >> rsi.m_itsSold;
// 		ar >> rsi.m_itsGoldenFork;
// 		ar >> rsi.m_itsDeadFork;
// 
// 		// CBIAS
// 		ar >> bias.m_nDays;
// 		ar >> bias.m_itsSold;
// 		ar >> bias.m_itsBought;
// 
// 		// CMTM
// 		ar >> mtm.m_nDays;
// 		ar >> mtm.m_nMADays;
// 		ar >> mtm.m_itsGoldenFork;
// 		ar >> mtm.m_itsDeadFork;
// 
// 		// CDMI
// 		ar >> dmi.m_nDays;
// 
// 		// CROC
// 		ar >> roc.m_nDays;
// 		ar >> roc.m_nMADays;
// 		ar >> roc.m_itsGoldenFork;
// 		ar >> roc.m_itsDeadFork;
// 
// 		// CCCI
// 		ar >> cci.m_nDays;
// 		ar >> cci.m_dQuotiety;
// 		ar >> cci.m_nMADays;
// 		ar >> cci.m_itsGoldenFork;
// 		ar >> cci.m_itsDeadFork;
// 
// 		//// CCV
// 		ar >> cv.m_nMAHLDays;
// 		ar >> cv.m_nCVDays;
// 		ar >> cv.m_itsSold;
// 		ar >> cv.m_itsBought;
// 
// 		// CARBR
// 		ar >> arbr.m_nDays;
// 		ar >> arbr.m_itsGoldenFork;
// 		ar >> arbr.m_itsDeadFork;
// 		ar >> arbr.m_itsSold;
// 		ar >> arbr.m_itsBought;
// 
// 		// CCR
// 		ar >> cr.m_nDays;
// 		ar >> cr.m_nMADaysA;
// 		ar >> cr.m_nMADaysB;
// 		ar >> cr.m_nMADaysC;
// 		ar >> cr.m_nMADaysD;
// 		ar >> cr.m_itsSold;
// 		ar >> cr.m_itsBought;
// 
// 		// COSC
// 		ar >> osc.m_nDays;
// 		ar >> osc.m_nMADays;
// 		ar >> osc.m_itsGoldenFork;
// 		ar >> osc.m_itsDeadFork;
// 
// 		//// CUOS
// 		//ar >> uos.m_nDays1;
// 		//ar >> uos.m_nDays2;
// 		//ar >> uos.m_nDays3;
// 		//ar >> uos.m_nMADays;
// 		//ar >> uos.m_itsGoldenFork;
// 		//ar >> uos.m_itsDeadFork;
// 
// 		//// CMAOSC
// 		//ar >> maosc.m_nDays1;
// 		//ar >> maosc.m_nDays2;
// 		//ar >> maosc.m_nMADays;
// 		//ar >> maosc.m_itsGoldenFork;
// 		//ar >> maosc.m_itsDeadFork;
// 
// 		//// C36BIAS
// 		ar >> bias36.m_itsSold;
// 		ar >> bias36.m_itsBought;
// 
// 		//// CDPO
// 		//ar >> dpo.m_nDays;
// 		//ar >> dpo.m_nMADays;
// 		//ar >> dpo.m_itsGoldenFork;
// 		//ar >> dpo.m_itsDeadFork;
// 
// 		// CKST
// 		//kst.m_adwROCDays.Serialize( ar );
// 		//ar >> kst.m_nMADays;
// 		//ar >> kst.m_itsGoldenFork;
// 		//ar >> kst.m_itsDeadFork;
// 
// 		//// CREI
// 		//ar >> rei.m_nDays;
// 		//ar >> rei.m_itsLong;
// 		//ar >> rei.m_itsShort;
// 
// 		//// CDMKI
// 		//ar >> dmki.m_nDays;
// 		//ar >> dmki.m_itsSold;
// 		//ar >> dmki.m_itsBought;
// 
// 		//// CPCNT
// 		//ar >> pcnt.m_nMADays;
// 		//ar >> pcnt.m_itsGoldenFork;
// 		//ar >> pcnt.m_itsDeadFork;
// 
// 		//// CHLC
// 		//ar >> hlc.m_nDays;
// 		//ar >> hlc.m_nMADays;
// 		//ar >> hlc.m_itsGoldenFork;
// 		//ar >> hlc.m_itsDeadFork;
// 
// 		// CCDP
// 		
// 		// CASI
// 		ar >> asi.m_nDays;
// 		ar >> asi.m_itsDeviateOnBottom;
// 		ar >> asi.m_itsDeviateOnTop;
// 
// 		//// CATR
// 		//ar >> atr.m_nDays;
// 		//ar >> atr.m_itsSold;
// 		//ar >> atr.m_itsBought;
// 		
// 		//// CCYO
// 		//cyo.m_adwMTMDays.Serialize( ar );
// 		//ar >> cyo.m_nMADays;
// 		//ar >> cyo.m_itsGoldenFork;
// 		//ar >> cyo.m_itsDeadFork;
// 
// 		//// CDCYO
// 		//dcyo.m_adwMTMDays.Serialize( ar );
// 		//ar >> dcyo.m_nMADays;
// 		//ar >> dcyo.m_itsGoldenFork;
// 		//ar >> dcyo.m_itsDeadFork;
// 
// 		//// CHSL
// 		//ar >> hsl.m_nDays;
// 		//ar >> hsl.m_nMADays;
// 		//ar >> hsl.m_itsGoldenFork;
// 		//ar >> hsl.m_itsDeadFork;
// 
// 		//// CDPER
// 		//ar >> dper.m_nDays;
// 		//ar >> dper.m_nMADays;
// 		//ar >> dper.m_nDetrendDays;
// 		//ar >> dper.m_itsGoldenFork;
// 		//ar >> dper.m_itsDeadFork;
// 
// 		DWORD	dwTemp;
// 		for( int i=0; i<200; i++ )
// 			ar >> dwTemp;
// 	}
// }

