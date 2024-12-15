// StockGraph.cpp : implementation of the CStockGraph class
//

#include "stdafx.h"
#include <winuser.h>
#include "StockGraph.h"
#include "../Dialog/EditKDATADlg.h"
#include <math.h>

//#define _IS_DEBUG
//#define   _FLOAT_WND_
//#define  _E_DRAW_  

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	RT_DETERMINE_COLOR(x,y)	((x)-(y)>1e-4?clrRise:((x)-(y)<1e-4?clrFall:clrPlane))

#define REPORT_WIDTH      174
#define KLINE_OFFSET      60  // 82
#define UNIT_WIDTH        40

/////////////////////////////////////////////////////////////////////////////
// Some Useful static function

void DrawLine( CDC * pDC, int nWidth, COLORREF clr, int xStart, int yStart, int xEnd, int yEnd )
{
	CPen	pen( PS_SOLID, nWidth, clr );
	CPen *pOldPen = pDC->SelectObject( &pen );
	pDC->MoveTo( xStart, yStart );
	pDC->LineTo( xEnd, yEnd );
	pDC->SelectObject( pOldPen );
}

void DrawDashLine( CDC * pDC, int nWidth, COLORREF clr, int xStart, int yStart, int xEnd, int yEnd )
{
	CPen	pen( PS_SOLID, nWidth, clr );
	CPen *pOldPen = pDC->SelectObject( &pen );
	int	x = xStart, y = yStart;
	while( x < xEnd || y < yEnd )
	{
		if( x > xEnd )	x	=	xEnd;
		if( y > yEnd )	y	=	yEnd;
		pDC->MoveTo( x, y );
		int	xNew = x+3;
		int	yNew = y+3;
		if( xNew > xEnd )	xNew	=	xEnd;
		if( yNew > yEnd )	yNew	=	yEnd;
		pDC->LineTo( xNew, yNew );
		x	=	xNew+3;
		y	=	yNew+3;
	}
	pDC->SelectObject( pOldPen );
}

void DrawDashAxis( CDC * pDC, int nWidth, COLORREF clr, int nCount, int xStart, int yStart, int xEnd, int yEnd )
{
	int	nHeight	=	yEnd - yStart;
	if( nHeight <= 0 )
		return;
	if( nCount <= 0 )
		return;
	for( int k=0; k<nCount; k++ )
		DrawDashLine( pDC, nWidth, clr, xStart, yStart+k*nHeight/nCount, xEnd, yStart+k*nHeight/nCount );
}

void DrawAxisNumber(CDC * pDC, COLORREF clr, int nCount,
					int xStart, int yStart, int xEnd, int yEnd,
					double fMin, double fMax, BOOL bDrawMin, BOOL bDrawMax, int nDotCount )
{
	int	nHeight	=	yEnd - yStart;
	if( nHeight <= 0 )
		return;
	if( nCount <= 0 )
		return;
	ASSERT( nDotCount >= 0 );

	CString	strFmt;
	strFmt.Format( _T("%%.%df"), nDotCount );

	pDC->SetTextColor( clr );
	CString	strText;
	CSize	sizeText;
	
	for( int k=0; k<=nCount; k++ )
	{
		if( !bDrawMin && 0 == k )
			continue;
		if( !bDrawMax && nCount == k )
			continue;
		double	dTemp	=	fMin+k*(fMax-fMin)/nCount;
		if( dTemp < 0 && dTemp > (0-0.6*pow((double)10,0-nDotCount)) )
			dTemp	=	0;
		strText.Format( strFmt, dTemp );
		sizeText	=	pDC->GetTextExtent( strText );
		pDC->TextOut( xStart-sizeText.cx-1, yStart+(nCount-k)*nHeight/nCount-7, strText );
	}
}

BOOL DrawAxis(CDC * pDC, CRect rect, int nMarginTop, int nAxisLines, double dMin, double dMax, BOOL bDrawMin, BOOL bDrawMax, int nDotCount )
{
	DECLARE_COLOR_DEFINATION

	if( rect.Height() <= nMarginTop )
		return FALSE;

	// Draw Axis
	pDC->Draw3dRect( &rect, clrBorder, clrBorder );
	DrawDashAxis(	pDC, 1, clrBorder, nAxisLines,
					rect.left, rect.top+nMarginTop, rect.right, rect.bottom );

	// Draw Axis Number, Will Extent the rect
	CFont	* pOldFont	=	AfxSelectDCFont( pDC, 12 );
	pDC->SetBkColor( clrBK );
	DrawAxisNumber( pDC, clrText, nAxisLines,
					rect.left, rect.top+nMarginTop, rect.right, rect.bottom,
					dMin, dMax, bDrawMin, bDrawMax, nDotCount );
	pDC->SelectObject( pOldFont );
	return TRUE;
}

//显示指标标题
BOOL DrawTechTitle(CDC * pDC, int x, int y, LPCTSTR lpszText,
				   UINT nTextAlign, int nFontSize, COLORREF clrBK, COLORREF clrText )
{
	CFont	* pOldFont	=	AfxSelectDCFont( pDC, nFontSize );
	pDC->SetBkColor( clrBK );
	pDC->SetTextColor( clrText );
	UINT nOldAlign = pDC->SetTextAlign( nTextAlign );
	pDC->TextOut( x, y, lpszText );
	pDC->SetTextAlign( nOldAlign );
	pDC->SelectObject( pOldFont );
	return TRUE;
}

BOOL FixRect( CRect & rect, LPRECT lpRectMax )
{
	if( lpRectMax )
	{
		if( rect.left < lpRectMax->left )
			rect.left	=	lpRectMax->left;
		if( rect.top < lpRectMax->top )
			rect.top	=	lpRectMax->top;
		if( rect.right > lpRectMax->right )
			rect.right	=	lpRectMax->right;
		if( rect.bottom > lpRectMax->bottom )
			rect.bottom	=	lpRectMax->bottom;
		return TRUE;
	}
	return FALSE;
}

BOOL DrawTextWithRect(CDC *pDC, int left, int top, int right, int bottom, LPCTSTR lpszText,
					  COLORREF clrRect, COLORREF clrText, COLORREF clrBK, LPRECT lpRectMax )
{
	CRect	rectDraw	=	CRect( left, top, right, bottom );
	if( lpRectMax )
		FixRect( rectDraw, lpRectMax );
	pDC->Draw3dRect( &rectDraw, clrRect, clrRect );
	pDC->SetTextColor( clrText );
	pDC->SetBkColor( clrBK );
	pDC->DrawText( lpszText, rectDraw, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	return TRUE;
}

BOOL DrawTextWithRect(CDC *pDC, int left, int top, int right, int bottom, UINT nIDString,
					  COLORREF clrRect, COLORREF clrText, COLORREF clrBK, LPRECT lpRectMax )
{
	CString	string;
	string.LoadString( nIDString );
	return DrawTextWithRect( pDC, left, top, right, bottom, string, clrRect, clrText, clrBK, lpRectMax );
}

/////////////////////////////////////////////////////////////////////////////
// CStockGraph

CStockGraph::CStockGraph( )
{
	m_nCrossCursorPos =	CPoint(0,0);
	m_HighPrice.clear();
	m_LowPrice.clear();
	m_HighIndex.clear();
	m_LowIndex.clear();

	ResetMargins( );
    m_bInvalidate   =   TRUE;
	m_pParent		=	NULL;
	m_nThickness	=	m_nDefaultThickness; // 6 K线柱体宽度
	m_dwLatestDate	=	-1;
    m_nDotCount     =   2;
	m_ptSelectBegin	=	CPoint(0,0);
	m_ptSelectEnd	=	CPoint(0,0);
    m_ptLastMovingLine	=	CPoint(0,0);
	m_nReportWhat	=	CStockGraph::reportParam;

	SetKType( );
	SetKFormat( AfxGetProfile().GetGraphKFormat() );
	SetMaindataType( AfxGetProfile().GetGraphMaindataType() );
	SetKLineMode( CStockGraph::klineCandle ); //蜡烛形状
	ResetIndex( );

	m_rectAll.SetRectEmpty();

	m_dMaxPrice			=	0;
	m_dMinPrice			=	0;

	m_bCursorBorder		=	FALSE;

	m_nSelectedTechID	=	STT_INVALID;
	m_strBSDesc         = _T("");
}

CStockGraph::~CStockGraph( )
{
	m_HighPrice.clear();
	m_LowPrice.clear();
	m_HighIndex.clear();
	m_LowIndex.clear();
	ClearCurStock( );
}

void CStockGraph::SetParent( CWnd * pParent )
{
	m_pParent	=	pParent;
}

void CStockGraph::ResetMargins(	int nMarginTop, int nMarginBottom,
								int nHeightSubtitle, int nWidthSubtitle,
								int nWidthParameter, int nDefaultThickness )
{
	ASSERT( nMarginTop >= 0 && nMarginBottom >= 0 );
	ASSERT( nHeightSubtitle >= 0 && nWidthSubtitle >= 0 );
	ASSERT( nWidthParameter >= 0 );
	ASSERT( nDefaultThickness > 0 );

	m_nMarginTop		=	nMarginTop;
	m_nMarginBottom		=	nMarginBottom;
	m_nHeightSubtitle	=	nHeightSubtitle + 2;
	m_nWidthSubtitle	=	nWidthSubtitle;
	m_nWidthParameter	=	nWidthParameter;

    if (nDefaultThickness > 1 ) m_nDefaultThickness = (nDefaultThickness/2 ) * 2;
	// m_nDefaultThickness	=	nDefaultThickness ;

}

LRESULT CStockGraph::OnStkReceiverData( WPARAM wParam, LPARAM lParam )
{
	PCOMMPACKET	pCommPacket	=	(PCOMMPACKET)lParam;
    DWORD i;
	
	switch( wParam )
	{

	case CStock::dataReport:
#if 0    
        if( pCommPacket && CStock::dataReport == pCommPacket->m_dwDataType )
		{
			BOOL	bInv = FALSE;
			for( i=0; i<pCommPacket->m_dwCount; i++)
			{
				if( m_CurStock.GetStockInfo().IsEqualTo( pCommPacket->m_pReport[i].m_dwMarket, pCommPacket->m_pReport[i].m_szCode ) )
				{
					UpdateStockInfoByREPORT( m_CurStock.GetStockInfo(), &(pCommPacket->m_pReport[i]) );
					if( -1 != m_CurStock.GetReport().InsertReportSort( pCommPacket->m_pReport[i] ) )
					{
						MINUTE	minute;
						if( convert_REPORT_to_MINUTE( &(pCommPacket->m_pReport[i]), &minute ) )
							m_CurStock.GetMinute().InsertMinuteSort( minute );
						//m_nCurrentStartPos	=	-1;
						bInv = TRUE;
					}
					break;
				}
			}

			if( /*i != pCommPacket->m_dwCount &&*/ bInv )
			{
			// 加入最新一天的K线数据
				PrepareStockData( TRUE );
				if( m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()) )
					m_pParent->Invalidate();

			}
		}

#else
	    if( pCommPacket && CStock::dataReport == pCommPacket->m_dwDataType )
		{
			BOOL	bInv = FALSE;
			for( i=0; i<pCommPacket->m_dwCount; i++ )
			{
				if( m_CurStock.GetStockInfo().IsEqualTo( pCommPacket->m_pReport[i].m_dwMarket, pCommPacket->m_pReport[i].m_szCode ) )
				{
					bInv = TRUE;
					break;
				}
			}
			//if( bInv && m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()) )
			//{
			//	  // 加入最新一天的K线数据
			//	   PrepareStockData( TRUE );
			//	   m_pParent->Invalidate();//Rect(NULL,FALSE);
			//}

			if(bInv /*&& i != pCommPacket->m_dwCount */&& m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()))
			{
				// 加入最新一天的K线数据
				PrepareStockData( TRUE );
				//if( m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()) )
				//{
					m_pParent->InvalidateRect(NULL,FALSE);
				//}
			}
		}
#endif
		break;

	case CStock::dataK:
		if( pCommPacket && CStock::dataK == pCommPacket->m_dwDataType )
		{
			for(  i=0; i<pCommPacket->m_dwCount; i++ )
			{
				if( m_CurStock.GetStockInfo().IsEqualTo( pCommPacket->m_pKdata[i].m_dwMarket, pCommPacket->m_pKdata[i].m_szCode ) )
					break;
			}

			if( i != pCommPacket->m_dwCount && m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()) )
			{
				// 加入最新一天的K线数据
				PrepareStockData( TRUE );
				//AfxMessageBox(_T("加入最新一天的K线数据"));
				//if( m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()) )
				//{
					m_pParent->InvalidateRect(NULL,FALSE);
				//}
			}

		}
		break;
	case CStock::dataMinute:
		// WILLCHECK
		if( !CKData::IsDayOrMin(m_nCurKType) && pCommPacket && CStock::dataMinute == pCommPacket->m_dwDataType )
		{
			int	nOldSize	=	m_CurStock.GetMinute().GetSize();
			
			for( i=0; i<pCommPacket->m_dwCount; i++)
			{
				if( m_CurStock.GetStockInfo().IsEqualTo( pCommPacket->m_pMinute[i].m_dwMarket, pCommPacket->m_pMinute[i].m_szCode ) )
					break;
			}
			if( i != pCommPacket->m_dwCount && m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()) )
			{
				// 加入最新的分钟K线数据
				PrepareStockData( TRUE );
				//if( m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()) )
				//{
				   m_pParent->InvalidateRect(NULL,FALSE);
				//}
			}
		}
		break;
	}

	return 0L;
}

int CStockGraph::GetKType( )
{
	return m_nCurKType;
}

// 根据K线类型，读取数据
void CStockGraph::SetKType( int ktype )
{
	m_nCurKType	=	ktype;
    PrepareStockData(); // 重新取K线数据
}

int CStockGraph::GetKFormat( )
{
	return m_nCurKFormat;
}

void CStockGraph::SetKFormat( int kformat )
{
	m_nCurKFormat	=	kformat;
	AfxGetProfile().SetGraphKFormat( kformat );
	PrepareStockData( );
}

int CStockGraph::GetMaindataType( )
{
	return m_nCurMaindataType;
}

void CStockGraph::SetMaindataType( int maindatatype )
{
	m_nCurMaindataType	=	maindatatype;
	AfxGetProfile().SetGraphMaindataType( maindatatype );
	PrepareStockData( );
}

int CStockGraph::GetKLineMode( )
{
	return m_nCurKLineMode;
}

void CStockGraph::SetKLineMode( int klineMode )
{
	m_nCurKLineMode	=	klineMode;
}

CRect & CStockGraph::GetKDataRect( )
{
	return m_rectKData;
}

void CStockGraph::ResetIndexCurrent( int nIndexCurrent )
{
	m_nIndexCurrent	=	nIndexCurrent;
	m_anIndexCurrent.RemoveAll();
}

void CStockGraph::ResetIndex( int nIndexStart, int nIndexEnd, int nIndexCurrent )
{
	m_nIndexStart	=	nIndexStart;
	m_nIndexEnd		=	nIndexEnd;
	m_nIndexCurrent	=	nIndexCurrent;
	m_anIndexCurrent.RemoveAll();
}

void CStockGraph::ResetClient( CRect rectAll )
{
	if( rectAll.IsRectEmpty() )
		return;

	m_rectAll	=	rectAll;
	if( m_rectAll.IsRectEmpty() )	
		return;

	//	The Left Side m_rectKData,显示K线数据区
	m_rectKData.top		=	m_rectAll.top + m_nMarginTop;//+m_nMarginTop
	m_rectKData.left	=	m_rectAll.left+1;
	m_rectKData.bottom	=	m_rectKData.top + 280 ; // 313;
	m_rectKData.right	=	m_rectKData.left + KLINE_OFFSET;
 
	// The Right Side m_rectReport 显示各种指标数据区
	m_rectReport.top	=	m_rectAll.top + m_nMarginTop+7;
	m_rectReport.left	=	m_rectAll.right - REPORT_WIDTH;
	m_rectReport.bottom	=	m_rectAll.bottom - m_nMarginBottom;
	m_rectReport.right	=	m_rectAll.right;

	// The Center m_rectKLine   K线显示区
	CSPDWordArray	& anTech	=	AfxGetProfile().GetGraphTechsShow();
	int	nTechCount	=	anTech.GetSize();
	//分割窗口的数 =  主图数 + 技术指标个数,主图数的高度是技术指标高度的1.6
	//int	nUnit		=	(int)( (m_rectAll.Height()-m_nMarginTop-m_nMarginBottom) / (nTechCount+1.8) );
	int	nUnit		=	(int)( (m_rectAll.Height()-m_nMarginTop-m_nMarginBottom) / (nTechCount+1.6) );

	m_rectKLine.top		=	m_rectAll.top + m_nMarginTop;
	m_rectKLine.left	=	m_rectKData.right+1;
	m_rectKLine.bottom	=	m_rectKLine.top + m_rectAll.Height() - m_nMarginTop - m_nMarginBottom - nUnit*nTechCount;
	m_rectKLine.right	=	m_rectAll.right - m_rectReport.Width() - 1 ; //显示K线的宽度

	// The m_rectKLineCenter
	m_rectKLineCenter	=	m_rectKLine;
	m_rectKLineCenter.InflateRect( -2, -m_nHeightSubtitle, -2, -2 );

	// The m_arcTech
	m_arcTech.RemoveAll();
	int	nLastBottom = m_rectKLine.bottom;
	for( int k=0; k<anTech.GetSize(); k++ )
	{
		CRect	rectTech;
		rectTech.top		=	nLastBottom;
		rectTech.left		=	m_rectKLine.left;
		rectTech.bottom		=	rectTech.top + nUnit;
		rectTech.right		=	m_rectKLine.right;
		nLastBottom			=	rectTech.bottom;
		m_arcTech.Add( rectTech );
	}

	// The m_rectCenter
	m_rectCenter	=	m_rectKLine;
	m_rectCenter.bottom	=	m_rectAll.bottom;
}

void CStockGraph::ClearTechniqueCache( )
{
	m_techparam.Clear();
	for( int i=0; i<m_aptrTechUser.GetSize(); i++ )
	{
		CTechUser * pTechUser = (CTechUser *)m_aptrTechUser.GetAt(i);
		if( pTechUser )
			delete	pTechUser;
	}
	m_aptrTechUser.RemoveAll();
}

void CStockGraph::ClearCurStock( )
{
	ClearTechniqueCache( );
	m_CurStock.Clear();
	m_CurStockDown.Clear();
}

BOOL CStockGraph::SetCurStock( const TCHAR * szCode )
{
	if( NULL == szCode || lstrlen(szCode) <= 0 )
	{
		ASSERT( FALSE );
		return FALSE;
	}

	CStockInfo	info;
	if( ! /*AfxGetStockContainer().*/AfxGetProfile().m_pStockContainer->GetStockInfo( szCode, &info ) ) //?
	{
		ASSERT( FALSE );
		return FALSE;
	}

	return SetCurStock( info );
}

BOOL CStockGraph::SetCurStock( CStockInfo & info )
{
	if( m_CurStock.GetStockInfo().IsEqualTo( info.GetMarket(), info.GetStockCode() ) )
	{
		//return TRUE;
	}

	ResetIndex( );
	ClearCurStock( );

	m_nDotCount = info.DigitBit( );

	m_CurStock.SetStockInfo( &info );
	m_CurStock.SetDatabase(  AfxGetProfile().m_pDB /*&AfxGetDB()*/ );
	m_CurStockDown.SetStockInfo( &info );
	m_CurStockDown.SetDatabase( AfxGetProfile().m_pDB /* &AfxGetDB()*/ );
	m_dwLatestDate		=	-1;
	
	return TRUE;
}

void CStockGraph::SetReportWhat( int nReportWhat )
{
	m_nReportWhat	=	nReportWhat;
}

BOOL CStockGraph::PrepareStockData( BOOL bReload )
{
	if( !m_CurStock.GetStockInfo().IsValidStock() )
		return FALSE;
#if 1  // 重新取基本资料数据
	CStockInfo	info;

	if(  /*AfxGetStockContainer().*/AfxGetProfile().m_pStockContainer->GetStockInfo( m_CurStock.GetMarketStockCode(), &info )) //?
	{
	  if( m_CurStock.GetStockInfo().IsEqualTo( info.GetMarket(), info.GetStockCode() ) )
	  {
		m_CurStock.SetStockInfo( &info );
		m_CurStockDown.SetStockInfo( &info );
	  }
	}
#endif
   	int nOldSize = m_CurStock.GetKData(m_nCurKType).GetSize();
    m_bInvalidate = FALSE;
	// Prepare Data
	AfxPrepareStockData(  AfxGetProfile().m_pDB /*&AfxGetDB()*/, m_CurStock, m_nCurKType, m_nCurKFormat, m_nCurMaindataType, FALSE, bReload );
	AfxPrepareStockData(  AfxGetProfile().m_pDB /*&AfxGetDB()*/, m_CurStockDown, m_nCurKType, CKData::formatXDRdown, m_nCurMaindataType, FALSE, bReload );

	if( m_CurStock.GetKData(m_nCurKType).GetSize() != nOldSize )
	{
		ResetIndex( );
	}

	//提取K线数据
#if 1
	// 所有K线数据，均从行情服务器取得
	
     int nRet = AfxGetStkReceiver()->RequestKData( &m_CurStock, m_nCurKType );
	 if ( nRet == 0)
	 {
         m_bInvalidate = TRUE;
	 }
#else
	if ( CKData::ktypeWeek == m_nCurKType || CKData::ktypeMonth == m_nCurKType || CKData::ktypeDay == m_nCurKType  )
	{
	   AfxGetStkReceiver()->RequestKData( &m_CurStock, m_nCurKType );
	}
	else if (CKData::ktypeDay !=m_nCurKType )  //如果没有提取分时数据，将无法显示5min,15min,30min,60min数据
	{
        //AfxGetStkReceiver()->RequestKData( &m_CurStock, CKData::ktypeDay  );
		AfxGetStkReceiver()->RequestStockData( CStock::dataMinute, &info, 1, 0, 0 ); // 分时数据
	}
	else
	{
        AfxGetStkReceiver()->RequestKData( &m_CurStock, m_nCurKType );
	}
#endif
	m_techparam.Clear( );
	
	m_techparam.SetKData( &(m_CurStockDown.GetKData(m_nCurKType)) ); //当前股票向下复权后的数据

	m_techparam.volume.SetKData( &(m_CurStock.GetKData(m_nCurKType)) );
	
	m_techparam.ma.SetKData( &(m_CurStock.GetKData(m_nCurKType)) );
	
	m_techparam.bbi.SetKData( &(m_CurStock.GetKData(m_nCurKType)) );
	
	m_techparam.boll.SetKData( &(m_CurStock.GetKData(m_nCurKType)) );
	
	m_techparam.pv.SetKData( &(m_CurStock.GetKData(m_nCurKType)) );
	
	m_techparam.sar.SetKData( &(m_CurStock.GetKData(m_nCurKType)) );
	
	m_techparam.dj.SetKData( &(m_CurStock.GetKData(m_nCurKType)) );
	
	m_techparam.mike.SetKData( &(m_CurStock.GetKData(m_nCurKType)) );

	return TRUE;
}

void CStockGraph::Redraw( CDC * pDC, CRect rectAll )
{
	ASSERT( m_pParent && ::IsWindow( m_pParent->GetSafeHwnd() ) );
	if( m_rectAll != rectAll )
		ResetClient( rectAll );
	
	DrawStock( pDC );
}

void CStockGraph::DrawReportRegion( CDC * pDC )
{
	ASSERT( m_pParent && ::IsWindow( m_pParent->GetSafeHwnd() ) );
	if( !m_pParent || !::IsWindow(m_pParent->GetSafeHwnd()) )
		return;


	CClientDC	dc(m_pParent);
	if( NULL == pDC )
		pDC		=	&dc;

	switch( m_nReportWhat )
	{
	 case CStockGraph::reportParam:		DrawReportParam( pDC );		break;
	 case CStockGraph::reportCost:		DrawReportCost( pDC );		break;
	 case CStockGraph::reportFlame:		DrawReportFlame( pDC );		break;
	 case CStockGraph::reportActivity:	DrawReportActivity( pDC );	break;
	}
}

// 按照放大和缩小来计算
// 配合GetOneKLineRect函数
int CStockGraph::GetNextThickness( int nCurrentThickness, BOOL bAsc )
{
	if( bAsc )
	{
		nCurrentThickness+=2;
		//if (nCurrentThickness > 1 )  nCurrentThickness = (nCurrentThickness/2 ) * 2;
		if ( nCurrentThickness > UNIT_WIDTH ) nCurrentThickness = UNIT_WIDTH; 
		return nCurrentThickness;
	}
	else
	{
		nCurrentThickness-=2;
		//if (nCurrentThickness > 1 )  nCurrentThickness = (nCurrentThickness/2 ) * 2;
		if ( nCurrentThickness < 1 ) nCurrentThickness=1;
		return nCurrentThickness;
	}
}

//修改放大和缩小倍数
void CStockGraph::ExpandUp( BOOL bShiftPressed )
{
	if( m_nIndexCurrent >= 0 )
	{
		CKData	&kdata	=	m_CurStock.GetKData( m_nCurKType );
		if( m_nIndexCurrent < kdata.GetSize() )
		{	
			if( -1 != m_dwLatestDate && kdata.ElementAt(m_nIndexCurrent).m_date > m_dwLatestDate )
			{
				KDATA	&	kd	=	kdata.ElementAt(m_nIndexCurrent);
				if( bShiftPressed )
				{
#ifdef _E_DRAW_
					float	fIncrease	=	100000;
					fIncrease	=	( kd.m_fVolume/10 > fIncrease ? kd.m_fVolume/10 : fIncrease );
#else  //按手
					float	fIncrease	=	100;
					fIncrease	=	( kd.m_fVolume/100 > fIncrease ? kd.m_fVolume/100 : fIncrease );
#endif
					kd.m_fVolume	+=	fIncrease;
					kd.m_fAmount	=	( kd.m_fVolume*(kd.m_fOpen+kd.m_fClose)/2 );
				}
				else
				{
#ifdef _E_DRAW_
					float	fIncrease	=	(float)0.001;
					fIncrease	=	(float)( kd.m_fClose/500 > fIncrease ? kd.m_fClose/500 : fIncrease );
#else // 按厘
					float	fIncrease	=	(float)0.001;
					fIncrease	=	(float)( kd.m_fClose/1000 > fIncrease ? kd.m_fClose/1000 : fIncrease );
#endif
					kd.m_fClose		+=	fIncrease;
					kd.m_fOpen		+=	fIncrease;
					kd.m_fHigh		+=	fIncrease;
					kd.m_fLow		+=	fIncrease;
				}
				DrawStock( NULL );
				return;
			}
		}
	}

	int nOld = m_nThickness;
	m_nThickness = GetNextThickness( m_nThickness, TRUE );
	if( nOld != m_nThickness )
		DrawStock( NULL );
}

void CStockGraph::ExpandDown( BOOL bShiftPressed )
{
	if( m_nIndexCurrent >= 0 )
	{
		CKData	&kdata	=	m_CurStock.GetKData( m_nCurKType );
		if( m_nIndexCurrent < kdata.GetSize() )
		{	
			if( -1 != m_dwLatestDate && kdata.ElementAt(m_nIndexCurrent).m_date > m_dwLatestDate )
			{
				KDATA	&	kd	=	kdata.ElementAt(m_nIndexCurrent);
				if( bShiftPressed )
				{
#ifdef _E_DRAW_					
					float	fDecline	=	100000;
					fDecline		=	( kd.m_fVolume/10 > fDecline ? kd.m_fVolume/10 : fDecline );
#else // 按手
					float	fDecline	=	100;
					fDecline		=	( kd.m_fVolume/100 > fDecline ? kd.m_fVolume/100 : fDecline );
#endif
					if( kd.m_fVolume > fDecline )
						kd.m_fVolume	-=	fDecline;
					kd.m_fAmount	=	(float)( kd.m_fVolume*(kd.m_fOpen+kd.m_fClose)/2 );
				}
				else
				{
#ifdef _E_DRAW_
					float	fDecline	=	(float)0.001;
					fDecline	=	(float)( kd.m_fClose/500 > fDecline ? kd.m_fClose/500 : fDecline );
#else // 按厘
					float	fDecline	=	(float)0.001;
					fDecline	=	(float)( kd.m_fClose/1000 > fDecline ? kd.m_fClose/1000 : fDecline );
#endif
					if( kd.m_fLow > fDecline )
					{
						kd.m_fClose		-=	fDecline;
						kd.m_fOpen		-=	fDecline;
						kd.m_fHigh		-=	fDecline;
						kd.m_fLow		-=	fDecline;
					}
				}
				DrawStock( NULL );
				return;
			}
		}
	}

	int nOld = m_nThickness;
	m_nThickness = GetNextThickness( m_nThickness, FALSE );
	if( nOld != m_nThickness )
		DrawStock( NULL );
}

void CStockGraph::MoveLeft( BOOL bShiftPressed )
{
	Move( -1, bShiftPressed, FALSE );
}

void CStockGraph::MoveRight( BOOL bShiftPressed )
{
	if( m_nIndexCurrent >= 0 && bShiftPressed )
	{
		CKData	&kdata	=	m_CurStock.GetKData( m_nCurKType );
		if( m_nIndexCurrent+1 >= kdata.GetSize() )
		{	
			//	Add New KDATA to test predicate
			if( 0 == kdata.GetSize() )
				m_dwLatestDate	=	0;
			else if( -1 == m_dwLatestDate )
				m_dwLatestDate	=	kdata.ElementAt(kdata.GetSize()-1).m_date;
			KDATA	kd;
			memset( &kd, 0, sizeof(kd) );
			if( kdata.GetSize() > 0 )
				kd	=	kdata.ElementAt(kdata.GetSize()-1);
			else
				kd.m_fClose	=	1;
			kd.m_fOpen	=	kd.m_fHigh	=	kd.m_fLow	=	kd.m_fClose;
			kd.m_date	=	CSPTime::GetStockTimeNext( kd.m_date, m_nCurKType );
			kdata.Add( kd );
			m_CurStockDown.GetKData( m_nCurKType ).Add( kd );
			
			bShiftPressed	=	FALSE;
		}
	}

	Move( 1, bShiftPressed, FALSE );
}

void CStockGraph::PageLeft( BOOL bShiftPressed )
{
	int	nMove	=	-(m_nIndexEnd-m_nIndexStart-5);
	if( nMove >= 0 )
		nMove	=	-1;
	Move( nMove, bShiftPressed, FALSE );
}

void CStockGraph::PageRight( BOOL bShiftPressed )
{
	int	nMove	=	(m_nIndexEnd-m_nIndexStart-5);
	if( nMove <= 0 )
		nMove	=	1;
	Move( nMove, bShiftPressed, FALSE );
}

void CStockGraph::MoveHome( BOOL bShiftPressed )
{
	if( m_nIndexCurrent >= 0 )
	{
		// Move Current Selection
		int	nPos	=	m_nIndexStart;
		MoveTo( nPos, bShiftPressed, FALSE ,TRUE ,m_nCrossCursorPos);
	}
	else
	{
		// Show Prev Technique Line
		UINT	nTechUserCount	=	CTechUser::GetTechUserCount();
		UINT	nTechMax	=	STT_MAX;
		if( nTechUserCount > 0 )
			nTechMax	=	STT_USER_MIN + nTechUserCount - 1;

		CSPDWordArray	& auShow	=	AfxGetProfile().GetGraphTechsShow();
		int	nOldSize = auShow.GetSize();
		if( auShow.GetSize() > 0 )
		{
			UINT	nNext	=	auShow.ElementAt(auShow.GetSize()-1);
			nNext	--;
			if( nNext < STT_TREND_MIN || nNext > nTechMax )
				nNext	=	nTechMax;
			if( nNext > STT_MAX && nNext < STT_USER_MIN )
				nNext	=	STT_MAX;
			AfxGetProfile().ShowGraphViewTech( nNext );
		}
		else
		{
			auShow.Add( nTechMax );
		}
		if( auShow.GetSize() == nOldSize )
			InvalidateCenter( );
		else if( m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()) )
		{
			ResetClient( m_rectAll );
			m_pParent->Invalidate(FALSE);
		}
	}
}

void CStockGraph::MoveEnd( BOOL bShiftPressed )
{
	if( m_nIndexCurrent >= 0 )
	{
		// Move Current Selection
		CKData	& kdata	=	m_CurStock.GetKData( m_nCurKType );
		int	nPos	=	( m_nIndexEnd < 0 ? kdata.GetSize()-1 : m_nIndexEnd );
		MoveTo( nPos, bShiftPressed, FALSE  ,TRUE,m_nCrossCursorPos);
	}
	else
	{
		// Show Next Technique Line
		UINT	nTechUserCount	=	CTechUser::GetTechUserCount();
		UINT	nTechMax	=	STT_MAX;
		if( nTechUserCount > 0 )
			nTechMax	=	STT_USER_MIN + nTechUserCount - 1;

		CSPDWordArray	& auShow	=	AfxGetProfile().GetGraphTechsShow();
		int nOldSize = auShow.GetSize();
		if( auShow.GetSize() > 0 )
		{
			UINT	nNext	=	auShow.ElementAt(auShow.GetSize()-1);
			nNext	++;
			if( nNext < STT_TREND_MIN || nNext > nTechMax )
				nNext	=	STT_TREND_MIN;
			if( nNext > STT_MAX && nNext < STT_USER_MIN )
				nNext	=	STT_USER_MIN;
			AfxGetProfile().ShowGraphViewTech( nNext );
		}
		else
		{
			auShow.Add( STT_TREND_MIN );
		}
		if( auShow.GetSize() == nOldSize )
			InvalidateCenter( );
		else if( m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()) )
		{
			ResetClient( m_rectAll );
			m_pParent->Invalidate(FALSE);
		}
	}
}

void CStockGraph::OnInsert( )
{
	CKData	&kdata	=	m_CurStock.GetKData( m_nCurKType );

	KDATA	kd;
	memset( &kd, 0, sizeof(kd) );
	if( kdata.GetSize() > 0 )
		kd	=	kdata.ElementAt(kdata.GetSize()-1);
	else
		kd.m_fClose	=	1;
	kd.m_fOpen	=	kd.m_fHigh	=	kd.m_fLow	=	kd.m_fClose;
	kd.m_date	=	CSPTime::GetStockTimeNext( kd.m_date, m_nCurKType );

	CEditKDATADlg	dlg( m_pParent );
	dlg.SetKDATA( kd, m_nCurKType );
	if( IDOK == dlg.DoModal( ) )
	{
		kd	=	dlg.GetKDATA( );
		kdata.Add( kd );
		m_CurStockDown.GetKData( m_nCurKType ).Add( kd );
		m_nIndexCurrent	=	kdata.GetSize()-1;
		if( -1 == m_dwLatestDate )
		{
			if( kdata.GetSize() <= 1 )
				m_dwLatestDate	=	0;
			else
				m_dwLatestDate	=	kdata.ElementAt(kdata.GetSize()-2).m_date;
		}
		MoveTo( m_nIndexCurrent, FALSE, FALSE ,TRUE,m_nCrossCursorPos );
	}

	if( m_pParent && ::IsWindow( m_pParent->GetSafeHwnd() ) )
		m_pParent->SetFocus( );
}

void CStockGraph::OnLButtonDown( UINT nFlags, CPoint point, BOOL bShiftPressed, BOOL bCtrlPressed )
{
	// Border Move
	if( m_bCursorBorder )
	{
		SetCapture(m_pParent->GetSafeHwnd());
		
		//DrawBorderMovingLine( NULL, m_ptSelectBegin,  AfxGetProfile().GetColor( CColorClass::clrLine1 ) , FALSE );
      //  DrawSelectionLine( NULL,m_nCrossCursorPos,FALSE);
		//m_nCrossCursorPos=	CPoint(0,0);

		// Draw Border Moving Line
		DrawBorderMovingLine( NULL, point, AfxGetProfile().GetColor( CColorClass::clrBorder ) , TRUE );
		m_ptLastMovingLine	=	point;
		return;
	}

	// KLine Index
	int	nIndexClick	=	GetIndexClick( point );
	MoveTo( nIndexClick, bShiftPressed, bCtrlPressed  , FALSE, point);

	// Selection Rect
	if ( -1 != nIndexClick && m_rectKLineCenter.PtInRect(point) )
	{
		SetCapture(m_pParent->GetSafeHwnd());
		m_ptSelectBegin	=	point;
		m_ptSelectEnd	=	point;
		DrawSelectionRect( NULL, m_ptSelectBegin, m_ptSelectEnd, TRUE );
	}

	// Selected TechID
	SelectTechID( point );
}

void CStockGraph::OnLButtonUp( UINT nFlags, CPoint point, BOOL bShiftPressed, BOOL bCtrlPressed )
{
	if( m_bCursorBorder && GetCapture() == m_pParent->GetSafeHwnd() )
	{
		ReleaseCapture( );
		
		//DrawBorderMovingLine( NULL, m_ptSelectBegin,  AfxGetProfile().GetColor( CColorClass::clrLine1 ) , FALSE );

		// Remove Border Moving Line
		DrawBorderMovingLine( NULL, point, AfxGetProfile().GetColor( CColorClass::clrBorder ) , FALSE );
		// Reset Border
		if( 0 == m_nPosSelectBorder )
		{
			m_rectKLine.bottom	=	point.y;
			m_rectKLineCenter	=	m_rectKLine;
			m_rectKLineCenter.InflateRect( -2, -m_nHeightSubtitle, -2, -2 );
			if( m_arcTech.GetSize() > 0 )
				m_arcTech.ElementAt(0).top	=	point.y;
		}
		else if( m_nPosSelectBorder > 0 )
		{
			if( m_nPosSelectBorder < m_arcTech.GetSize() )
			{
				m_arcTech.ElementAt(m_nPosSelectBorder-1).bottom	=	point.y;
				m_arcTech.ElementAt(m_nPosSelectBorder).top			=	point.y;
			}
		}

		m_bCursorBorder = FALSE;
		DrawStock( NULL );
		return;
	}

	if( GetCapture() == m_pParent->GetSafeHwnd() )
	{
		ReleaseCapture( );
		DrawSelectionRect( NULL, m_ptSelectBegin, m_ptSelectEnd, FALSE );

		int nClickBegin = GetIndexClick( m_ptSelectBegin );
		int nClickEnd = GetIndexClick( m_ptSelectEnd );
		if( -1 == nClickBegin )
		{
			if( m_ptSelectBegin.x < m_rectCenter.left )		nClickBegin	=	m_nIndexStart;
			if( m_ptSelectBegin.x > m_rectCenter.right )	nClickBegin	=	m_nIndexEnd;
		}
		if( -1 == nClickEnd )
		{
			if( m_ptSelectEnd.x < m_rectCenter.left )		nClickEnd	=	m_nIndexStart;
			if( m_ptSelectEnd.x > m_rectCenter.right )		nClickEnd	=	m_nIndexEnd;
		}

		if( -1 != nClickBegin && -1 != nClickEnd )
		{
			if( nClickBegin < nClickEnd ) // 放大
			{
				int nLeft = min(nClickBegin,nClickEnd);
				int nRight = max(nClickBegin,nClickEnd);
#ifndef _E_DRAW_
				if( 5*(nRight-nLeft+1) < m_rectCenter.Width() )
				{
					nLeft -= (m_rectCenter.Width() - 5*(nRight-nLeft+1))/10 + 1;
					nRight += (m_rectCenter.Width() - 5*(nRight-nLeft+1))/10 + 1;
				}
#else
				if( 15*(nRight-nLeft+1) < m_rectCenter.Width() )
				{
					nLeft -= (m_rectCenter.Width() - 15*(nRight-nLeft+1))/30 + 1;
					nRight += (m_rectCenter.Width() - 15*(nRight-nLeft+1))/30 + 1;
				}

#endif
				int	nOldLen = m_nIndexEnd - m_nIndexStart;
				int	nOldThick = m_nThickness;
				ResetIndex( nLeft, nRight, m_nIndexCurrent );
				int nNewLen = m_nIndexEnd - m_nIndexStart;

				while( nOldLen * nOldThick > nNewLen * m_nThickness )
				{
					int	nTemp = m_nThickness;
					m_nThickness = GetNextThickness( m_nThickness, TRUE );
					if( nTemp == m_nThickness )
						break;
				}
			}
			else if( nClickBegin > nClickEnd ) // 缩小
			{
				m_nThickness = GetNextThickness( m_nThickness, FALSE );
			//	m_nThickness = GetNextThickness( m_nThickness, FALSE );
			//	m_nThickness = GetNextThickness( m_nThickness, FALSE );
			}

			DrawStock( NULL );
		}
	}
}

void CStockGraph::OnMouseMove( UINT nFlags, CPoint point )
{
#if 0
	CClientDC	dc(m_pParent);
    CString	strDate;
	strDate.Format(_T( "x=%d  y%d"),point.x,point.y );
	dc.TextOut(100, 100, strDate );
#endif      
	//CRect rect;
	//if ( m_pParent== NULL ) return ;
	if ( (( MK_MBUTTON & nFlags)==MK_MBUTTON )|| ((MK_RBUTTON & nFlags)==MK_RBUTTON))
	{
		return;
	}

  //  GetClientRect(m_pParent->GetSafeHwnd(),rect);
 // If outside client area, return 
   // if ( !m_rectCenter.PtInRect(point))
	//{	
	  //     return;
	//}

	// Size Border
	if ( m_bCursorBorder && GetCapture() == m_pParent->GetSafeHwnd() )
	{
		DrawBorderMovingLine( NULL, m_ptLastMovingLine, AfxGetProfile().GetColor( CColorClass::clrBorder ) , TRUE );
		DrawBorderMovingLine( NULL, point, AfxGetProfile().GetColor( CColorClass::clrBorder ) , TRUE );
		m_ptLastMovingLine	=	point;
		return;
	}

	int nPosSelectBorder = -1;
	if ( m_rectCenter.PtInRect(point) )
	{
		CRect	rect = m_rectKLine;
		for( int i=0; i<m_arcTech.GetSize(); i++ )
		{
			rect.top = rect.bottom-5;
			rect.bottom = rect.bottom+5;
			if( rect.PtInRect(point) )
				nPosSelectBorder = i;
			rect = m_arcTech[i];
		}
	}
	if ( -1 != nPosSelectBorder )
	{
		::SetCursor( AfxGetApp()->LoadCursor(AFX_IDC_VSPLITBAR) );
		m_bCursorBorder = TRUE;
		m_nPosSelectBorder = nPosSelectBorder;
	}
	if ( -1 == nPosSelectBorder && m_bCursorBorder && GetCapture() != m_pParent->GetSafeHwnd() )
	{
		::SetCursor( AfxGetApp()->LoadCursor(IDC_ARROW) );
		m_bCursorBorder = FALSE;
	}

	if ( m_bCursorBorder )
	{
		return;
	}

	// Selection Rect
	if ( GetCapture() == m_pParent->GetSafeHwnd() && (( MK_LBUTTON & nFlags)==MK_LBUTTON  ))
	{
		DrawSelectionRect( NULL, m_ptSelectBegin, m_ptSelectEnd, FALSE );

		m_ptSelectEnd	=	point;
		if( m_ptSelectEnd.x <= m_rectKLineCenter.left )
			m_ptSelectEnd.x = m_rectKLineCenter.left+1;
		if( m_ptSelectEnd.x >= m_rectKLineCenter.right )
			m_ptSelectEnd.x = m_rectKLineCenter.right-1;
		if( m_ptSelectEnd.y <= m_rectKLineCenter.top )
			m_ptSelectEnd.y = m_rectKLineCenter.top+1;
		if( m_ptSelectEnd.y >= m_rectKLineCenter.bottom )
			m_ptSelectEnd.y = m_rectKLineCenter.bottom-1;

		DrawSelectionRect( NULL, m_ptSelectBegin, m_ptSelectEnd, TRUE );
	}
	else  // add 2010.01.01
	{
	 //  if ( m_nCrossCursorPos == point ) return;
	  // KLine Index
	  int	nIndexClick	= -1;
	  CRect	rect(m_rectKLine.left,m_rectKLine.top,m_rectKLine.right,m_rectAll.bottom-m_nMarginBottom/*m_rectKLine.bottom*/);
	  if( rect.PtInRect(point) )
	  {
         nIndexClick	=	GetIndexClick( point );
	  }
	  BOOL bShiftPressed = ((MK_SHIFT& nFlags)==MK_SHIFT);
	  BOOL bCtrlPressed = (( MK_CONTROL & nFlags)== MK_CONTROL );
	  //BOOL bSelected = 
	  MoveTo( nIndexClick, bShiftPressed ,bCtrlPressed, FALSE,point);
#if 0
	  // Selection Rect
	  if (bSelected && -1 != nIndexClick && m_rectKLineCenter.PtInRect(point) )
	  {
		strDate.Format(_T( "bSelected x=%d  y%d"),point.x,point.y );
	    dc.TextOut(100, 150, strDate );
		//if( m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()) )
		//{
		 // m_pParent->InvalidateRect(NULL,TRUE);
		//}
	  }
	  else
	  {
		strDate.Format(_T( "UNSelected x=%d  y%d"),point.x,point.y );
	    dc.TextOut(100, 120, strDate );
	  }
#endif
	  // Selected TechID
	  SelectTechID( point );
	 
	}
}

void CStockGraph::OnLButtonDblClk( UINT nFlags, CPoint point, BOOL bShiftPressed, BOOL bCtrlPressed )
{
	OnLButtonDown( nFlags, point, bShiftPressed, bCtrlPressed );

	AdjustSelectedTech();

	OnReturn( bShiftPressed, bCtrlPressed );
    
	//AfxGetStaticDoc()->UpdateAllViews( NULL, UPDATE_HINT_GRAPHVIEW, NULL );
}

void CStockGraph::OnRButtonDown( UINT nFlags, CPoint point, BOOL bShiftPressed, BOOL bCtrlPressed )
{
	SelectTechID( point );
}

//修改K线数据
// 加了m_dwLatestDate 意义？
void CStockGraph::OnReturn( BOOL bShiftPressed, BOOL bCtrlPressed )
{
	if( m_nIndexCurrent >= 0 )
	{
		CKData	&kdata	=	m_CurStock.GetKData( m_nCurKType );

		if( m_nIndexCurrent < kdata.GetSize() )
		{	
			//  m_dwLatestDate 永远等于 -1 ？
			//if( -1 != m_dwLatestDate && kdata.ElementAt(m_nIndexCurrent).m_date > m_dwLatestDate )
			{
				KDATA	&	kd	=	kdata.ElementAt(m_nIndexCurrent);

				CEditKDATADlg	dlg( m_pParent );
				dlg.SetKDATA( kd, m_nCurKType );
				if( IDOK == dlg.DoModal( ) )
				{
					kd	=	dlg.GetKDATA( );
					if( m_nIndexCurrent < m_CurStockDown.GetKData( m_nCurKType ).GetSize() )
						m_CurStockDown.GetKData( m_nCurKType ).SetAt(m_nIndexCurrent,kd);
					DrawStock( NULL );
                    //保存修改的数据
					CKData	& kdata	=	m_CurStock.GetKData(m_nCurKType);
					AfxGetProfile().m_pDB->InstallKData( kdata ,TRUE); // 保存K线数据

				}
				if( m_pParent && ::IsWindow( m_pParent->GetSafeHwnd() ) )
					m_pParent->SetFocus( );
			}
		}
	}
}

UINT CStockGraph::GetSelectedTech( )
{
	return m_nSelectedTechID;
}

void CStockGraph::HideSelectedTech()
{
	AfxGetProfile().ToggleGraphViewTech( m_nSelectedTechID );

	if( m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()) )
	{
		ResetClient( m_rectAll );
		m_pParent->Invalidate(FALSE);
	}
}

void CStockGraph::AdjustSelectedTech()
{
	if( STT_INVALID != m_nSelectedTechID )
	{
		CSetParamDlg	dlg;
		dlg.SetFirstSelected( m_nSelectedTechID );
		dlg.DoModal();

		CGraphView * pGraphView = AfxGetGraphView();
		if( pGraphView )
			pGraphView->OnTechParametersChange( );
		AfxGetStaticDoc()->UpdateAllViews( NULL, UPDATE_HINT_GRAPHVIEW, NULL );
	}
}

BOOL CStockGraph::MoveTo( int nIndex, BOOL bShiftPressed, BOOL bCtrlPressed,BOOL bKeyState , CPoint point )
{
	try
	{
	
	 BOOL bSelected = DrawSelectionLine( NULL, point,FALSE );

	 // To Change m_anIndexCurrent
	 if( bShiftPressed && nIndex >= 0 )
	 {
		if( m_nIndexCurrent < 0 )
		{
			AddIndexCurrent( nIndex );
		}
		else if( m_nIndexCurrent <= nIndex )
		{
			for( int nSel=m_nIndexCurrent; nSel<=nIndex; nSel++ )
			{
				AddIndexCurrent( nSel );
			}
		}
		else
		{
			for( int nSel=nIndex; nSel<=m_nIndexCurrent; nSel++ )
			{
				AddIndexCurrent( nSel );
			}
		}
	 }
	 else if( bCtrlPressed && nIndex >= 0 )
	 {
		AddIndexCurrent( nIndex );
	 }
	 else
	 {
		m_anIndexCurrent.RemoveAll();
	 }

	 BOOL	bNeedDrawAll	=	FALSE;
	 if( -1 != nIndex && nIndex < m_nIndexStart )
	 {
		m_nIndexEnd		-=	(m_nIndexStart-nIndex);
		m_nIndexStart	=	nIndex;
		m_nIndexCurrent	=	nIndex;
		bNeedDrawAll	=	TRUE;
	 }
	 if( -1 != nIndex && nIndex > m_nIndexEnd )
	 {
		m_nIndexStart	+=	(nIndex-m_nIndexEnd);
		m_nIndexEnd		=	nIndex;
		m_nIndexCurrent	=	nIndex;
		bNeedDrawAll	=	TRUE;
	 }

	 if( m_nIndexCurrent != nIndex )
	 {
		int	nOld	=	m_nIndexCurrent;
		m_nIndexCurrent	=	nIndex;
		OnIndexCurrentChanged( );
	 }

      if ( !bKeyState )   // 无键按下的时候
	  {
	     m_nCrossCursorPos = point;
	  }

	  if( bNeedDrawAll )
	  { 
		DrawStock( NULL );
	  }
	  else
	  {
	    bSelected = DrawSelectionLine( NULL, point, TRUE );
	  }
	  return ( bSelected );
	}
	catch(...)
	{
		return FALSE;
	}
}

void CStockGraph::OnIndexCurrentChanged( )
{
	if( m_pParent && ::IsWindow(m_pParent->GetSafeHwnd()) )
	{
		CClientDC	dc(m_pParent);

		CRect	rectKLineAttrib = m_rectKLineCenter;
		rectKLineAttrib.top -= m_nHeightSubtitle;
		CSPDWordArray	& anTech	=	AfxGetProfile().GetGraphTechsKLine();
		if( anTech.GetSize() > 0 )
			DrawKLineAttribute( &dc, anTech[0], rectKLineAttrib, TRUE, 0, 0 );
		
		CSPDWordArray	& anShow	=	AfxGetProfile().GetGraphTechsShow( );
		for( int k=0; k<m_arcTech.GetSize(); k++ )
		{
			CRect	rect	=	m_arcTech.ElementAt(k);
			if( k < anShow.GetSize() && anShow[k] == STT_MIKE )
			{
				DrawTechLine( &dc, anShow[k], rect, FALSE );
				DrawTechLine( &dc, anShow[k], rect, TRUE );
			}
			else if( k < anShow.GetSize() )
			{
				DrawTechLine( &dc, anShow[k], rect, TRUE );
			}
		}

		DrawReportRegion( &dc );
		DrawKData( &dc );              // 光标改变时
	}
}

void CStockGraph::Move( int nMove, BOOL bShiftPressed, BOOL bCtrlPressed )
{
	int	nIndexCurrent	=	m_nIndexCurrent;
	
	if( nIndexCurrent < 0 )
	{
		if( nMove < 0 )
			nIndexCurrent	=	m_nIndexEnd;
		else if( nMove > 0 )
			nIndexCurrent	=	m_nIndexStart;
	}
	else
	{
		nIndexCurrent	+=	nMove;
	}
   try
   {
	 CKData	&kdata	=	m_CurStock.GetKData( m_nCurKType );
	 if( nIndexCurrent < 0 )
		nIndexCurrent	=	0;
	 if( nIndexCurrent >= kdata.GetSize() )
		nIndexCurrent	=	kdata.GetSize()-1;

   }catch(...)
   {
		return ;
	}

	MoveTo( nIndexCurrent, bShiftPressed, bCtrlPressed ,FALSE ,m_nCrossCursorPos);
}

int CStockGraph::GetIndexClick( CPoint point )
{
	int	nIndexClick	=	-1;
	if( m_rectCenter.PtInRect( point ) )
	{
		for( int nIndex=m_nIndexStart; nIndex<=m_nIndexEnd; nIndex ++ )
		{
			CRect	rectK;
			if( GetOneKLineRect( nIndex, &rectK, NULL, NULL, NULL ) && rectK.PtInRect(point) )
			{
				nIndexClick	=	nIndex;
				break;
			}
		}
	}
	return nIndexClick;
}

void CStockGraph::SelectTechID( CPoint point )
{
	m_nSelectedTechID	=	STT_INVALID;
	int	nTechClick	=	-1;
	for( int k=0; k<m_arcTech.GetSize(); k++ )
	{
		if( m_arcTech.ElementAt(k).PtInRect( point ) )
		{
			nTechClick	=	k;
			break;
		}
	}

	CSPDWordArray	& anTech	=	AfxGetProfile().GetGraphTechsShow();
	if( nTechClick >= 0 && nTechClick < m_arcTech.GetSize() && nTechClick < anTech.GetSize() )
	{
		m_nSelectedTechID	=	anTech.ElementAt(nTechClick);
	}
}

BOOL CStockGraph::AddIndexCurrent( int nIndex )
{
	for( int k=0; k<m_anIndexCurrent.GetSize(); k++ )
	{
		if( (UINT)nIndex == m_anIndexCurrent[k] )
			return FALSE;
	}
	m_anIndexCurrent.Add( nIndex );
	return TRUE;
}

void CStockGraph::InvalidateCenter( )
{
	if( !m_pParent || !::IsWindow(m_pParent->GetSafeHwnd()) )
		return;
	CClientDC	dc(m_pParent);
	DrawDateAxis( &dc );
	DrawTechLine( &dc );
	DrawKLine( &dc );
}

// 计算K线画图区间
BOOL CStockGraph::GetOneKLineRect( int nIndex, LPRECT lpRect, long *pxEntityLeft, long *pxEntityRight, long *pxMedium )
{
	if( nIndex == -1 || nIndex < m_nIndexStart || nIndex > m_nIndexEnd )
		return FALSE;

	CRect	rectK	=	m_rectCenter;
	rectK.left	=	m_rectKLineCenter.left + (nIndex-m_nIndexStart) * m_nThickness;
	rectK.right	=	rectK.left + m_nThickness; //K线柱的宽度(美国线)

	if( rectK.Width() <= 0 || rectK.Height() <= 0 )
		return FALSE;
	if( lpRect )
		*lpRect	=	rectK;

	int	xStart	=	rectK.left;

	CRect	rcEntity;

	//计算K线柱体宽度
#if 1	
	rcEntity.left	=	xStart;
	rcEntity.right	=	xStart +m_nThickness;

	if( pxEntityLeft )
		*pxEntityLeft	=	rcEntity.left;
	if( pxEntityRight )
		*pxEntityRight	=	rcEntity.right;
	if( pxMedium )
		*pxMedium	=	rcEntity.left + rcEntity.Width() / 2;
#else
	switch( m_nThickness )
	{
	case 1:
	case 2:
		rcEntity.left	=	xStart;
		rcEntity.right	=	xStart + 1;
		break;
	case 4:
	case 5:
		rcEntity.left	=	xStart;
		rcEntity.right	=	xStart + 3;
		break;
	case 6:
	case 7:
		rcEntity.left	=	xStart;
		rcEntity.right	=	xStart + 5;
		break;
	case 9:
	case 10:
		rcEntity.left	=	xStart;
		rcEntity.right	=	xStart + 7;
		break;
	case 13:
	case 15:
		rcEntity.left	=	xStart;
		rcEntity.right	=	xStart + 11;
		break;
	default:
		ASSERT( FALSE );
		rcEntity.left	=	xStart;
		rcEntity.right	=	xStart + 3;
		return FALSE;
	}
	if( pxEntityLeft )
		*pxEntityLeft	=	rcEntity.left;
	if( pxEntityRight )
		*pxEntityRight	=	rcEntity.right;
	if( pxMedium )
		*pxMedium	=	rcEntity.left + rcEntity.Width() / 2;
#endif

	return TRUE;
}

void CStockGraph::DrawStock( CDC * pDC )
{
	ASSERT( m_pParent && ::IsWindow( m_pParent->GetSafeHwnd() ) );
	if( !m_pParent || !::IsWindow(m_pParent->GetSafeHwnd()) )
		return;

	CClientDC	dc(m_pParent);
	if( NULL == pDC )
		pDC		=	&dc;

	if( !m_CurStock.GetStockInfo().IsValidStock() )
	{
		DrawNoDataInfo( pDC ); // 2008.03.20
		return;
    }
	CKData	& kdata	=	m_CurStock.GetKData(m_nCurKType);
	if( kdata.GetSize() == 0 )
	{
		DrawNoDataInfo( pDC );
    //提取K线数据
	//	AfxGetStkReceiver()->RequestKData( &m_CurStock, m_nCurKType );
  	
	}
	else if ( PrepareDrawKLine( ) ) // Reset m_nIndexStart, m_nIndexEnd, m_nIndexCurrent
	{
		DrawDateAxis( pDC );
		DrawTechLine( pDC );
		DrawKLine( pDC );
		DrawKData( pDC );     //交易时段不刷新
	}
   DrawReportRegion( pDC );
}

//计算显示K线数据的长度
BOOL CStockGraph::PrepareDrawKLine(	)
{
	m_HighPrice.clear();
	m_LowPrice.clear();
 	m_HighIndex.clear();
	m_LowIndex.clear();

	ASSERT( m_nThickness > 0 );
	if( m_nThickness <= 0 || m_rectKLineCenter.Width() <= 0 )
		return FALSE;

	CKData	& kdata	=	m_CurStock.GetKData(m_nCurKType);

	int	nWidth	=	m_rectKLineCenter.Width()- m_nThickness * 1.5 ;
	int	nCount	=	nWidth / m_nThickness+1; //根据空间计算可以显示的数据长度
	if( nCount <= 0 )
		return FALSE;

	if( kdata.GetSize() == 0 )
	{
	//	AfxMessageBox(_T("kdata.GetSize() == 0 "));
		m_nIndexCurrent	=	m_nIndexStart	=	m_nIndexEnd	=	-1; 
		return FALSE;
	}

	// Check Old Values
	ASSERT( m_nIndexStart <= m_nIndexEnd );
	ASSERT( m_nIndexCurrent < 0 || (m_nIndexCurrent >= m_nIndexStart && m_nIndexCurrent <= m_nIndexEnd) );
	if( m_nIndexCurrent >= 0 && (m_nIndexCurrent < m_nIndexStart || m_nIndexCurrent > m_nIndexEnd) )
		m_nIndexCurrent	=	-1;

	if( -1 == m_nIndexStart || -1 == m_nIndexEnd )
	{
		m_nIndexEnd	=	kdata.GetSize()-1;
	}
	else if( m_nIndexEnd - m_nIndexStart + 1 < nCount )
	{
		// Reset m_nIndexStart and m_nIndexEnd
		int	nExpand	=	nCount-(m_nIndexEnd-m_nIndexStart+1);
		if( m_nIndexCurrent >= 0 && m_nIndexEnd-m_nIndexStart > 0 )
			m_nIndexEnd		+=	(m_nIndexEnd-m_nIndexCurrent)*nExpand / (m_nIndexEnd-m_nIndexStart);
	}
	else if( m_nIndexEnd - m_nIndexStart + 1 > nCount )
	{
		int	nReduce	=	(m_nIndexEnd-m_nIndexStart+1)-nCount;
		if( m_nIndexCurrent >= 0 && m_nIndexEnd-m_nIndexStart > 0 )
			m_nIndexEnd		-=	(m_nIndexEnd-m_nIndexCurrent)*nReduce / (m_nIndexEnd-m_nIndexStart);
	}
	if( m_nIndexEnd >= kdata.GetSize() )
		m_nIndexEnd	=	kdata.GetSize()-1;
	m_nIndexStart	=	m_nIndexEnd - nCount + 1; //K线的起点
	if( m_nIndexStart < 0 )
	{
		m_nIndexEnd		+=	(-m_nIndexStart);
		m_nIndexStart	=	0;
	}
	if( m_nIndexEnd >= kdata.GetSize() )
		m_nIndexEnd	=	kdata.GetSize()-1;

	ASSERT( m_nIndexStart <= m_nIndexEnd );
	ASSERT( m_nIndexCurrent < 0 || (m_nIndexCurrent >= m_nIndexStart && m_nIndexCurrent <= m_nIndexEnd) );
	if( m_nIndexStart > m_nIndexEnd )
		return FALSE;
	if( m_nIndexCurrent >= 0 && (m_nIndexCurrent < m_nIndexStart || m_nIndexCurrent > m_nIndexEnd) )
		m_nIndexCurrent	=	-1;

	for (int  k=m_nIndexStart; k<=m_nIndexEnd; k++ )
	{
 
		//if ( kdata.ElementAt(k).m_fHigh == 0 &&   // 停盘日处理
        //     kdata.ElementAt(k).m_fLow == 0 )
		//	 continue;

		m_HighPrice.push_back(kdata.ElementAt(k).m_fHigh);//所有数据的最高
		m_LowPrice.push_back(kdata.ElementAt(k).m_fLow);//所有数据的最低
		int nSize =  m_HighPrice.size();
 	    if ( nSize > 1 )
		{
		   m_HighPrice[nSize-1] = m_HighPrice[nSize-1] * 0.9995 + m_HighPrice[nSize-2] * 0.0005;
		}
  	    if ( nSize > 1  )
		{
			m_LowPrice[nSize-1] = m_LowPrice[nSize-1] * 0.9995 + m_LowPrice[nSize-2] * 0.0005;
		}
	}

    for (int  k=m_nIndexStart; k<=m_nIndexEnd; k++ )
	{
        if ( IsHighLabel(k-m_nIndexStart) )
	    {
          m_HighIndex.push_back(k) ;     //最高点数据的index 
	    }
        else if ( IsLowLabel(k-m_nIndexStart) )
		{
          m_LowIndex.push_back(k);         //最低点数据的Index
		}
	}


	return TRUE;
}


BOOL CStockGraph::IsHighLabel(int Index)     //某段区间里的最高
{
  int 	I, SS, EE;
  float FH;
  int  Interval;
  int  DataPerPage  = m_nIndexEnd - m_nIndexStart;	
  Interval = max(10,DataPerPage/20);
  if ( m_HighPrice.size() == 0 )  return FALSE;
  if ( Interval < 1 ) return FALSE;
  if (Index > -1 )
  {
     FH = 0;
     SS = max(0,Index - Interval);
     EE = min(m_HighPrice.size()-1,Index + Interval);
     for ( I = SS ; I <= EE; I++)
	 {
	    FH = max(FH,m_HighPrice[I]);
	 }
	 
	 if ( Index > 0  )
	 {
	    if ( m_HighPrice[Index] == FH  && m_HighPrice[Index-1] == FH ) return FALSE;
	 }

     return (m_HighPrice[Index] == FH);
  }
  return FALSE;
}

BOOL CStockGraph::IsLowLabel(int Index)    //某段区间里的最低
{
  int I, SS, EE;
  float FL;
  int Interval;
  int  DataPerPage  = m_nIndexEnd - m_nIndexStart;	
  Interval = max(10,DataPerPage/20);
  if ( m_LowPrice.size() == 0 )  return FALSE;
  if ( Interval < 1 ) return FALSE;
  if (Index > -1 )
  {
      FL = 999999 ; // MaxSingle;
      SS = max(0,Index - Interval);
      EE = min(m_LowPrice.size()-1,Index + Interval);
      for ( I = SS ; I <= EE; I++)
	  {
          FL = min(FL,m_LowPrice[I]);
	  }
	  
      if ( Index > 0  )
	  {
	    if ( m_LowPrice[Index] == FL  && m_LowPrice[Index-1] == FL ) return FALSE;
	  }
      return (m_LowPrice[Index] == FL);
  }
    return FALSE;
}

// 显示获取更多数据请点击菜单“数据->历史数据下载...
void CStockGraph::DrawNoDataInfo( CDC * pDC )
{
	DECLARE_COLOR_DEFINATION

	CKData	&	kdata	=	m_CurStock.GetKData(m_nCurKType);
	if( 0 == kdata.GetSize() )
	{
		CString	strNoKData, strTemp;
		strNoKData.LoadString( IDS_STOCKGRAPH_NOKDATA );
		strTemp		=	CString(m_CurStock.GetStockInfo().GetStockName()) + "(" + m_CurStock.GetStockInfo().GetStockCode() + ")";
		strTemp		+=	(LPCTSTR)AfxGetKTypeString( m_nCurKType );
		strNoKData	=	strTemp + strNoKData;
		CFont	* pOldFont	=	AfxSelectDCFont( pDC, 14 );
		pDC->SetBkColor( clrBK );
		pDC->SetTextColor( clrTitle );
		pDC->TextOut( m_rectAll.left+20, m_rectAll.top+20,  strNoKData );
		pDC->SelectObject( pOldFont );
	}
}

// 显示获取更多数据请点击菜单“数据->历史数据下载...
void CStockGraph::DrawNotExistStockDataInfo( CDC * pDC )
{
	DECLARE_COLOR_DEFINATION

	CClientDC	dc(m_pParent);
	if( NULL == pDC )
		pDC		=	&dc;

	//CKData	&	kdata	=	m_CurStock.GetKData(m_nCurKType);
	//if( 0 == kdata.GetSize() )
	{
		CString	strNoKData, strTemp;
		strNoKData.LoadString( IDS_STOCKGRAPH_NOKDATA );
		strTemp		=	CString(m_CurStock.GetStockInfo().GetStockName()) + "(" + m_CurStock.GetStockInfo().GetStockCode() + ")";
		strTemp		+=	(LPCTSTR)AfxGetKTypeString( m_nCurKType );
		strNoKData	=	strTemp + strNoKData;
		CFont	* pOldFont	=	AfxSelectDCFont( pDC, 14 );
		pDC->SetBkColor( clrBK );
		pDC->SetTextColor( clrTitle );
		pDC->TextOut( m_rectAll.left+20, m_rectAll.top+20,  strNoKData );
		pDC->SelectObject( pOldFont );
	}
}

void CStockGraph::DrawDateAxis( CDC * pDC )
{
	CHECK_NODATATODRAW
	DECLARE_COLOR_DEFINATION

	CRect	rect	=	m_rectCenter;
	rect.top	=	m_rectAll.bottom - m_nMarginBottom + 1;
	pDC->FillSolidRect( &rect, clrBK );
	pDC->SetBkColor( clrBK );
	pDC->SetTextColor( clrDJ );

	CKData & kdata	=	m_CurStock.GetKData( m_nCurKType );
	BOOL	bDayOrMin	=	CKData::IsDayOrMin(m_nCurKType);
	int		y	=	m_rectAll.bottom - m_nMarginBottom + 2;
	CFont	* pOldFont	=	AfxSelectDCFont( pDC, 12 );
	int	xLast	=	0;
	for( int k=m_nIndexStart; k<=m_nIndexEnd; k++ )
	{
		if( k > 0 && k < kdata.GetSize() )
		{
			CSPTime	sptimeLast, sptime;
			if( sptimeLast.FromStockTime( kdata.ElementAt(k-1).m_date, bDayOrMin ) //取日期 YYYYMMDD
				&& sptime.FromStockTime( kdata.ElementAt(k).m_date, bDayOrMin ) )
			{
				CString	strText;
				if( (CKData::ktypeDay == m_nCurKType )
					&& (sptimeLast.GetMonth() != sptime.GetMonth()) )
				{
						strText	=	sptime.Format( _T("%m") );
			    }
				else if( (CKData::ktypeWeek == m_nCurKType || CKData::ktypeMonth == m_nCurKType)
					&& (sptimeLast.GetYear() != sptime.GetYear()) )
				{
						strText	=	sptime.Format( _T("%y") );
				}
				else if ( !CKData::IsDayOrMin(m_nCurKType)        //  分钟K线时间坐标 D/M
					&& (sptimeLast.GetDay() != sptime.GetDay()) 
					/*&& (sptimeLast.GetMonth() != sptime.GetMonth()*/)
				{				
					strText	=	sptime.Format( _T("%d") );
                    // strText.Format(_T("%d"),k);
				}
				CRect	rectK;
				if( strText.GetLength() > 0 && GetOneKLineRect( k, &rectK, NULL, NULL, NULL ) && xLast < rectK.left )
				{
					pDC->TextOut( rectK.left, y, strText );
					xLast	=	rectK.left + pDC->GetTextExtent(strText).cx;
				}
			}
		} // if( k > 0 && k < kdata.GetSize() )
	}
	pDC->SelectObject( pOldFont );
}

void CStockGraph::DrawKLine(CDC * pDC )
{
   	DECLARE_COLOR_DEFINATION

	CKData	& kdata	=	m_CurStock.GetKData(m_nCurKType);
 
	CRect  rect1;
//
	//rect1.top	 =	m_rectAll.top + m_nMarginTop;
	//rect1.left   =	m_rectAll.left+1;
	//rect1.bottom =	m_rectKLine.bottom  ;
	//rect1.right	 =	m_rectKData.left + KLINE_OFFSET;

//水平显示
	//rect1.left = m_rectKLine.left + 40;
	//rect1.top= m_rectAll.top + 2;
   // rect1.right = m_rectKLine.right;
	//rect1.bottom= rect1.top + m_nMarginTop - 5;
   // pDC->FillSolidRect( &rect1,clrBK );
	
	pDC->FillSolidRect( &m_rectKLine, clrBK );
	pDC->Draw3dRect( &m_rectKLine, clrBorder, clrBorder );
	
  
	// Draw Title
	CFont	* pOldFont	=	AfxSelectDCFont( pDC, 14 ); //add 08.01.29

	CString	strTitle, strText;
	pDC->SetTextColor( clrTitle );
	pDC->SetBkColor( clrBK );
	strTitle	=	AfxGetKTypeString( m_nCurKType );
	pDC->TextOut( m_rectAll.left+8, m_rectAll.top+2, strTitle );
	strTitle.LoadString( IDS_STOCKGRAPH_KLINE );
	pDC->TextOut( m_rectKLine.left+1, m_rectAll.top+2, strTitle );   //显示"K 线" 标题

	pDC->SelectObject( pOldFont );  //add 08.01.29
   
	double	dMin = 0, dMax = 0;
	if( !GetKLineMinMaxInfo( m_nIndexStart, m_nIndexEnd, &dMin, &dMax ) )
		return;

    double D = (dMax-dMin) / 40;
    dMax= dMax + D * 1.5;
    dMin  = dMin - D * 2;
	//InflateRect(m_rectKLineCenter,0,-2); //??

	m_dMaxPrice			=	dMax;
	m_dMinPrice			=	dMin;
   
	// Draw Axis
	DrawAxis( pDC, m_rectKLine, m_nHeightSubtitle, 10, dMin, dMax, TRUE, TRUE,  m_nDotCount/*2*/ );

	// KLine Attribute Rect
	CRect	rectKLineAttrib = m_rectKLineCenter;
	rectKLineAttrib.top -= m_nHeightSubtitle;

	// Draw STT_DJ and STT_CW
	CSPDWordArray	&	anTech	=	AfxGetProfile().GetGraphTechsKLine( );
	int k;
	for(  k=0; k<anTech.GetSize(); k++ )
		if( STT_DJ == anTech[k] || STT_CW == anTech[k] )
			DrawKLineAttribute( pDC, anTech[k], rectKLineAttrib, FALSE, dMin, dMax );

     //显示最高
      //C.Font.Name := 'ARIAL';
      //C.Font.Height := Max(2,Round(_height_(R)*0.05)-1);	
	  CStockInfo & info	=	m_CurStock.GetStockInfo();
      CString strPrice;
	  CString strPriceFmt;
	  strPriceFmt.Format( _T("%%.%df"), info.DigitBit() );
	  int x;
	  // 取当前字体大小
	  pOldFont	=	AfxSelectDCFont( pDC, 12); //add 08.01.29

      //CFont *curFont = pDC->GetCurrentFont();
      //LOGFONT curLogFont;
	 // LOGFONT newLogFont;
      //curFont->GetLogFont( &curLogFont );
      //long nFontWidth = curLogFont.lfWidth;
      //long nFontHeight = curLogFont.lfHeight;

	  TEXTMETRIC tm;
	  pDC->GetTextMetrics(&tm);   
	  long nFontWidth = tm.tmAveCharWidth;
      long nFontHeight = tm.tmHeight + tm.tmExternalLeading;

      for (std::vector<UINT>::const_iterator iter = m_HighIndex.begin(); iter != m_HighIndex.end(); iter++)
	  {
	    int nIndex = *iter;
        float f1= m_HighPrice[nIndex -m_nIndexStart];
		
		if ( kdata[nIndex].m_fOpen < 1e-4 ) continue;// 停盘日处理
		
		strPrice.Format(strPriceFmt,f1);
		strPrice.TrimLeft();
		strPrice.TrimRight();
		
	    int	yHigh	=	int( m_rectKLineCenter.bottom - m_rectKLineCenter.Height() * (f1 - dMin) / (dMax-dMin) );
        x = m_rectKLineCenter.left + (nIndex-m_nIndexStart) * m_nThickness;

		//DWORD tmWH = GetTextExtent(pDC->m_hDC,strPrice.GetBuffer(),strPrice.GetLength());        
		int TW =  nFontWidth * strPrice.GetLength(); 
        int TH =  nFontHeight;

        //TW = LOWORD(tmWH);
		//TH = HIWORD(tmWH);

		//TW = pDC->GetTextExtent(strPrice).cx;
        //TH = pDC->GetTextExtent(strPrice).cy;

		x = m_rectKLineCenter.left +(nIndex-m_nIndexStart) * m_nThickness - TW/2 + m_nThickness/2 - 1;
        yHigh	= yHigh	- TH + 1;
		if ( x < m_rectKLineCenter.left ) x= m_rectKLineCenter.left ;
       /* if ( yHigh <= m_rectKLineCenter.top ) 
		{
			pDC->MoveTo( x, yHigh);   
			yHigh = m_rectKLineCenter.top + TH/2 + 1;
			x += TW;  
			pDC->LineTo( x, yHigh );
		}*/
        pDC->SetTextColor( RGB(255,0,0 ) | 0x008000 );

		/*int OFH =TH  * 0.9;
        CString S1,S2,S3;
      //  CRect R1,R2,R3;
		nIndex	=	strPrice.Find( _T('.') );
		if( -1 != nIndex )
		{
			S1	=	strPrice.Left( nIndex );
			S2	=	strPrice.Mid( nIndex+1 );
			S3 = _T("");
			//SubtractRect(lprcDst As RECT, lprcSrc1 As RECT, lprcSrc2 As RECT)
			//这个函数会装载矩形lprcDst,它是在矩形lprcSrc1中减去lprcSrc2得到的结果 
			// with Canvas, Rect do R2 := Classes.Rect(Left+_width_(Rect)-TextWidth(S2),Top,Right,Bottom);
			//SubtractRect(R1, Rect, R2);

			//nIndex	=	S2.Find( _T('%') );
			if( -1 != nIndex )
			{
				S3 = _T("%");
				//with Canvas, R2 do R3 := Classes.Rect(Left+_width_(R2)-TextWidth(S3),Top,Right,Bottom);
				//SubtractRect(R2, R2, R3);
			}*/
			//R2.Bottom := R2.Bottom - 3;
			//Font.Height := OFH 
           // pDC->TextOut( x - TW,yHigh , S1 );
			//DrawText(Canvas.Handle,PChar(S1),-1,R1,_textRect_fmt_(Alignment,Layout));
			//curLogFont.lfUnderline =  TRUE;
		//curLogFont.lfFaceName = _T("ARIAL");
            //CFont newFont;
            //CFont *oldFont;
            //newFont.CreateFontIndirect(&curLogFont);
            //oldFont = pDC->SelectObject(&newFont );
			//Font.Height := _if_(I in [2,3],OFH * 9 div 10,OFH);
			//DrawText(Canvas.Handle,PChar(S2),-1,R2,_textRect_fmt_(Alignment,Layout));
		   // pDC->TextOut( x - TW -nFontWidth * S1.GetLength(),yHigh , S2 );
			//if S3 <> '' then DrawText(Canvas.Handle,PChar(S3),-1,R3,_textRect_fmt_(Alignment,Layout));
            //pDC->SelectObject(oldFont );
		//}
		//else
		//{
			pDC->TextOut( x,yHigh , strPrice );
		//}	
       }
     

     //显示最低
     // C.Font.Name := 'ARIAL';
     // C.Font.Height := Max(2,Round(_height_(R)*0.05)-1);  
      for (std::vector<UINT>::const_iterator iter =m_LowIndex.begin(); iter != m_LowIndex.end(); iter++)
	  {
		 int nIndex = *iter;
		 float f1= m_LowPrice[nIndex -m_nIndexStart];

		 if ( kdata[nIndex].m_fOpen < 1e-4 ) continue;// 停盘日处理

		 strPrice.Format(strPriceFmt,f1);
         strPrice.TrimLeft();
		 strPrice.TrimRight();
       
		int	yLow	=	int( m_rectKLineCenter.bottom - m_rectKLineCenter.Height() * (f1 - dMin) / (dMax-dMin) );
	    x = m_rectKLineCenter.left + (nIndex-m_nIndexStart) * m_nThickness;
        int TW =  nFontWidth * strPrice.GetLength(); 
        int TH =  nFontHeight;
		//TW = pDC->GetTextExtent(strPrice).cx;
        //TH = pDC->GetTextExtent(strPrice).cy;

		x = m_rectKLineCenter.left +(nIndex-m_nIndexStart) * m_nThickness - TW/2 + m_nThickness/2 - 1;
        yLow	= yLow	+ 1;  
		if ( x < m_rectKLineCenter.left ) x= m_rectKLineCenter.left ;
		pDC->SetTextColor( RGB(0,255,255 ) & 0xC0C0C0 );
		pDC->TextOut( x,yLow , strPrice );
		

        //str := _vs_(LA[LIndex[I]]);
        //TW := C.TextWidth(str);
        //TH := C.TextHeight(str);
        //X1 := 1 + UnitWidth * LIndex[I]- TW div 2 + UnitWidth div 2 - 1;
        //Y1 := Fy2Iy(LA[LIndex[I]],R,High,Low) + 1;
    	//	Rt := Rect(X1,Y1,X1+TW,Y1+TH);
        //Rt.Left := Min(R.Right - TW - 1,Max(Rt.Left,1));
        //Rt.Right := Rt.Left + TW + 2;
        //if not IS_FRACTION_UNDERLINE then
        //	_textRectBackground_(C,Rt,str,C.Font.Height,clAqua and $C0C0C0,GRID.Color,taCenter,tlTop,True) //小数点显示
        //else _textRect_(C,Rt,str,clAqua and $C0C0C0,GRID.Color,taCenter,tlTop,False);  //下划线显示小数
	 }
     pDC->SelectObject( pOldFont );  //add 08.01.29
	// Draw K Line
	for( k=m_nIndexStart; k<=m_nIndexEnd; k++ )
	{
		DrawOneKLine( pDC, k, k, &kdata, dMin, dMax, FALSE );
	}

/* 显示最高/最低
 //   for (std::list<float>::reverse_iterator iter=m_HighPrice.rbegin(); iter!=m_HighPrice.rend(); iter++)
	const_iterator
	for (std::list<UINT>::const_iterator iter = m_HighIndex.begin(); iter != m_HighIndex.end(); iter++)
	{
		int nIndex = *iter;
	}

procedure DRAW_UNDERLINE_ALIGNCENTER(	Canvas: TCanvas; Rect: TRect; Str: string; fgColor: TColor;
                                      bgColor: TColor; Alignment: TAlignment; Layout: TTextLayout);
var
	I, OFH: Integer;
  S1,S2,S3: string;
  R1,R2,R3,TR: TRect;
begin
	OFH := Canvas.Font.Height;
  I := Pos('.', Str);
  if I < 1 then DrawText(Canvas.Handle,PChar(Str),-1,Rect,_textRect_fmt_(Alignment,Layout))     //不存在小数点
  else begin
    S1 := Copy(Str,1,I-1);
    S2 := Copy(Str,I+1,Length(Str)-I);
    S3 := '';
    R1 := Rect;
    R2 := Rect;
    if (Length(S1) + Length(S2)) > 0 then R1.Right := R1.Left + Round(_width_(Rect) * Length(S1) / (Length(S1) + Length(S2)));
    R2.Left := R1.Left + _width_(R1);
    R2.Right := R2.Left + _width_(Rect) - _width_(R1);
    Canvas.Font.Height := OFH * 9 div 10;
    if (Length(S2)>1) and (S2[Length(S2)]in['%']) then     //存在%吗?
    begin
      S2 := Copy(S2,1,Length(S2)-1);
      S3 := '%';
      TR := R2;
      with Canvas, TR do R2 := Classes.Rect(Left,Top,Left+TextWidth(S2),Bottom);
      SubtractRect(R3, TR, R2);
    end;

   R2.Bottom := R2.Bottom - 3;

   for I := 1 to 3 do
    begin
      Canvas.Font.Style := Canvas.Font.Style - [fsUnderLine];
      if I = 2 then Canvas.Font.Style := Canvas.Font.Style + [fsUnderLine];
      Canvas.Font.Height := _if_(I in [2,3],OFH * 9 div 10,OFH);
      case I of
        1: DrawText(Canvas.Handle,PChar(S1),-1,R1,_textRect_fmt_(taRightJustify,Layout)); // 显示整数部分
        2: DrawText(Canvas.Handle,PChar(S2),-1,R2,_textRect_fmt_(taLeftJustify,Layout));  //显示小数点部分
        3: if S3 <> '' then DrawText(Canvas.Handle,PChar(S3),-1,R3,_textRect_fmt_(taLeftJustify,Layout));  //显示%
      end;
    end;
  end;
end;
*/
	// Draw Tech KLine except STT_DJ and STT_CW
	for( k=0; k<anTech.GetSize(); k++ )
		if( STT_DJ != anTech[k] && STT_CW != anTech[k] )
			DrawKLineAttribute( pDC, anTech[k], rectKLineAttrib, FALSE, dMin, dMax );
	if( anTech.GetSize() > 0 )
		DrawKLineAttribute( pDC, anTech[0], rectKLineAttrib, TRUE, dMin, dMax );

	// Draw Select Line
	DrawSelectionLine( pDC,m_nCrossCursorPos, FALSE );

}

BOOL CStockGraph::GetKLineMinMaxInfo( int nStart, int nEnd, double * pdMin, double * pdMax )
{
	CKData	& kdata	=	m_CurStock.GetKData(m_nCurKType);
	CKLine	kline( &kdata );
	double	dMin = 0, dMax = 0, dMinTemp = 0, dMaxTemp = 0;
	if( nStart < 0 || nEnd < 0 || ! kline.GetMinMaxInfo( nStart, nEnd, &dMin, &dMax ) )
		return FALSE;

	CSPDWordArray	&	anTech	=	AfxGetProfile().GetGraphTechsKLine( );
	for( int k=0; k<anTech.GetSize(); k++ )
	{
		UINT	nTech	=	anTech[k];
		switch( nTech )
		{
		case STT_MA:
			{
				CMA	ma( &kdata ); 
				if( ma.GetMinMaxInfo( nStart, nEnd, &dMinTemp, &dMaxTemp ) )
				{
					if( dMinTemp < dMin )	dMin	=	dMinTemp;
					if( dMaxTemp > dMax )	dMax	=	dMaxTemp;
				}
			}
			break;
        case STT_EXPMA:
			{
				CMA	expma( &kdata ); 
				if( expma.GetMinMaxInfo( nStart, nEnd, &dMinTemp, &dMaxTemp ) )
				{
					if( dMinTemp < dMin )	dMin	=	dMinTemp;
					if( dMaxTemp > dMax )	dMax	=	dMaxTemp;
				}
			}
			break;
		case STT_BBI:
			{
				CBBI	bbi( &kdata ); 
				if( bbi.GetMinMaxInfo( nStart, nEnd, &dMinTemp, &dMaxTemp ) )
				{
					if( dMinTemp < dMin )	dMin	=	dMinTemp;
					if( dMaxTemp > dMax )	dMax	=	dMaxTemp;
				}
			}
			break;
		case STT_BOLL:
			{
				CBOLL	boll( &kdata ); 
				if( boll.GetMinMaxInfo( nStart, nEnd, &dMinTemp, &dMaxTemp ) )
				{
					if( dMinTemp < dMin )	dMin	=	dMinTemp;
					if( dMaxTemp > dMax )	dMax	=	dMaxTemp;
				}
			}
			break;
		case STT_PV:
			{
				CPV	pv( &kdata ); 
				if( pv.GetMinMaxInfo( nStart, nEnd, &dMinTemp, &dMaxTemp ) )
				{
					if( dMinTemp < dMin )	dMin	=	dMinTemp;
					if( dMaxTemp > dMax )	dMax	=	dMaxTemp;
				}
			}
			break;
		case STT_SAR:
			{
				CSAR	sar( &kdata ); 
				if( sar.GetMinMaxInfo( nStart, nEnd, &dMinTemp, &dMaxTemp ) )
				{
					if( dMinTemp < dMin )	dMin	=	dMinTemp;
					if( dMaxTemp > dMax )	dMax	=	dMaxTemp;
				}
			}
			break;
		case STT_DJ:
		case STT_CW:
			break;
		default:;
			ASSERT( FALSE );
		}
	}
	
	if( pdMin )
		*pdMin	=	dMin;
	if( pdMax )
		*pdMax	=	dMax;
	return TRUE;
}

void CStockGraph::DrawOneKLine(CDC * pDC, int nIndexPos, int nIndexKD, CKData * pKData, double dMin, double dMax, BOOL bGreyed )
{
	DECLARE_COLOR_DEFINATION

	// Check Valid
	ASSERT( pDC && nIndexPos >= m_nIndexStart && nIndexPos <= m_nIndexEnd && nIndexPos >= 0 );
	if( !(pDC && nIndexPos >= m_nIndexStart && nIndexPos <= m_nIndexEnd && nIndexPos >= 0) )
		return;
	if( !pKData || nIndexKD < 0 || nIndexKD >= pKData->GetSize() )
		return;
	if( dMax-dMin < 1e-4 )
		return;

	// Get Region
	CRect	rectK, rcEntity;
	long	xMedium	=	0;
	if( !GetOneKLineRect( nIndexPos, &rectK, &rcEntity.left, &rcEntity.right, &xMedium ) )
		return;

	int	xStart	=	rectK.left;
	int	xEnd	=	rectK.right;
	ASSERT( xEnd <= m_rectKLineCenter.right );
	if( xEnd > m_rectKLineCenter.right )
		return;

	KDATA	kd	=	pKData->ElementAt(nIndexKD);

// 停盘日处理,按昨收盘处理来画?
	if ( kd.m_fVolume < 1e-4 && kd.m_fAmount < 1e-4  )
		//kd.m_fOpen == 0  && kd.m_fLow ==0 && kd.m_fHigh ==0  && kd.m_fClose ==0 )
	{
		return;
	}
	// Set rcEntity's top and bottom, set yLow, yHigh
	 
	CFont *curFont = pDC->GetCurrentFont();
    LOGFONT curLogFont;
	curFont->GetLogFont( &curLogFont );
    long nFontHeight = curLogFont.lfHeight;

	int	yLow	=	int( m_rectKLineCenter.bottom - m_rectKLineCenter.Height() * (kd.m_fLow - dMin) / (dMax-dMin) );
	int	yHigh	=	int( m_rectKLineCenter.bottom - m_rectKLineCenter.Height() * (kd.m_fHigh - dMin) / (dMax-dMin) );
	int	yOpen	=	int( m_rectKLineCenter.bottom - m_rectKLineCenter.Height() * (kd.m_fOpen - dMin) / (dMax-dMin) );
	int	yClose	=	int( m_rectKLineCenter.bottom - m_rectKLineCenter.Height() * (kd.m_fClose - dMin) / (dMax-dMin) );

	rcEntity.top	=	min( yOpen, yClose );
	rcEntity.bottom	=	max( yOpen, yClose ) + 1;
	
	if( CStockGraph::klineCandle == m_nCurKLineMode ) //蜡烛线
	{
		// Draw Entity
		COLORREF	clr	=	clrRise;
		if( kd.m_fClose < kd.m_fOpen )
			clr	=	clrFallEntity;
		if( kd.m_date > m_dwLatestDate )
			clr	=	clrNewKLine;
		if( bGreyed )
			clr	=	clrDJ;
		pDC->SetBkColor( clrBK );
		if( kd.m_fClose < kd.m_fOpen )
			pDC->FillSolidRect( &rcEntity, clr );
		else
			pDC->Draw3dRect( &rcEntity, clr, clr );
		
		// Draw Line
		CPen	pen( PS_SOLID, 1, clr );
		CPen *pOldPen = pDC->SelectObject( &pen );
		pDC->MoveTo( xMedium, yHigh );
		pDC->LineTo( xMedium, rcEntity.top );
		pDC->MoveTo( xMedium, rcEntity.bottom );
		pDC->LineTo( xMedium, yLow );

		pDC->SelectObject( pOldPen );
	}
	else if( CStockGraph::klineAmerica == m_nCurKLineMode ) //美国线
	{
		// Draw Entity
		COLORREF	clr	=	clrRise;
		if( kd.m_date > m_dwLatestDate )
			clr	=	clrNewKLine;
		if( bGreyed )
			clr	=	clrDJ;
		pDC->SetBkColor( clrBK );
		
		// Draw Line
		CPen	pen( PS_SOLID, 1, clr );
		CPen *pOldPen = pDC->SelectObject( &pen );
		pDC->MoveTo( xStart, yHigh );
		pDC->LineTo( xStart, yLow );
		pDC->MoveTo( xStart, yClose );
		pDC->LineTo( xEnd, yClose );

		pDC->SelectObject( pOldPen );
	}
	else if( CStockGraph::klineTower == m_nCurKLineMode ) //宝塔线
	{
		/*
		在起步股價和當日收盤價之間畫線,股價上漲時,畫紅色空心實體,股價下跌時,畫綠色實心實體。
		若前一交易日是上漲紅線,而次日下跌,股價未跌破前一天紅線低點的那一部分跌幅仍畫紅線,跌破部分畫綠線。
		若前一交易日為下跌綠線,而次日上漲,股價未漲過前一天綠線低點的那一部分漲幅仍畫綠色實體,漲過部分才畫紅線。
		*/
		// Draw Entity
		COLORREF	clr	=	clrRise;
		if( kd.m_fClose < kd.m_fOpen )
			clr	=	clrFallEntity;
		if( kd.m_date > m_dwLatestDate )
			clr	=	clrNewKLine;
		if( bGreyed )
			clr	=	clrDJ;
		pDC->SetBkColor( clrBK );
		if( kd.m_fClose < kd.m_fOpen )
			pDC->FillSolidRect( &rcEntity, clr );
		else
			pDC->Draw3dRect( &rcEntity, clr, clr );

		if( nIndexKD > 0 )
		{
			KDATA	kdLast	=	pKData->ElementAt(nIndexKD-1);
			int	yOpenLast	=	int( m_rectKLineCenter.bottom - m_rectKLineCenter.Height() * (kdLast.m_fOpen - dMin) / (dMax-dMin) );
			int	yCloseLast	=	int( m_rectKLineCenter.bottom - m_rectKLineCenter.Height() * (kdLast.m_fClose - dMin) / (dMax-dMin) );
			if( kdLast.m_fClose > kdLast.m_fOpen && kd.m_fClose < kd.m_fOpen )
			{
				rcEntity.bottom	=	min(yOpenLast,rcEntity.bottom);
				if( rcEntity.bottom > rcEntity.top )
				{
					pDC->FillSolidRect( &rcEntity, clrBK );
					clr	=	clrRise;
					if( kd.m_date > m_dwLatestDate )
						clr	=	clrNewKLine;
					if( bGreyed )
						clr	=	clrDJ;
					pDC->Draw3dRect( &rcEntity, clr, clr );
				}
			}
			else if( kdLast.m_fClose < kdLast.m_fOpen && kd.m_fClose > kd.m_fOpen )
			{
				rcEntity.top	=	max(yOpenLast,rcEntity.top);
				if( rcEntity.bottom > rcEntity.top )
				{
					clr	=	clrFallEntity;
					if( kd.m_date > m_dwLatestDate )
						clr	=	clrNewKLine;
					if( bGreyed )
						clr	=	clrDJ;
					pDC->FillSolidRect( &rcEntity, clr );
				}
			}
		}
	}
}

void CStockGraph::DrawBorderMovingLine( CDC * pDC, CPoint point, int nColorIndex, BOOL bAddOrRemove )
{
	ASSERT( m_pParent && ::IsWindow( m_pParent->GetSafeHwnd() ) );
	if( !m_pParent || !::IsWindow(m_pParent->GetSafeHwnd()) )
		return;
    
	if ( point.x == 0 && point.y == 0  ) return ;

	CClientDC	dc(m_pParent);
	if( NULL == pDC )
		pDC		=	&dc;

	//CPen	penLine( PS_SOLID, 1, AfxGetProfile().GetColor( CColorClass::clrBorder ) );
	
	CPen	penLine( PS_SOLID, 1,nColorIndex);

	CPen * pOldPen = pDC->SelectObject( &penLine );
	int nOldDrawMode = pDC->SetROP2( R2_XORPEN );

	pDC->MoveTo( m_rectKLine.left+2, point.y );
	pDC->LineTo( m_rectKLine.right-2, point.y );

	pDC->SetROP2( nOldDrawMode );
	pDC->SelectObject( pOldPen );
}

BOOL CStockGraph::DrawSelectionLine( CDC * pDC, CPoint point, BOOL bAddOrRemove )
{
	ASSERT( m_pParent && ::IsWindow( m_pParent->GetSafeHwnd() ) );
	if( !m_pParent || !::IsWindow(m_pParent->GetSafeHwnd()) )
		return FALSE;

    BOOL bSelected = FALSE;

//	if ( point.x == 0 && point.y == 0 && !bAddOrRemove) return  bSelected ;

	CClientDC	dc(m_pParent);
	if( NULL == pDC )
		pDC		=	&dc;

	CPen	penVLine( PS_SOLID, 1, AfxGetProfile().GetColor( CColorClass::clrLine1 ) );
	CPen * pOldPen = pDC->SelectObject( &penVLine );
	int nOldDrawMode = pDC->SetROP2( R2_XORPEN );

	CRect	rectK;
	long	xMedium = 0;
	if( GetOneKLineRect( m_nIndexCurrent, &rectK, NULL, NULL, &xMedium ) )
	{
		pDC->MoveTo( xMedium, m_rectCenter.bottom - m_nMarginBottom -2 );
		pDC->LineTo( xMedium, m_rectKLineCenter.top );

		if ( m_nCrossCursorPos.x > 0 )
		{
		  pDC->MoveTo( m_rectKLine.left+2, m_nCrossCursorPos.y );
	      pDC->LineTo( m_rectKLine.right-2, m_nCrossCursorPos.y );
		 // CString strDate;
		 // strDate.Format(_T( "1x=%d  1y%d,%d"),point.x,point.y ,xMedium);
	      //pDC->TextOut(100, 170, strDate );
		 
		}
		//m_nCrossCursorPos = point;
		bSelected = TRUE;
	}

	for( int k=0; k<m_anIndexCurrent.GetSize(); k++ )
	{
		if( ((int)m_anIndexCurrent[k]) == m_nIndexCurrent )
			continue;
		if( GetOneKLineRect( m_anIndexCurrent[k], &rectK, NULL, NULL, &xMedium ) )
		{
			pDC->MoveTo( xMedium, m_rectKLineCenter.bottom - m_nMarginBottom -2 );
			pDC->LineTo( xMedium, m_rectKLineCenter.top );
            if ( m_nCrossCursorPos.x > 0 )
			{
			  pDC->MoveTo( m_rectKLine.left+2, m_nCrossCursorPos.y );
	          pDC->LineTo( m_rectKLine.right-2,m_nCrossCursorPos.y );
			  //CString strDate;
		     // strDate.Format(_T( "2x=%d  2y%d"),point.x,point.y );
	         // pDC->TextOut(100, 190, strDate );
		   
			}
		//	m_nCrossCursorPos = point;
			bSelected = TRUE;
		}
	}
   
	//if( ::IsWindow( m_wndKData.GetSafeHwnd() ))
	//{
	//   m_wndKData.SetWindowPos( NULL,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW|SWP_NOZORDER );
	//}
  //  m_wndKData.SetWindowPos( NULL,xMedium, m_rectKData.top, m_rectKData.Width(), m_rectKData.Height(),SWP_SHOWWINDOW );

	pDC->SetROP2( nOldDrawMode );
	pDC->SelectObject( pOldPen );

	return (bSelected );
}

void CStockGraph::DrawSelectionRect( CDC * pDC, CPoint ptBegin, CPoint ptEnd, BOOL bAddOrRemove )
{
	ASSERT( m_pParent && ::IsWindow( m_pParent->GetSafeHwnd() ) );
	if( !m_pParent || !::IsWindow(m_pParent->GetSafeHwnd()) )
		return;

	CClientDC	dc(m_pParent);
	if( NULL == pDC )
		pDC		=	&dc;

	CPen	penVLine( PS_SOLID, 1, AfxGetProfile().GetColor( CColorClass::clrLine2 ) );
	CPen * pOldPen = pDC->SelectObject( &penVLine );
	int nOldDrawMode = pDC->SetROP2( R2_XORPEN );

	CRect	rect;
	rect.left	=	min(ptBegin.x,ptEnd.x);
	rect.right	=	max(ptBegin.x,ptEnd.x);
	rect.top	=	min(ptBegin.y,ptEnd.y);
	rect.bottom	=	max(ptBegin.y,ptEnd.y);

	pDC->MoveTo( rect.left, rect.top );
	pDC->LineTo( rect.right, rect.top );
	pDC->LineTo( rect.right, rect.bottom );
	pDC->LineTo( rect.left, rect.bottom );
	pDC->LineTo( rect.left, rect.top );

	pDC->SetROP2( nOldDrawMode );
	pDC->SelectObject( pOldPen );
}

// 显示K线选中点的数据
void CStockGraph::DrawKData( CDC * pDC)
{
#ifdef _FLOAT_WND_

    CRect rect1;
    rect1.left = m_rectKLine.left + 40;
    rect1.top= m_rectAll.top + 2;
    rect1.right = m_rectKLine.right;
    rect1.bottom= rect1.top + m_nMarginTop - 5;

	if( !::IsWindow( m_wndKData.GetSafeHwnd() ) )
	{
	
		if( !m_wndKData.Create( NULL, NULL, WS_CHILD/*|WS_OVERLAPPED|WS_SYSMENU|WS_CAPTIONWS_TABSTOP*/,  rect1 /*m_rectKData*/, m_pParent, 0 ) )
		{
			TRACE( "Failed to create m_wndKData!\n" );
			return;
		}

		m_wndKData.SetWindowPos( NULL, 0,0,0,0,	SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE );

		
		//m_wndKData.SetWindowPos( NULL, m_rectKData.left, m_rectKData.top, m_rectKData.Width(), m_rectKData.Height(),
		//						SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE );

		
		//LONG lStyle = GetWindowLong(m_wndKData.GetSafeHwnd(),GWL_EXSTYLE)|0x00080000;   
       // SetWindowLong( m_wndKData.GetSafeHwnd(),GWL_EXSTYLE,lStyle);  
		// COLORREF maskColor = RGB(0, 255, 0);
		//SetLayeredWindowAttributes(maskColor,255*0.40,LWA_ALPHA);
        //SetLayeredWindowAttributes(m_wndKData.GetSafeHwnd(),RGB(0,0,0),BYTE((255*70)/100),2);   
	
			// allow bar to be resized when floating
	  //  m_wndKData.SetBarStyle(m_wndKData. | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	}
#ifdef _FLOAT_WND_  
//	
#endif

#endif
	CKData	&kdata	=	m_CurStock.GetKData(m_nCurKType);
	if( m_nIndexCurrent >= 0 && m_nIndexCurrent < kdata.GetSize() )
	{
#ifdef _FLOAT_WND_
		
		m_wndKData.SetWindowPos( NULL,rect1.left, rect1.top, rect1.Width(), rect1.Height(),SWP_SHOWWINDOW|SWP_NOZORDER );
	    
		CClientDC	dc(&m_wndKData);
	    CRect		rectClient;
	    m_wndKData.GetClientRect( &rectClient );
      
		m_wndKData.SetWindowPos( NULL, 0, 0, 0, 0,/* SWP_NOZORDER|*/SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );
		DrawKData( &dc, rectClient );
#else
		CRect		rect;
		m_pParent->GetClientRect( &rect );
		DrawKData( pDC, rect );
#endif
		
	}
#ifdef _FLOAT_WND_
	else
	//	m_wndKData.SetWindowPos( NULL, 0, 0, 0, 0,/* SWP_NOZORDER|*/SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );
		m_wndKData.SetWindowPos( NULL, 0, 0, 0, 0, /*SWP_NOZORDER|*/SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW );
#endif
}

// 显示某日K线的数据
void CStockGraph::DrawKData( CDC * pDC, CRect rect )
{
   
	if (   m_nIndexCurrent  < 0  ) return;
  //  if ( m_anIndexCurrent.GetSize() == 0 ) return;

	CHECK_NODATATODRAW
	DECLARE_COLOR_DEFINATION

	CStockInfo & info	=	m_CurStock.GetStockInfo();
	CKData &kdata		=	m_CurStock.GetKData( m_nCurKType );
	CKData &kdataDown	=	m_CurStockDown.GetKData( m_nCurKType );
   
	// Prepare kd and fLastClose
	KDATA	kd;
	memset( &kd, 0, sizeof(kd) );
	float	fLastClose	=	0;
	double	dAmount = 0, dVolume = 0, dPriceAverage = 0, dRatioChangeHand = 0, dVolumeDown = 0;
	if( m_anIndexCurrent.GetSize() > 0 
		&& (int)m_anIndexCurrent[0] < kdata.GetSize() && (int)m_anIndexCurrent[m_anIndexCurrent.GetSize()-1] < kdata.GetSize() )
	{
		int	nStart	=	m_anIndexCurrent[0];
		int	nEnd	=	m_anIndexCurrent[m_anIndexCurrent.GetSize()-1];
		kd.m_date	=	kdata.ElementAt(nStart).m_date;
		kd.m_fOpen	=	kdata.ElementAt(nStart).m_fOpen;
		kd.m_fLow	=	kdata.ElementAt(nStart).m_fLow;
		kd.m_fClose	=	kdata.ElementAt(nEnd).m_fClose;
		for( int k=0; k<m_anIndexCurrent.GetSize() && (int)m_anIndexCurrent[k]<kdata.GetSize(); k++ )
		{
			KDATA	& kdTemp	=	kdata.ElementAt(m_anIndexCurrent[k]);
			if( (int)m_anIndexCurrent[k] < nStart )
			{
				kd.m_fOpen	=	kdTemp.m_fOpen;
				kd.m_date	=	kdTemp.m_date;
				nStart	=	m_anIndexCurrent[k];
			}
			if( (int)m_anIndexCurrent[k] > nEnd )
			{
				kd.m_fClose=	kdTemp.m_fClose;
				nEnd	=	m_anIndexCurrent[k];
			}
			if( kd.m_fHigh < kdTemp.m_fHigh )
				kd.m_fHigh	=	kdTemp.m_fHigh;
			if( kd.m_fLow > kdTemp.m_fLow )
				kd.m_fLow	=	kdTemp.m_fLow;
			dAmount	+=	kdTemp.m_fAmount;
			dVolume	+=	kdTemp.m_fVolume;
			if( (int)m_anIndexCurrent[k] < kdataDown.GetSize() )
				dVolumeDown	+=	kdataDown.ElementAt(m_anIndexCurrent[k]).m_fVolume;
		}
		fLastClose	=	kd.m_fOpen;
		if( nStart - 1 >= 0 )
			fLastClose	=	kdata.ElementAt(nStart-1).m_fClose;
	}
	else if( m_nIndexCurrent >= 0 && m_nIndexCurrent < kdata.GetSize() )
	{
		kd	=	kdata.ElementAt(m_nIndexCurrent);
		dAmount	=	kd.m_fAmount;
		dVolume	=	kd.m_fVolume;
		if( m_nIndexCurrent < kdataDown.GetSize() )
			dVolumeDown	+=	kdataDown.ElementAt(m_nIndexCurrent).m_fVolume;
		fLastClose	=	kd.m_fOpen;
		if( m_nIndexCurrent - 1 >= 0 )
			fLastClose	=	kdata.ElementAt(m_nIndexCurrent-1).m_fClose;
	}
	else
	{
		return;
	}

	if( dVolume >= 1 )
		dPriceAverage	=	dAmount / dVolume;
	m_CurStockDown.GetStockInfo().GetRatioChangeHand( &dRatioChangeHand, dVolumeDown );

	// 相对换手率
	double	dMainChangeHand = CHSL::GetMainChangeHand( m_CurStockDown.GetStockInfo().GetMarket(), kdataDown, m_nIndexCurrent );
	double	dRelativeChangeHand = 1.0;
	if( dMainChangeHand > 1e-6 )
		dRelativeChangeHand	=	dRatioChangeHand / dMainChangeHand;

	
#ifdef _FLOAT_WND_
//注解掉的确为左边框时,显示的处理
   // Fill Background
	pDC->FillSolidRect( &rect, clrBK );

	//pDC->Draw3dRect( &rect, clrBorder, clrBorder );
	//pDC->SetBkColor( clrBK );
  
	CString	strTemp;
	int	x	=	rect.left + 1;
	int	y	=	rect.top;

 	CFont	* pOldFont	=	AfxSelectDCFont( pDC, 14 );
	pDC->SetBkColor( clrBK );
	
	pDC->SetTextColor( clrText );
	strTemp.LoadString( IDS_STOCKGRAPH_TIME );
   	//pDC->TextOut( x, y+2,   strTemp );
	if ( x + 100 < m_rectReport.left )
      pDC->TextOut( x , y,   strTemp );

	strTemp.LoadString( IDS_STOCKGRAPH_OPEN );
	//pDC->TextOut( x, y+37,  strTemp );
	if ( x + 210 < m_rectReport.left )
      pDC->TextOut( x + 110, y,   strTemp );

	strTemp.LoadString( IDS_STOCKGRAPH_HIGH );
	//pDC->TextOut( x, y+71,  strTemp );
	if ( x + 320 < m_rectReport.left )
       pDC->TextOut( x + 220, y,   strTemp );

	strTemp.LoadString( IDS_STOCKGRAPH_LOW );
	//pDC->TextOut( x, y+105, strTemp );
	if ( x + 430 < m_rectReport.left )
      pDC->TextOut( x + 330, y,   strTemp );

	strTemp.LoadString( IDS_STOCKGRAPH_CLOSE );
	//pDC->TextOut( x, y+139, strTemp );
	if ( x + 540 < m_rectReport.left )
      pDC->TextOut( x + 440, y,   strTemp );

	
	if ( !info.IsIndex() )
	{
       strTemp.LoadString( IDS_STOCKGRAPH_VOLUME );
	  //pDC->TextOut( x, y+173, strTemp );
	   if ( x + 650 < m_rectReport.left )
         pDC->TextOut( x + 550, y,   strTemp );

	   strTemp.LoadString( IDS_STOCKGRAPH_AMOUNT );
	  //pDC->TextOut( x, y+207, strTemp );
	   if ( x + 780 < m_rectReport.left )
         pDC->TextOut( x + 680, y,   strTemp );
    
	   strTemp.LoadString( IDS_STOCKGRAPH_PRICEAVERAGE );
	 // pDC->TextOut( x, y+241, strTemp );
	   if ( x + 890 < m_rectReport.left )
	     pDC->TextOut( x + 790, y,   strTemp );
	}
	else
	{
	   strTemp.LoadString( IDS_STOCKGRAPH_VOLUME );
	   //pDC->TextOut( x, y+173, strTemp );
	   if ( x + 650 < m_rectReport.left )
         pDC->TextOut( x + 550, y,   strTemp );

	   strTemp.LoadString( IDS_STOCKGRAPH_AMOUNT );
	//pDC->TextOut( x, y+207, strTemp );
	   if ( x + 820 < m_rectReport.left )
         pDC->TextOut( x + 720, y,   strTemp );
    
	}
#if 0

	strTemp.LoadString( IDS_STOCKGRAPH_RELATIVECHANGEHAND );
	pDC->TextOut( x, y+275, strTemp );
#endif

	//x	=	rect.right - 3;
	x += 90; 
	UINT nOldAlign = pDC->SetTextAlign( TA_RIGHT | TA_TOP );
	CSPTime	sptime;
	if( CKData::IsDayOrMin( m_nCurKType ) )
	{
		sptime.FromStockTimeDay( kd.m_date );
		strTemp	=	sptime.Format( _T("%y.%m.%d") );
	}
	else  // 5min K线指标
	{ 
	  try
	  {
		 // 日期为啥不对？，接口需要将接收到数据进行转换ToStockTimeMin();
		 sptime.FromStockTimeMin(  /*kd.m_time*/kd.m_date );  
		 // CSPTime sptime1( kd.m_time );
		  strTemp	=	sptime.Format( _T("%m.%d %H:%M") );
         //strTemp.Format(_T("%ld"),kd.m_time ); 
	   }
	   catch(...) // CException* e)
	   {
            // e->delete();
	   }
	}

	pDC->SetTextColor( clrPlane );
	//pDC->TextOut( x, y+18, strTemp );
	if ( x + 10 < m_rectReport.left )
	  pDC->TextOut( x+10, y, strTemp );

	// 价格小数位数格式串
	CString strPriceFmt;
	strPriceFmt.Format( _T("%%.%df"), info.DigitBit() );

	strTemp.Format( strPriceFmt, kd.m_fOpen );
	pDC->SetTextColor( kd.m_fOpen > fLastClose ? clrRise : (kd.m_fOpen<fLastClose?clrFall:clrPlane) );
	//pDC->TextOut( x, y+53, strTemp );
	if ( x + 115 < m_rectReport.left )
	  pDC->TextOut( x + 115 , y, strTemp );
	
	strTemp.Format( strPriceFmt, kd.m_fHigh );
	pDC->SetTextColor( kd.m_fHigh > fLastClose ? clrRise : (kd.m_fHigh<fLastClose?clrFall:clrPlane) );
	//pDC->TextOut( x, y+87, strTemp );
	if ( x + 225 < m_rectReport.left )
	  pDC->TextOut( x + 225 , y, strTemp );

	strTemp.Format( strPriceFmt, kd.m_fLow );
	pDC->SetTextColor( kd.m_fLow > fLastClose ? clrRise : (kd.m_fLow<fLastClose?clrFall:clrPlane) );
	//pDC->TextOut( x, y+121, strTemp );
	if ( x + 335 < m_rectReport.left )
      pDC->TextOut( x + 335 , y, strTemp );
	strTemp.Format( strPriceFmt, kd.m_fClose );
	pDC->SetTextColor( kd.m_fClose > fLastClose ? clrRise : (kd.m_fClose<fLastClose?clrFall:clrPlane) );
	//pDC->TextOut( x, y+155, strTemp );
	if ( x + 445 < m_rectReport.left )
      pDC->TextOut( x + 445 , y, strTemp );
	
	if ( !info.IsIndex() )
	{
	   strTemp.Format( _T("%.0f"), dVolume/100 );
	   pDC->SetTextColor( clrTitle );
	  //pDC->TextOut( x, y+189, strTemp );
	   if ( x + 570 < m_rectReport.left )
         pDC->TextOut( x + 570 , y, strTemp );
	   strTemp.Format( _T("%.0f"), dAmount/1000 );
	   pDC->SetTextColor( clrTitle );
     	//pDC->TextOut( x, y+223, strTemp );
	   if ( x + 690 < m_rectReport.left )
         pDC->TextOut( x + 690 , y, strTemp );

	   strTemp.Format( strPriceFmt, dPriceAverage );
	   pDC->SetTextColor( clrTitle );
	  //pDC->TextOut( x, y+257, strTemp );
	   if ( x + 780 < m_rectReport.left )
	     pDC->TextOut( x + 780 , y, strTemp );
    }
	else
	{
	   strTemp.Format( _T("%.0f"), dVolume/100 );
	   pDC->SetTextColor( clrTitle );
	  //pDC->TextOut( x, y+189, strTemp );
	   if ( x + 580 < m_rectReport.left )
         pDC->TextOut( x + 580 , y, strTemp );
	   strTemp.Format( _T("%.0f"), dAmount/1000 );
	   pDC->SetTextColor( clrTitle );
     	//pDC->TextOut( x, y+223, strTemp );
	   if ( x + 760 < m_rectReport.left )
         pDC->TextOut( x + 760 , y, strTemp );
	}

#if 0
	strTemp.Format( _T("%.2f"), dRelativeChangeHand );
	pDC->SetTextColor( clrTitle );
	pDC->TextOut( x, y+291, strTemp );
#endif

	pDC->SetTextAlign( nOldAlign );

	pDC->SelectObject( pOldFont );

#else
	CRect rect1;
	rect1.left = m_rectKLine.left + 40;
	rect1.top= m_rectAll.top + 2;
    rect1.right = m_rectKLine.right;
	rect1.bottom= rect1.top + m_nMarginTop - 5;
	
//m_nMarginTop;			// 上边边缘大小
	//int		m_nMarginBottom;	
	pDC->FillSolidRect( &rect1, clrBK );
   
	//pDC->Draw3dRect( &rect, clrBorder, clrBorder );
	//pDC->SetBkColor( clrBK );//clrPlane); // 

	//CRect	rectClient;
	//pDC->GetClientRect( &rectClient );
//超过客户显示区的范围,不显示

	CString	strTemp;
	//int	x	=	rect.left + 100;
	//int	y	=	rect.top;

    //int	x = rect1.left; // m_rectKLine.left +100;
	//int	y = rect1.top; // m_rectAll.top + 2;
   
	int	x	=  100;
	int	y	=  2;
	if ( info.IsIndex() ) x+=35;  // why ??

    //strTemp.Format(_T("x= %d,y=%d"),x,y);
	//AfxMessageBox( strTemp);
	//pDC->TextOut( x+240 , y,   strTemp );

 	CFont	* pOldFont	=	AfxSelectDCFont( pDC, 14 );
	pDC->SetBkColor( clrBK );
	
	pDC->SetTextColor( clrText );
	strTemp.LoadString( IDS_STOCKGRAPH_TIME );
   	//pDC->TextOut( x, y+2,   strTemp );
	if ( x + 100 < m_rectReport.left )
      pDC->TextOut( x , y,   strTemp );

	strTemp.LoadString( IDS_STOCKGRAPH_OPEN );
	//pDC->TextOut( x, y+37,  strTemp );
	if ( x + 210 < m_rectReport.left )
      pDC->TextOut( x + 110, y,   strTemp );
;
	strTemp.LoadString( IDS_STOCKGRAPH_HIGH );
	//pDC->TextOut( x, y+71,  strTemp );
	if ( x + 320 < m_rectReport.left )
       pDC->TextOut( x + 220, y,   strTemp );

	strTemp.LoadString( IDS_STOCKGRAPH_LOW );
	//pDC->TextOut( x, y+105, strTemp );
	if ( x + 430 < m_rectReport.left )
      pDC->TextOut( x + 330, y,   strTemp );

	strTemp.LoadString( IDS_STOCKGRAPH_CLOSE );
	//pDC->TextOut( x, y+139, strTemp );
	if ( x + 540 < m_rectReport.left )
      pDC->TextOut( x + 440, y,   strTemp );

	
	if ( !info.IsIndex() )
	{
       strTemp.LoadString( IDS_STOCKGRAPH_VOLUME );
	  //pDC->TextOut( x, y+173, strTemp );
	   if ( x + 650 < m_rectReport.left )
         pDC->TextOut( x + 550, y,   strTemp );

	   strTemp.LoadString( IDS_STOCKGRAPH_AMOUNT );
	  //pDC->TextOut( x, y+207, strTemp );
	   if ( x + 780 < m_rectReport.left )
         pDC->TextOut( x + 680, y,   strTemp );
    
	   strTemp.LoadString( IDS_STOCKGRAPH_PRICEAVERAGE );
	 // pDC->TextOut( x, y+241, strTemp );
	   if ( x + 890 < m_rectReport.left )
	     pDC->TextOut( x + 790, y,   strTemp );
	}
	else
	{
	   strTemp.LoadString( IDS_STOCKGRAPH_VOLUME );
	   //pDC->TextOut( x, y+173, strTemp );
	   if ( x + 650 < m_rectReport.left )
         pDC->TextOut( x + 550, y,   strTemp );

	   strTemp.LoadString( IDS_STOCKGRAPH_AMOUNT );
	//pDC->TextOut( x, y+207, strTemp );
	   if ( x + 820 < m_rectReport.left )
         pDC->TextOut( x + 720, y,   strTemp );
    
	}
#if 0

	strTemp.LoadString( IDS_STOCKGRAPH_RELATIVECHANGEHAND );
	pDC->TextOut( x, y+275, strTemp );
#endif

	//x	=	rect.right - 3;
	x += 90; 
	if ( info.IsIndex() ) x-=35;  // why ???
	UINT nOldAlign = pDC->SetTextAlign( TA_RIGHT | TA_TOP );
	CSPTime	sptime;
	if( CKData::IsDayOrMin( m_nCurKType ) )
	{
		sptime.FromStockTimeDay( kd.m_date );
		strTemp	=	sptime.Format( _T("%y.%m.%d") );
	}
	else  // 5min K线指标
	{ 
	  try
	  {
		 // 日期为啥不对？，接口需要将接收到数据进行转换ToStockTimeMin();
		 sptime.FromStockTimeMin(  /*kd.m_time*/kd.m_date );  
		 // CSPTime sptime1( kd.m_time );
		  strTemp	=	sptime.Format( _T("%m.%d %H:%M") );
         //strTemp.Format(_T("%ld"),kd.m_time ); 
	   }
	   catch(...) // CException* e)
	   {
            // e->delete();
	   }
	}

	pDC->SetTextColor( clrPlane );
	//pDC->TextOut( x, y+18, strTemp );
	if ( x + 10 < m_rectReport.left )
	  pDC->TextOut( x+10, y, strTemp );

	// 价格小数位数格式串
	CString strPriceFmt;
	strPriceFmt.Format( _T("%%.%df"), info.DigitBit() );

	strTemp.Format( strPriceFmt, kd.m_fOpen );
	pDC->SetTextColor( kd.m_fOpen > fLastClose ? clrRise : (kd.m_fOpen<fLastClose?clrFall:clrPlane) );
	//pDC->TextOut( x, y+53, strTemp );
	if ( x + 115 < m_rectReport.left )
	  pDC->TextOut( x + 115 , y, strTemp );
	
	strTemp.Format( strPriceFmt, kd.m_fHigh );
	pDC->SetTextColor( kd.m_fHigh > fLastClose ? clrRise : (kd.m_fHigh<fLastClose?clrFall:clrPlane) );
	//pDC->TextOut( x, y+87, strTemp );
	if ( x + 225 < m_rectReport.left )
	  pDC->TextOut( x + 225 , y, strTemp );

	strTemp.Format( strPriceFmt, kd.m_fLow );
	pDC->SetTextColor( kd.m_fLow > fLastClose ? clrRise : (kd.m_fLow<fLastClose?clrFall:clrPlane) );
	//pDC->TextOut( x, y+121, strTemp );
	if ( x + 335 < m_rectReport.left )
      pDC->TextOut( x + 335 , y, strTemp );
	strTemp.Format( strPriceFmt, kd.m_fClose );
	pDC->SetTextColor( kd.m_fClose > fLastClose ? clrRise : (kd.m_fClose<fLastClose?clrFall:clrPlane) );
	//pDC->TextOut( x, y+155, strTemp );
	if ( x + 445 < m_rectReport.left )
      pDC->TextOut( x + 445 , y, strTemp );
	
	if ( !info.IsIndex() )
	{
	   strTemp.Format( _T("%.0f"), dVolume/100 );
	   pDC->SetTextColor( clrTitle );
	  //pDC->TextOut( x, y+189, strTemp );
	   if ( x + 570 < m_rectReport.left )
         pDC->TextOut( x + 570 , y, strTemp );
	   strTemp.Format( _T("%.0f"), dAmount/1000 );
	   pDC->SetTextColor( clrTitle );
     	//pDC->TextOut( x, y+223, strTemp );
	   if ( x + 690 < m_rectReport.left )
         pDC->TextOut( x + 690 , y, strTemp );

	   strTemp.Format( strPriceFmt, dPriceAverage );
	   pDC->SetTextColor( clrTitle );
	  //pDC->TextOut( x, y+257, strTemp );
	   if ( x + 780 < m_rectReport.left )
	     pDC->TextOut( x + 780 , y, strTemp );
    }
	else
	{
	   strTemp.Format( _T("%.0f"), dVolume/100 );
	   pDC->SetTextColor( clrTitle );
	  //pDC->TextOut( x, y+189, strTemp );
	   if ( x + 580 < m_rectReport.left )
         pDC->TextOut( x + 580 , y, strTemp );
	   strTemp.Format( _T("%.0f"), dAmount/1000 );
	   pDC->SetTextColor( clrTitle );
     	//pDC->TextOut( x, y+223, strTemp );
	   if ( x + 760 < m_rectReport.left )
         pDC->TextOut( x + 760 , y, strTemp );
	}
#if 0
	strTemp.Format( _T("%.2f"), dRelativeChangeHand );
	pDC->SetTextColor( clrTitle );
	pDC->TextOut( x, y+291, strTemp );
#endif

	pDC->SetTextAlign( nOldAlign );

	pDC->SelectObject( pOldFont );
#endif
}

void CStockGraph::DrawTechLine(CDC * pDC )
{
	CSPDWordArray	& anShow	=	AfxGetProfile().GetGraphTechsShow( );
	for( int k=0; k<m_arcTech.GetSize(); k++ )
	{
		CRect	rect	=	m_arcTech.ElementAt(k);
		if( k < anShow.GetSize() )
		{
			DrawTechLine( pDC, anShow[k], rect, FALSE );
			DrawTechLine( pDC, anShow[k], rect, TRUE );
		}
	}
}

void CStockGraph::DrawKLineAttribute(CDC * pDC, UINT nTech, CRect rect, BOOL bDrawTitle, double dMin, double dMax )
{
	switch( nTech )
	{
	case STT_MA:		DrawTechMA( pDC, rect, bDrawTitle, dMin, dMax );		break;
	case STT_EXPMA:		DrawTechEXPMA( pDC, rect, bDrawTitle, dMin, dMax );		break;
	case STT_BBI:		DrawTechBBI( pDC, rect, bDrawTitle, dMin, dMax );		break;
	case STT_BOLL:		DrawTechBOLL( pDC, rect, bDrawTitle, dMin, dMax );		break;
	case STT_PV:		DrawTechPV( pDC, rect, bDrawTitle, dMin, dMax );		break;
	case STT_SAR:		DrawTechSAR( pDC, rect, bDrawTitle, dMin, dMax );		break;
	case STT_DJ:		DrawTechDJ( pDC, rect, bDrawTitle, dMin, dMax );		break;
	case STT_CW:		DrawTechCW( pDC, rect, bDrawTitle, dMin, dMax );		break;
	default:;
		ASSERT( FALSE );
	}
}

void CStockGraph::DrawTechLine(CDC * pDC, UINT nTech, CRect rect, BOOL bDrawTitle )
{
	DECLARE_COLOR_DEFINATION
   
	if( !bDrawTitle ) //显示技术指标的标题
	{
		pDC->FillSolidRect( &rect, clrBK );
		pDC->FillSolidRect( CRect(m_rectAll.left,rect.top,rect.left-1,rect.bottom), clrBK );
		pDC->Draw3dRect( &rect, clrBorder, clrBorder );
	}

	switch( nTech )
	{
	case STT_MACD:		DrawTechMACD( pDC, rect, bDrawTitle );		break;
	case STT_MIKE:		DrawTechMIKE( pDC, rect, bDrawTitle );		break;
	case STT_PSY:		DrawTechPSY( pDC, rect, bDrawTitle );		break;

	case STT_VOLUME:	DrawTechVOLUME( pDC, rect, bDrawTitle );	break;
	case STT_NVI:		DrawTechNVI( pDC, rect, bDrawTitle );		break;
	case STT_PVI:		DrawTechPVI( pDC, rect, bDrawTitle );		break;
	case STT_VR:		DrawTechVR( pDC, rect, bDrawTitle );		break;
	case STT_VROC:		DrawTechVROC( pDC, rect, bDrawTitle );		break;
	case STT_OBV:		DrawTechOBV( pDC, rect, bDrawTitle );		break;
	case STT_MOBV:		DrawTechMOBV( pDC, rect, bDrawTitle );		break;
	case STT_MFI:		DrawTechMFI( pDC, rect, bDrawTitle );		break;
	case STT_VMACD:		DrawTechVMACD( pDC, rect, bDrawTitle );		break;
	case STT_WVAD:		DrawTechWVAD( pDC, rect, bDrawTitle );		break;
	case STT_EMV:		DrawTechEMV( pDC, rect, bDrawTitle );		break;
	case STT_VRSI:		DrawTechVRSI( pDC, rect, bDrawTitle );		break;
	case STT_NVRSI:		DrawTechNVRSI( pDC, rect, bDrawTitle );		break;
	case STT_AD:		DrawTechAD( pDC, rect, bDrawTitle );		break;
	case STT_CI:		DrawTechCI( pDC, rect, bDrawTitle );		break;

	case STT_KDJ:		DrawTechKDJ( pDC, rect, bDrawTitle );		break;
	case STT_R:			DrawTechR( pDC, rect, bDrawTitle );			break;
	case STT_RSI:		DrawTechRSI( pDC, rect, bDrawTitle );		break;
	case STT_BIAS:		DrawTechBIAS( pDC, rect, bDrawTitle );		break;
	case STT_MTM:		DrawTechMTM( pDC, rect, bDrawTitle );		break;
	case STT_DMI:		DrawTechDMI( pDC, rect, bDrawTitle );		break;
	case STT_ROC:		DrawTechROC( pDC, rect, bDrawTitle );		break;
	case STT_CCI:		DrawTechCCI( pDC, rect, bDrawTitle );		break;
	case STT_CV:		DrawTechCV( pDC, rect, bDrawTitle );		break;
	case STT_ARBR:		DrawTechARBR( pDC, rect, bDrawTitle );		break;
	case STT_CR:		DrawTechCR( pDC, rect, bDrawTitle );		break;
	case STT_OSC:		DrawTechOSC( pDC, rect, bDrawTitle );		break;
	case STT_UOS:		DrawTechUOS( pDC, rect, bDrawTitle );		break;
	case STT_MAOSC:		DrawTechMAOSC( pDC, rect, bDrawTitle );		break;
	case STT_36BIAS:	DrawTech3_6BIAS( pDC, rect, bDrawTitle );	break;
	case STT_DPO:		DrawTechDPO( pDC, rect, bDrawTitle );		break;
	case STT_KST:		DrawTechKST( pDC, rect, bDrawTitle );		break;

	case STT_SKDJ:		DrawTechSKDJ( pDC, rect, bDrawTitle );		break;

	case STT_REI:		DrawTechREI( pDC, rect, bDrawTitle );		break;
	case STT_DMKI:		DrawTechDMKI( pDC, rect, bDrawTitle );		break;
	case STT_PCNT:		DrawTechPCNT( pDC, rect, bDrawTitle );		break;
	case STT_HLC:		DrawTechHLC( pDC, rect, bDrawTitle );		break;
	case STT_CDP:		DrawTechCDP( pDC, rect, bDrawTitle );		break;
	case STT_ASI:		DrawTechASI( pDC, rect, bDrawTitle );		break;
	case STT_ATR:		DrawTechATR( pDC, rect, bDrawTitle );		break;
//#ifdef _ACTIVE_STT_USER
	case STT_CYO:		DrawTechCYO( pDC, rect, bDrawTitle );		break;
	case STT_DCYO:		DrawTechDCYO( pDC, rect, bDrawTitle );		break;
	case STT_HSL:		DrawTechHSL( pDC, rect, bDrawTitle );		break;
	case STT_DPER:		DrawTechDPER( pDC, rect, bDrawTitle );		break;
//#endif
	default:
//#ifdef _ACTIVE_STT_USER
		{
			UINT nTechUserCount = CTechUser::GetTechUserCount();
			if( nTech >= STT_USER_MIN && nTech <= STT_USER_MIN+nTechUserCount-1 )
				DrawTechUser( nTech, pDC, rect, bDrawTitle );
			//else
			//	ASSERT( FALSE );
		}
//#endif
		break;
	}
}

int CStockGraph::DrawBuySell( CDC * pDC,CStockInfo & info, int xStart, int yStart, int nWidth )
{
	DECLARE_COLOR_DEFINATION

//	CStockInfo & info	=	m_CurStock.GetStockInfo();
	CString	strTemp, strTempFmt;

	// Draw Name And Code
	//CFont	* pOldFont	=	AfxSelectDCFont( pDC, 18 );
	//pDC->SetTextColor( clrTitle );
	//pDC->TextOutW( xStart+1, m_rectAll.top+3, info.GetStockName() );
	//pDC->SetTextColor( clrText );
	//pDC->TextOutW( xStart+96, m_rectAll.top+3, info.GetStockCode() );

	// Select Report Font
	CFont	* pOldFont	=AfxSelectDCFont( pDC, 14 );
	
	// Draw Report Names
	int	x	=	xStart+2;
	int	x2	=	xStart+2+nWidth/2;
	int	y	=	yStart;
	
	int difY =  10;

	pDC->SetTextColor( clrText );
    int	nRetValue =0 ;
	UINT nOldAlign;

    // 价格小数位数格式串
	CString strPriceFmt;
	strPriceFmt.Format( _T("%%.%df"), info.DigitBit() );

    if( info.IsIndex() )
	{

		CStockInfo	infoX;

		CStockInfo	info1;
		CStockInfo	info2;
        double	dDiff1 = 0;
		double	dDiff2 = 0;
		double	dDiff =  0;
		
		double	dfClose1 = 0;
		double	dfClose2 = 0;
		double	dfClose = info.m_fClose;

		double	dfLast1 = 0;
		double	dfLast2 = 0;
		double	dfLast= info.m_fLast;

 		DWORD	dwMarket = info.GetMarket();

        if ( _tcsncmp(info.GetMarketStockCode(),_T("1A0001SH"),8)== 0|| 
			_tcsncmp(info.GetMarketStockCode(),_T("999999SH"),8)== 0)
		{
            //dDiff = info.m_fClose - info.m_fLast;
			infoX = info;
      
            if ( /*AfxGetStockContainer().*/AfxGetProfile().m_pStockContainer->GetStockInfo(_T("1A0002SH"), &info1)) //?
		    {
               dfClose1 = info1.m_fClose;
			   dfLast1= info1.m_fLast;
			   info1.GetDiff(&dDiff1,info1.m_datetech,1);
		    }
		    if ( /*AfxGetStockContainer().*/AfxGetProfile().m_pStockContainer->GetStockInfo(_T("1A0003SH"), &info2)) //?
		    {
			   dfClose2 = info2.m_fClose;
			   dfLast2= info.m_fLast;
               info2.GetDiff(&dDiff2,info2.m_datetech,1);
		    }
		}
		else if ( _tcsncmp(info.GetStockCode(),_T("1A0"),3)== 0 ||
			_tcsncmp(info.GetStockCode(),_T("999999"),6)== 0 ||
			_tcsncmp(info.GetStockCode(),_T("1B0"),3)== 0 ||
			_tcsncmp(info.GetStockCode(),_T("1C0"),3)== 0 )
		{
			// dfClose1 = info.m_fClose;
			 //dfLast1= info.m_fLast;
			 //info.GetDiff(&dDiff1,info.m_datetech,1);
            // dwAdvance =info.m_dwAdvance;
		    // dwDecline =info.m_dwDecline;
			  /*AfxGetStockContainer().*/AfxGetProfile().m_pStockContainer->GetStockInfo(_T("1A0001SH"), &infoX); //?
   
            if ( /*AfxGetStockContainer().*/AfxGetProfile().m_pStockContainer->GetStockInfo(_T("1A0002SH"), &info1)) //?
		    {
               dfClose1 = info1.m_fClose;
			   dfLast1= info1.m_fLast;
			   info1.GetDiff(&dDiff1,info1.m_datetech,1);
		    }

            if ( /*AfxGetStockContainer().*/AfxGetProfile().m_pStockContainer->GetStockInfo(_T("1A0003SH"), &info2)) //?
		    {
			   dfClose2 = info2.m_fClose;
			   dfLast2= info2.m_fLast;
               info2.GetDiff(&dDiff2,info2.m_datetech,1);
		    }  
		}
/*		else if ( _tcsncmp(info.GetMarketStockCode(),_T("1A0003SH"),8)== 0)
		{
			dfClose2 = info.m_fClose;
			dfLast2  = info.m_fLast;
            info.GetDiff(&dDiff2,info.m_datetech,1);
           

			AfxGetStockContainer().GetStockInfo(_T("1A0001SH"), &infoX);
            dwAdvance =infoX.m_dwAdvance;
		    dwDecline =infoX.m_dwDecline;

            if (AfxGetStockContainer().GetStockInfo(_T("1A0002SH"), &info1))
		    {
               dfClose1 = info1.m_fClose;
			   dfLast1= info1.m_fLast;
			   info1.GetDiff(&dDiff1,info1.m_datetech,1);
		    }
		}
*/
        else if ( _tcsncmp(info.GetMarketStockCode(),_T("399001SZ"),8)== 0) 
		{
            //dDiff = info.m_fClose - info.m_fLast;
			infoX = info;
    
            if ( /*AfxGetStockContainer().*/AfxGetProfile().m_pStockContainer->GetStockInfo(_T("399002SZ"), &info1)) //?
		    {
               dfClose1 = info1.m_fClose;
			   dfLast1= info1.m_fLast;
			   info1.GetDiff(&dDiff1,info1.m_datetech,1);
		    }
		    if ( /*AfxGetStockContainer().*/AfxGetProfile().m_pStockContainer->GetStockInfo(_T("399003SZ"), &info2)) //?
		    {
			   dfClose2 = info2.m_fClose;
			   dfLast2= info.m_fLast;
               info2.GetDiff(&dDiff2,info2.m_datetech,1);
		    }
		}
		else if ( _tcsncmp(info.GetStockCode(),_T("399"),3)== 0)
		{
			// dfClose1 = info.m_fClose;
			// dfLast1= info.m_fLast;
			// info.GetDiff(&dDiff1,info.m_datetech,1);
            // dwAdvance =info.m_dwAdvance;
		    // dwDecline =info.m_dwDecline;
			  /*AfxGetStockContainer().*/AfxGetProfile().m_pStockContainer->GetStockInfo(_T("399001SZ"), &infoX); //?
 
			if ( /*AfxGetStockContainer().*/AfxGetProfile().m_pStockContainer->GetStockInfo(_T("399002SZ"), &info1)) //?
		    {
               dfClose1 = info1.m_fClose;
			   dfLast1= info1.m_fLast;
			   info1.GetDiff(&dDiff1,info1.m_datetech,1);
		    }

            if ( /*AfxGetStockContainer().*/AfxGetProfile().m_pStockContainer->GetStockInfo(_T("399003SZ"), &info2)) //?
		    {
			   dfClose2 = info2.m_fClose;
			   dfLast2= info2.m_fLast;
               info2.GetDiff(&dDiff2,info2.m_datetech,1);
		    }  
		}
	/*	else if ( _tcsncmp(info.GetMarketStockCode(),_T("399003SZ"),8)== 0)
		{
			dfClose2 = info.m_fClose;
			dfLast2  = info.m_fLast;
            info.GetDiff(&dDiff2,info.m_datetech,1);
           

			AfxGetStockContainer().GetStockInfo(_T("399001SZ"), &infoX);
            dwAdvance =infoX.m_dwAdvance;
		    dwDecline =infoX.m_dwDecline;

            if (AfxGetStockContainer().GetStockInfo(_T("399002SZ"), &info1))
		    {
               dfClose1 = info1.m_fClose;
			   dfLast1= info1.m_fLast;
			   info1.GetDiff(&dDiff1,info1.m_datetech,1);
		    }
		}*/

        if ( dwMarket == CStock::marketSHSE)
		{
          strTemp.LoadString(IDS_REALTIME_INDEX); //       "上证指数"
		}
		else
		{
           strTemp.LoadString(IDS_READTIME_SZINDEX);
		}
		pDC->TextOut( x, y+10,   strTemp );
		strTemp.LoadString(IDS_READTIME_INDEXDIFF); //   "涨跌"
        pDC->TextOut( x, y+29, strTemp );   
		DrawLine( pDC, 1, clrBorder, xStart, y+48, xStart+nWidth, y+48 );

		if ( dwMarket == CStock::marketSHSE )
		{
		  strTemp.LoadString(IDS_READTIME_SHINDEXA); //    "上证Ａ股"
		}
		else
		{
           strTemp.LoadString(IDS_READTIME_SZINDEXA);
		}
		pDC->TextOut( x, y+52, strTemp );   
		strTemp.LoadString(IDS_READTIME_INDEXDIFF); //   "涨跌"
        pDC->TextOut( x, y+71, strTemp );
        DrawLine( pDC, 1, clrBorder, xStart, y+90, xStart+nWidth, y+90 );
        if ( dwMarket == CStock::marketSHSE )
		{
          strTemp.LoadString(IDS_READTIME_SHINDEXB); //    "上证Ｂ股"
		}
		else
		{
          strTemp.LoadString(IDS_READTIME_SZINDEXB);
		}
        pDC->TextOut( x, y+94, strTemp );
        strTemp.LoadString(IDS_READTIME_SHBINDEXDIFF); //  "涨跌"
        pDC->TextOut( x, y+113, strTemp );
        DrawLine( pDC, 1, clrBorder, xStart, y+132, xStart+nWidth, y+132 );

        strTemp.LoadString(IDS_READTIME_INDEXAMOUNT); //  "成交总额"
        pDC->TextOut( x, y+136, strTemp );
		strTemp.LoadString(IDS_READTIME_INDEXVOLUME); //  "成交手数"
        pDC->TextOut( x, y+154, strTemp );
        DrawLine( pDC, 1, clrBorder, xStart, y+173, xStart+nWidth, y+173 );

        strTemp.LoadString(IDS_READTIME_INDEXBUY); //    "委买手数"
        pDC->TextOut( x, y+177, strTemp );
		strTemp.LoadString(IDS_READTIME_INDEXSELL); //   "委卖手数"
        pDC->TextOut( x, y+196, strTemp );
		strTemp.LoadString( IDS_REALTIME_BSRATIO );	// "委比";
        pDC->TextOut( x, y+215, strTemp );
        DrawLine( pDC, 1, clrBorder, xStart, y+234, xStart+nWidth, y+234 );

        if ( dwMarket == CStock::marketSHSE )
		{ 
		  strTemp.LoadString(IDS_READTIME_SHINDEXOPEN); //  "上证开盘"
		}
		else
		{
			strTemp.LoadString(IDS_READTIME_SZINDEXOPEN);
		}
        pDC->TextOut( x, y+238, strTemp );
		if ( dwMarket == CStock::marketSHSE )
		{
		  strTemp.LoadString(IDS_READTIME_SHINDEXHIGH); //  "上证最高"
		}
		else
		{
		  strTemp.LoadString(IDS_READTIME_SZINDEXHIGH); 
		}

        pDC->TextOut( x, y+257, strTemp );
		if ( dwMarket == CStock::marketSHSE)
		{
		  strTemp.LoadString(IDS_READTIME_SHINDEXLOW); //  "上证最低"
		}
		else
		{
		  strTemp.LoadString(IDS_READTIME_SZINDEXLOW);
		}
        pDC->TextOut( x, y+275, strTemp );
        DrawLine( pDC, 1, clrBorder, xStart, y+294, xStart+nWidth, y+294 );


		strTemp.LoadString(IDS_READTIME_INDEXADVANCE); //  "上涨家数"
        pDC->TextOut( x, y+298, strTemp );
		strTemp.LoadString(IDS_READTIME_INDEXBAL); //    "平盘家数"
        pDC->TextOut( x, y+317, strTemp );
		strTemp.LoadString(IDS_READTIME_INDEXDECLINE); //  "下跌家数"
		pDC->TextOut( x, y+336, strTemp );

        strTemp.LoadString( IDS_PREINDEX_A7 );
        pDC->TextOut( x,y+336+19, strTemp );
	    strTemp.LoadString( IDS_PREINDEX_A8 );
	    pDC->TextOut( x, y+336+38, strTemp );
	    strTemp.LoadString( IDS_PREINDEX_A9 );
	    pDC->TextOut( x, y+336+57, strTemp );
/*
    IDS_READTIME_SZINDEX    "深证成指"
    IDS_READTIME_SZINDEXA   "成份Ａ指"
    IDS_READTIME_SZINDEXB   "成份Ｂ指"
    IDS_READTIME_SZINDEXOPEN "深证开盘"
    IDS_READTIME_SZINDEXHIGH "深证最高"
    IDS_READTIME_SZINDEXLOW "深证最低"
*/
    // Draw Report Values
	    x	=	xStart-1+nWidth/2;
	    x2	=	xStart+nWidth-1;
	    UINT nOldAlign = pDC->SetTextAlign( TA_RIGHT | TA_TOP );
	    pDC->SetTextColor( clrPlane );
	  
		strTemp = _T("--");
		pDC->SetTextColor( RT_DETERMINE_COLOR(infoX.m_fClose, infoX.m_fLast) );
		strTemp.Format( strPriceFmt,infoX.m_fClose);	
	    pDC->TextOut( x2, y+10,   strTemp );
		//   "涨跌"
		strTemp.Format( _T("%.2f"),infoX.m_fClose-infoX.m_fLast);
        pDC->TextOut( x2, y+29, strTemp );   
	
		 //    "上证Ａ股"
        pDC->SetTextColor( RT_DETERMINE_COLOR(info1.m_fClose, info1.m_fLast) );
		strTemp.Format( strPriceFmt,dfClose1);	
		pDC->TextOut( x2, y+52, strTemp );   
		 //   "涨跌"
		strTemp = _T("--");
		strTemp.Format( _T("%.2f"), dDiff1 );
        pDC->TextOut( x2, y+71, strTemp );
        
        //    "上证Ｂ股"
		strTemp	=	_T("-");
		pDC->SetTextColor( RT_DETERMINE_COLOR(info2.m_fClose, info2.m_fLast) );
		strTemp.Format( strPriceFmt,dfClose2);	
        pDC->TextOut( x2, y+94, strTemp );
        //  "涨跌"
		strTemp.Format( _T("%.2f"), dDiff2 );
        pDC->TextOut( x2, y+113, strTemp );
        
        //  "成交总额"
		pDC->SetTextColor( clrPlane );
		strTemp.Format( strPriceFmt, infoX.m_fAmount );
        pDC->TextOut( x2, y+136, strTemp );
		//  "成交手数"
		strTemp.Format( _T("%.0f"), infoX.m_fVolume/100.);
        pDC->TextOut( x2, y+154, strTemp );
        
        //    "委买手数"
		pDC->SetTextColor( clrRise );
		strTemp.Format( _T("%.0f"), infoX.m_fBuyVolume[0] );
        pDC->TextOut( x2, y+177, strTemp );
		//   "委卖手数"
		pDC->SetTextColor( clrFall );
		strTemp.Format( _T("%.0f"), infoX.m_fSellVolume[0]);
        pDC->TextOut( x2, y+196, strTemp );
		// "委比";
		//strTemp	=	_T("-");
		// 委比和委差
		double	dVolume, dRatio = 0; //, dDiff = 0;
		dDiff = infoX.m_fBuyVolume[0] - infoX.m_fSellVolume[0];
		dVolume = infoX.m_fBuyVolume[0] + infoX.m_fSellVolume[0];
		if ( dVolume !=0 )
		{
          dRatio = 100 * dDiff / dVolume ;
		}
		
        if (  dDiff  > 1e-4 )
		{	pDC->SetTextColor( clrRise );}
		else if ( dDiff  < 0 )
		{	pDC->SetTextColor( clrFall );}
		else
		{
          	pDC->SetTextColor( clrPlane );
		}
        strTemp.Format( _T("%.0f  %.2f%%"), dDiff,dRatio);
        pDC->TextOut( x2, y+215, strTemp );
        

		 //  "上证开盘"
		pDC->SetTextColor( RT_DETERMINE_COLOR(infoX.m_fOpen,infoX.m_fLast) );
	    strTemp.Format( strPriceFmt, infoX.m_fOpen );
        pDC->TextOut( x2, y+238, strTemp );
		 //  "上证最高"
		pDC->SetTextColor( RT_DETERMINE_COLOR(infoX.m_fHigh,infoX.m_fLast) );
	    strTemp.Format( strPriceFmt, infoX.m_fHigh );
        pDC->TextOut( x2, y+257, strTemp );
		//  "上证最低"
		pDC->SetTextColor( RT_DETERMINE_COLOR(infoX.m_fLow,infoX.m_fLast) );
	    strTemp.Format( strPriceFmt, infoX.m_fLow);
        pDC->TextOut( x2, y+275, strTemp );
        

		 //  "上涨家数"
	    pDC->SetTextColor( clrRise );
		strTemp	=	_T("-");
		if( infoX.m_dwAdvance > 0 )
			strTemp.Format( _T("%u"), infoX.m_dwAdvance );		
	    pDC->TextOut( x2, y+298, strTemp );
		 //    "平盘家数"
	    strTemp	=	_T("-");
		pDC->SetTextColor( clrPlane );
        pDC->TextOut( x2, y+317, strTemp );
		//  "下跌家数"
		pDC->SetTextColor( clrFall );
		strTemp	=	_T("-");
		if( infoX.m_dwDecline > 0 )
			strTemp.Format( _T("%u"), infoX.m_dwDecline );
		pDC->TextOut( x2, y+336, strTemp );


	}
	else
	{

	// 委比和委差
	strTemp.LoadString( IDS_REALTIME_BSRATIO );	// "委比";
	pDC->TextOut( x, y+2,   strTemp );
	strTemp.LoadString( IDS_REALTIME_BSDIFF );	// "委差";
	pDC->TextOut( x2, y+2,   strTemp );

	// 委卖数
	DrawLine( pDC, 1, clrBorder, xStart, y+19, xStart+nWidth, y+19 );
	strTemp.LoadString( IDS_REALTIME_SELL5 );	// "卖⑤";
	pDC->TextOut( x, y+21, strTemp );
	strTemp.LoadString( IDS_REALTIME_SELL4 );	// "卖④";
	pDC->TextOut( x, y+37-1, strTemp );
	strTemp.LoadString( IDS_REALTIME_SELL3 );	// "卖③";
	pDC->TextOut( x, y+53-2, strTemp );
	strTemp.LoadString( IDS_REALTIME_SELL2 );	// "卖②";
	pDC->TextOut( x, y+69-3, strTemp );
	strTemp.LoadString( IDS_REALTIME_SELL1 );	// "卖①";
	pDC->TextOut( x, y+85-4, strTemp );

	// 委买数
	DrawLine( pDC, 1, clrBorder, xStart, y+ 97 /*102 */, xStart+nWidth, y+ 97/*102*/ );
	strTemp.LoadString( IDS_REALTIME_BUY1 );	// "买①";
	pDC->TextOut( x, y + 101-2 /*104*/, strTemp );
	strTemp.LoadString( IDS_REALTIME_BUY2 );	// "买②";
	pDC->TextOut( x, y +117-3/*120*/, strTemp );
	strTemp.LoadString( IDS_REALTIME_BUY3 );	// "买③";
	pDC->TextOut( x, y + 133-4 /*136*/, strTemp );
	strTemp.LoadString( IDS_REALTIME_BUY4 );	// "买④";
	pDC->TextOut( x, y + 149-5 /*152*/, strTemp );
	strTemp.LoadString( IDS_REALTIME_BUY5 );	// "买⑤";
	pDC->TextOut( x, y + 165-6 /*168 */, strTemp );

	// 其他信息报告
	DrawLine( pDC, 1, clrBorder, xStart, y+185 - difY, xStart+nWidth, y+185 - difY );
	strTemp.LoadString( IDS_REALTIME_PRICENOW );	// "成交";
	pDC->TextOut( x, y+187 - difY, strTemp );
	strTemp.LoadString( IDS_REALTIME_PRICEAVE );	// "均价";
	pDC->TextOut( x2, y+187 - difY, strTemp );
	strTemp.LoadString( IDS_REALTIME_PRICEDIFF );	// "涨跌";
	pDC->TextOut( x, y+203 - difY, strTemp );
	strTemp.LoadString( IDS_REALTIME_PRICEOPEN );	// "开盘";
	pDC->TextOut( x2, y+203 - difY, strTemp );
	strTemp.LoadString( IDS_REALTIME_PRICEDIFFPERCENT );	// "幅度";
	pDC->TextOut( x, y+219 - difY, strTemp );
	strTemp.LoadString( IDS_REALTIME_PRICEHIGH );	// "最高";
	pDC->TextOut( x2, y+219 - difY, strTemp );
	strTemp.LoadString( IDS_REALTIME_VOLSUM );		// "总手";
	pDC->TextOut( x, y+235 - difY, strTemp );
	strTemp.LoadString( IDS_REALTIME_PRICELOW );	// "最低";
	pDC->TextOut( x2, y+235 - difY, strTemp );
	strTemp.LoadString( IDS_REALTIME_VOLNOW );		// "现手";
	pDC->TextOut( x, y+251 - difY, strTemp );
	strTemp.LoadString( IDS_REALTIME_VOLRATION );	// "量比";
	pDC->TextOut( x2, y+251 - difY, strTemp );

	// 内外盘
	DrawLine( pDC, 1, clrBorder, xStart, y+268 - difY, xStart+nWidth, y+268  - difY);
	strTemp.LoadString( IDS_REALTIME_VOLOUTER );	// "外盘";
	pDC->TextOut( x, y+270 - difY, strTemp );
	strTemp.LoadString( IDS_REALTIME_VOLINNER );	// "内盘";
	pDC->TextOut( x2, y+270 - difY, strTemp );

	DrawLine( pDC, 1, clrBorder, xStart, y+287 - difY, xStart+nWidth, y+287 - difY );	

	// 涨家数，跌家数
/*	if( info.IsIndex() )
	{
		strTemp.LoadString( IDS_REALTIME_ADVANCE );	// "涨家数";
		pDC->TextOut( x, y+289 - difY, strTemp );
		strTemp.LoadString( IDS_REALTIME_DECLINE );	// "跌家数";
		pDC->TextOut( x2, y+289 - difY, strTemp );
		DrawLine( pDC, 1, clrBorder, xStart, y+306 - difY, xStart+nWidth, y+306 - difY );
	} */
	

	// Draw Report Values
	x	=	xStart-1+nWidth/2;
	x2	=	xStart+nWidth-1;
    nOldAlign = pDC->SetTextAlign( TA_RIGHT | TA_TOP );
	pDC->SetTextColor( clrPlane );
	

	// 价格小数位数格式串
	//CString strPriceFmt;
	//strPriceFmt.Format( _T("%%.%df"), info.DigitBit() );

	/*if( info.IsIndex() )
	{
		// 委比和委差
		double	dRatio = 0, dDiff = 0;
		strTemp.Format( _T("%.2f%%"), dRatio );
		pDC->TextOut( x, y+2,  dRatio!=0?strTemp:_T("-") );
		strTemp.Format( _T("%.0f"), dDiff*0.01 );
		pDC->TextOut( x2, y+2, dDiff!=0?strTemp:_T("-") );
        strTemp = _T("-"); 
		// 委卖数
		pDC->SetTextColor( clrText );
		pDC->TextOut( x, y+21, strTemp );
		pDC->TextOut( x2, y+21, strTemp );

		pDC->TextOut( x, y+37-1, strTemp );
		pDC->TextOut( x2, y+37-1, strTemp );

		pDC->TextOut( x, y+53-2, strTemp );
		pDC->TextOut( x2, y+53-2, strTemp );

		pDC->TextOut( x, y+69-3, strTemp );
		pDC->TextOut( x2, y+69-3, strTemp );

		pDC->TextOut( x, y+85-4, strTemp );
		pDC->TextOut( x2, y+85-4, strTemp );

		// 委买数
		pDC->TextOut( x, y+101-2, strTemp );
		pDC->TextOut( x2, y+101-2, strTemp );

		pDC->TextOut( x, y+117-3, strTemp );
		pDC->TextOut( x2, y+117-3, strTemp );
		
		pDC->TextOut( x, y+133-4, strTemp );
		pDC->TextOut( x2, y+133-4, strTemp );

		pDC->TextOut( x, y+149-5, strTemp );
		pDC->TextOut( x2, y+149-5, strTemp );

		pDC->TextOut( x, y+165-6, strTemp );
		pDC->TextOut( x2,y+165-6, strTemp );
	}
	else 
	{ */
		// 委比和委差
		double	dRatio = 0, dDiff = 0;
		if( info.GetSellBuyRatio( &dRatio, &dDiff ) )
		{
			strTemp.Format( _T("%.2f%%"), dRatio );
			pDC->TextOut( x, y+2,   dRatio!=0?strTemp:_T("-") );
			strTemp.Format( _T("%.0f"), dDiff*0.01 );
			pDC->TextOut( x2, y+2,  dDiff!=0? strTemp:_T("-") );
		}

		// 委卖数
 
		pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fSellPrice[4],info.m_fLast) );
		strTemp.Format( strPriceFmt, info.m_fSellPrice[4] );
		pDC->TextOut( x, y+21,  info.m_fSellPrice[4]>1e-4?strTemp:_T("-") );
		strTemp.Format( _T("%.0f"),  info.m_fSellVolume[4]/100 );
		//strTemp.Format( _T("%.0f"), info.IsBond()?info.m_fSellVolume[4]: info.m_fSellVolume[4]/100 );
		pDC->TextOut( x2, y+21, info.m_fSellVolume[4]>1e-4?strTemp:_T("-") );

		pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fSellPrice[3],info.m_fLast) );
		strTemp.Format( strPriceFmt, info.m_fSellPrice[3] );
		pDC->TextOut( x, y+ 37-1, info.m_fSellPrice[3]>1e-4?strTemp:_T("-") );
		strTemp.Format( _T("%.0f"),  info.m_fSellVolume[3]/100 );
		//strTemp.Format( _T("%.0f"), info.IsBond()?info.m_fSellVolume[3]: info.m_fSellVolume[3]/100 );
		pDC->TextOut( x2, y+ 37-1, info.m_fSellVolume[3]>1e-4?strTemp:_T("-") );
	
		pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fSellPrice[2],info.m_fLast) );
		strTemp.Format( strPriceFmt, info.m_fSellPrice[2] );
		pDC->TextOut( x, y + 53-2, info.m_fSellPrice[2]>1e-4?strTemp:_T("-") );
		strTemp.Format( _T("%.0f"), info.m_fSellVolume[2]/100 );
		//strTemp.Format( _T("%.0f"),info.IsBond()?info.m_fSellVolume[2]:  info.m_fSellVolume[2]/100 );
		pDC->TextOut( x2, y + 53-2, info.m_fSellVolume[2]>1e-4?strTemp:_T("-") );

		pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fSellPrice[1],info.m_fLast) );
		strTemp.Format( strPriceFmt, info.m_fSellPrice[1] );
		pDC->TextOut( x, y + 69-3, info.m_fSellPrice[1]>1e-4?strTemp:_T("-") );
		strTemp.Format( _T("%.0f"),  info.m_fSellVolume[1]/100 );
		//strTemp.Format( _T("%.0f"), info.IsBond()?info.m_fSellVolume[1]: info.m_fSellVolume[1]/100 );
		pDC->TextOut( x2, y + 69-3, info.m_fSellVolume[1]>1e-4?strTemp:_T("-") );

		pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fSellPrice[0],info.m_fLast) );
		strTemp.Format( strPriceFmt, info.m_fSellPrice[0] );
		pDC->TextOut( x, y + 85-4, info.m_fSellPrice[0]>1e-4?strTemp:_T("-") );
		strTemp.Format( _T("%.0f"), info.m_fSellVolume[0]/100 );
		//strTemp.Format( _T("%.0f"),info.IsBond()?info.m_fSellVolume[0]: info.m_fSellVolume[0]/100 );
		pDC->TextOut( x2, y + 85-4, info.m_fSellVolume[0]>1e-4?strTemp:_T("-"));

		// 委买数
		pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fBuyPrice[0],info.m_fLast) );
		strTemp.Format( strPriceFmt, info.m_fBuyPrice[0] );
		pDC->TextOut( x, y + 101-2, info.m_fBuyPrice[0]>1e-4?strTemp:_T("-") );
		//if ( info.IsBond() )
		//{
		//	strTemp.Format( _T("%.0f"), info.m_fBuyVolume[0] );
		//}
		//else
		//{
		 strTemp.Format( _T("%.0f"), info.m_fBuyVolume[0]/100 );
	    //}
		pDC->TextOut( x2, y + 101-2 /*104*/,info.m_fBuyVolume[0]>1e-4?strTemp:_T("-") );

		pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fBuyPrice[1],info.m_fLast) );
		strTemp.Format( strPriceFmt, info.m_fBuyPrice[1] );
		pDC->TextOut( x, y + 117-3/*120*/, info.m_fBuyPrice[1]>1e-4?strTemp:_T("-"));
	    //if ( info.IsBond() )
		//{
		//	strTemp.Format( _T("%.0f"), info.m_fBuyVolume[1] );
		//}
		//else
		{
		  strTemp.Format( _T("%.0f"), info.m_fBuyVolume[1]/100 );
	    }
		pDC->TextOut( x2, y +117-3 /*120 */,info.m_fBuyVolume[1]>1e-4?strTemp:_T("-"));
		
		pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fBuyPrice[2],info.m_fLast) );
		strTemp.Format( strPriceFmt, info.m_fBuyPrice[2] );
		pDC->TextOut( x, y + 133-4 /*136 */,info.m_fBuyPrice[2]>1e-4?strTemp:_T("-") );
		//if ( info.IsBond() )
		//{
		//	strTemp.Format( _T("%.0f"), info.m_fBuyVolume[2] );
		//}
		//else
		{
		  strTemp.Format( _T("%.0f"), info.m_fBuyVolume[2]/100 );
		}
		pDC->TextOut( x2, y + 133-4 /*136 */,info.m_fBuyVolume[2]>1e-4?strTemp:_T("-"));
    
		pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fBuyPrice[3],info.m_fLast) );
		strTemp.Format( strPriceFmt, info.m_fBuyPrice[3] );
		pDC->TextOut( x, y+ 149-5 /*152*/, info.m_fBuyPrice[3]>1e-4?strTemp:_T("-") );
		//if ( info.IsBond() )
		//{
		//	strTemp.Format( _T("%.0f"), info.m_fBuyVolume[3] );
		//}
		//else
		{
		   strTemp.Format( _T("%.0f"), info.m_fBuyVolume[3]/100 );
		}
		pDC->TextOut( x2, y+149-5 /*152*/,info.m_fBuyVolume[3]>1e-4?strTemp:_T("-") );

		pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fBuyPrice[4],info.m_fLast) );
		strTemp.Format( strPriceFmt, info.m_fBuyPrice[4] );
		pDC->TextOut( x, y+165-6, info.m_fBuyPrice[4]>1e-4?strTemp:_T("-") );
		//if ( info.IsBond() )
		//{
		//	strTemp.Format( _T("%.0f"), info.m_fBuyVolume[4] );
		//}
		//else
		{
			strTemp.Format( _T("%.0f"), info.m_fBuyVolume[4]/100 );
		}
		pDC->TextOut( x2, y+165-6,info.m_fBuyVolume[4]>1e-4?strTemp:_T("-") );
	  
	//}
    
	// 其他信息报告
	// 成交
	pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fClose,info.m_fLast) );
	strTemp.Format( strPriceFmt, info.m_fClose );
	pDC->TextOut( x, y+187 - difY, info.m_fClose >1e-4?strTemp:_T("-") );
	
	// 均价
	pDC->SetTextColor( clrPlane );
	strTemp	=	_T("-");
	if( info.m_fVolume > 0 )
	{
		double dAve	=	info.m_fAmount/info.m_fVolume;
		if ( info.GetAverage( &dAve ) ) // 2008.02.27
		{
		  pDC->SetTextColor( RT_DETERMINE_COLOR(dAve,info.m_fLast) );
		  strTemp.Format( strPriceFmt, dAve );
		}
	}
	pDC->TextOut( x2, y+187 - difY, strTemp );
	
	// 涨跌
	pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fClose,info.m_fLast) );
	strTemp.Format( strPriceFmt, info.m_fClose-info.m_fLast );
	pDC->TextOut( x, y+203 - difY, strTemp );

	// 开盘
	pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fOpen,info.m_fLast) );
	strTemp.Format( strPriceFmt, info.m_fOpen );
	pDC->TextOut( x2, y+203 - difY, info.m_fOpen >1e-4?strTemp:_T("-") );
	
	// 幅度
	pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fClose,info.m_fLast) );
	strTemp	=	_T("-");
	if( info.m_fLast > 1e-4 )
		strTemp.Format( _T("%.2f%%"), 100*(info.m_fClose-info.m_fLast)/info.m_fLast );
	pDC->TextOut( x, y+219 - difY, strTemp );
	
	// 最高
	pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fHigh,info.m_fLast) );
	strTemp.Format( strPriceFmt, info.m_fHigh );
	pDC->TextOut( x2, y+219 - difY, info.m_fHigh >1e-4?strTemp:_T("-")  );
	
	// 总手
	pDC->SetTextColor( clrPlane );
	//strTemp.Format( _T("%.0f"), info.IsBond()?info.m_fVolume:info.m_fVolume/100. );
	strTemp.Format( _T("%.0f"), info.m_fVolume/100. );
	//strTemp.Format( _T("%.0f"), info.IsBond()? info.m_total_vol:info.m_total_vol/100. );
	pDC->TextOut( x, y+235 - difY, strTemp);
	//pDC->TextOut( x, y+235 - difY, info.m_fVolume >1e-4?strTemp:_T("-")  );
	
	// 最低
	pDC->SetTextColor( RT_DETERMINE_COLOR(info.m_fLow,info.m_fLast) );
	strTemp.Format( strPriceFmt, info.m_fLow );
	pDC->TextOut( x2, y+235 - difY, info.m_fLow  >1e-4?strTemp:_T("-"));
	
	// 现手
	pDC->SetTextColor( clrPlane );
	strTemp	=	_T("-");
	//double	dVolNow = 0, dVolOuter = 0, dVolInner = 0;
	//BOOL bStatSuc = m_CurStock.GetMinute().StatVolumeInfo( &dVolNow, &dVolOuter, &dVolInner );
	//if( bStatSuc && dVolNow !=0 )
	//	strTemp.Format( _T("%.0f"),info.IsBond()?dVolNow:dVolNow/100. );
	strTemp.Format( _T("%.0f"), info.m_fAvailVolume/100. );
	//strTemp.Format( _T("%.0f"), info.IsBond()? info.m_avail_vol:info.m_avail_vol/100. );
	pDC->TextOut( x, y+251 - difY, strTemp );
	
	// 量比
	pDC->SetTextColor( clrPlane );
	double	dVolRatio = 0;
	strTemp	=	_T("-");
	if( info.GetRatioVolume( &dVolRatio, info.m_datetech, AfxGetProfile().GetRatioVolumeDays() ) )
	{
		CSPTime	tTrade;
#if 1
		if( tTrade.FromStockTimeDay( info.m_datetech ) )
			dVolRatio = dVolRatio / CSPTime::GetTimeTradeRatioOfOneDay( tTrade, CSPTime::GetCurrentTime() );
#else
		  // 按照最新交易日比较 08.01.16
        if( tTrade.FromStockTimeDay( info.m_datetech ) )
			dVolRatio = dVolRatio / CSPTime::GetTimeTradeRatioOfOneDay( tTrade, CSPTime::GetLatestTradeTime(time(NULL)) );

#endif
		strTemp.Format( _T("%.2f"), dVolRatio );
	}
	pDC->TextOut( x2, y+251 - difY,dVolRatio!=0? strTemp:_T("-") );

	// 外盘
	pDC->SetTextColor( clrRise );
	strTemp	=	_T("-");
	//if( bStatSuc && dVolOuter!=0)
	//	strTemp.Format( _T("%.0f"), info.IsBond()?dVolOuter:dVolOuter/100 );
	if ( info.m_fOuterVolume != 0)
		strTemp.Format( _T("%.0f"), info.m_fOuterVolume/100 );
	//strTemp.Format( _T("%.0f"),info.IsBond()? info.m_outer_vol: info.m_outer_vol/100 );

	pDC->TextOut( x, y+270 - difY, strTemp );
	
	// 内盘
	pDC->SetTextColor( clrFall );
	strTemp	=	_T("-");
	//if( bStatSuc && dVolInner !=0)
	//	strTemp.Format( _T("%.0f"), info.IsBond()?dVolInner:dVolInner/100 );
	if ( info.m_fInnerVolume != 0 )
	   strTemp.Format( _T("%.0f"), info.m_fInnerVolume/100 );
	//strTemp.Format( _T("%.0f"),info.IsBond()? info.m_inner_vol: info.m_inner_vol/100 );

	pDC->TextOut( x2, y+270 - difY, strTemp );

	// 涨家数，跌家数
	nRetValue = 287 - difY;
	/*if( info.IsIndex() )
	{
		pDC->SetTextColor( clrRise );
		strTemp	=	_T("-");
		if( info.m_dwAdvance > 0 )
			strTemp.Format( _T("%uf"), info.m_dwAdvance );
		pDC->TextOut( x, y+289 - difY, strTemp );

		pDC->SetTextColor( clrFall );
		strTemp	=	_T("-");
		if( info.m_dwDecline > 0 )
			strTemp.Format( _T("%uf"), info.m_dwDecline );
		pDC->TextOut( x2, y+289 - difY, strTemp );
		
		nRetValue = 306 - difY;
	} */
     pDC->SetTextAlign( nOldAlign );
   }

	
	pDC->SelectObject( pOldFont );

	return nRetValue;
}

//换手率
void CStockGraph::DrawReportParam( CDC * pDC )
{

	DECLARE_COLOR_DEFINATION

	// Prepare Data
/* Old Version
	m_CurStock.PrepareData( CStock::dataK, CKData::ktypeDay );
	m_CurStock.PrepareData( CStock::dataDR );
	m_CurStock.PrepareData( CStock::dataK, m_nCurKType );
	CStockInfo	& info	=	m_CurStock.GetStockInfo();
	CKData	& kday	=	m_CurStock.GetKDataDay();
	CKData	& kdata	=	m_CurStock.GetKData(m_nCurKType);
	CKData	& kdayMain	=	AfxGetStockMain().GetKDataDay();
	kday.SetDRData( m_CurStock.GetDRData() );
	if( kday.GetCurFormat() < CKData::formatMin
		|| kday.GetCurFormat() > CKData::formatMax )
		return ;
	kday.FullFillKData( kdayMain, FALSE );
	if( CKData::formatXDRup != kday.GetCurFormat() && CKData::formatXDRdown != kday.GetCurFormat() )
		kday.ChangeCurFormat( CKData::formatXDRdown, AfxGetProfile().GetAutoResumeDRBegin(), AfxGetProfile().GetAutoResumeDRLimit() );
*/
#if 0
    m_CurStock.PrepareData( CStock::dataK, CKData::ktypeDay );
	m_CurStock.PrepareData( CStock::dataDR );
	m_CurStock.PrepareData( CStock::dataK, m_nCurKType );
	
	CStockInfo	& info	=	m_CurStock.GetStockInfo();
	CKData	& kday	=	m_CurStock.GetKDataDay();
	CKData	& kdata	=	m_CurStock.GetKData(m_nCurKType);
	CKData	& kdayMain	=	AfxGetStockMain().GetKDataDay();
	kday.SetDRData( m_CurStock.GetDRData() );
//	if( kday.GetCurFormat() < CKData::formatMin	|| kday.GetCurFormat() > CKData::formatMax )
//	{
//		return ;
//	}
	
	kday.FullFillKData( kdayMain, FALSE );
	
//	if( CKData::formatXDRup != kday.GetCurFormat() && CKData::formatXDRdown != kday.GetCurFormat() )
//		kday.ChangeCurFormat( CKData::formatXDRdown, AfxGetProfile().GetAutoResumeDRBegin(), AfxGetProfile().GetAutoResumeDRLimit() );

#else

	CStockInfo	&info	=	m_CurStock.GetStockInfo();

	CKData	& kday	=	m_CurStockDown.GetKDataDay();
	CKData	& kdata =	m_CurStockDown.GetKData(m_nCurKType);
	CKData	& kdayMain	=	AfxGetStockMain().GetKDataDay();

#endif

	double		dValue	=	0.;
	DWORD	dateCurrent	=	kdata.GetDate(m_nIndexCurrent);
	DWORD	dateDayCurrent	=	kdata.GetDateDay( m_nIndexCurrent );
	int		nIndexDay		=	kday.GetIndexByDate( dateDayCurrent );
	int		nIndexDayMain	=	kdayMain.GetIndexByDate( dateDayCurrent );

	// Draw Rect
	pDC->FillSolidRect( &m_rectReport, clrBK );
	pDC->Draw3dRect( &m_rectReport, clrBorder, clrBorder );
	pDC->SetBkColor( clrBK );

	// Select Name And Code Font
	CString	strTemp, strFont, strTempFmt, strDay;
	strDay.LoadString( IDS_STOCKGRAPH_DAY );
	CFont	* pOldFont	=	AfxSelectDCFont( pDC, 18 );

	// Draw Name And Code
	pDC->SetTextColor( clrTitle );
	pDC->TextOut( m_rectReport.left+1, m_rectAll.top+3, m_CurStock.GetStockInfo().GetStockName() );
	pDC->SetTextColor( clrText );
	pDC->TextOut( m_rectReport.left+96, m_rectAll.top+3, m_CurStock.GetStockInfo().GetStockCode() );

	// Select Param Font
	AfxSelectDCFont( pDC, 14 );
	
	// Draw Param Names
	int	x	=	m_rectReport.left + 6;
	int y	=	m_rectReport.top;
	int y1  =   y;

	//if ( !info.IsIndex() )
	//{
	//  pDC->SetTextColor( clrText );
	//  strTemp.LoadString( IDS_STOCKGRAPH_TIME2 );
	//  pDC->TextOut( x-3, y+5,   strTemp ); //时间
	//  strTempFmt.Format( _T("-%d%s-"), AfxGetProfile().GetYieldAverageDays(), strDay );
	//}
// 显示委买卖价，收盘价等

	int nBuySellHeight = DrawBuySell( pDC,info, m_rectReport.left, m_rectReport.top+5, m_rectReport.Width() );
#ifdef _IS_DEBUG
//    pDC->SetTextAlign( nOldAlign );
	//pDC->SelectObject( pOldFont );
	return;
#endif
//	UINT nOldAlign = pDC->SetTextAlign( TA_RIGHT | TA_TOP );
    if ( info.IsIndex() )
	{
        int nSize = kday.GetSize();
		if (  nSize >= 6 && info.IsIndex() )
	    {
         //A7 ＝ 2 × 前三天收盘价 － 前六天收盘价＝5897×2－5562＝6232 
         //A8 ＝ 2 × 前二天收盘价 － 前五天收盘价＝5954×2－5589＝6319 
         //A9 ＝ 2 × 前一天收盘价 － 前四天收盘价＝5914×2－5748＝6080

	     double dA7,dA8,dA9;

         dA7 = 2 * kday.ElementAt(nSize-3).m_fClose - kday.ElementAt(nSize-6).m_fClose;
		 dA8 = 2 * kday.ElementAt(nSize-2).m_fClose - kday.ElementAt(nSize-5).m_fClose;
		 dA9 = 2 * kday.ElementAt(nSize-1).m_fClose - kday.ElementAt(nSize-4).m_fClose;
         y +=nBuySellHeight-5;
         int difY = 5;
	
		 pDC->SetTextColor( clrText );
		
		 x	=	m_rectReport.right - 5;
		 CString strPriceFmt;
         strPriceFmt.Format( _T("%%.%df"), info.DigitBit() );
         strTemp.Format( strPriceFmt, dA7 );
         pDC->TextOut( x, y+366, strTemp );

         strPriceFmt.Format( _T("%%.%df"), info.DigitBit() );
         strTemp.Format( strPriceFmt, dA8 );
         pDC->TextOut( x, y+366+19, strTemp );

         strPriceFmt.Format( _T("%%.%df"), info.DigitBit() );
         strTemp.Format( strPriceFmt, dA9 );
         pDC->TextOut( x, y+366+38, strTemp );

	   }
	}
	else
	{
	  pDC->SetTextColor( clrText );
      y +=nBuySellHeight;
   // DrawLine( pDC, 1, clrBorder, m_rectReport.left, y+130, m_rectReport.right, y+130 );

	//DrawLine( pDC, 1, clrBorder, m_rectReport.left, y+126, m_rectReport.right, y+126 );
    
//	y +=112;
    int difY = 5;
#if 0
	strTemp.LoadString( IDS_STOCKGRAPH_YIELDAVERAGE ); // 年收益率
	pDC->TextOut( x, y+30 - difY ,  strTemp+strTempFmt );
	strTemp.LoadString( IDS_STOCKGRAPH_YIELDSTDDEV );
	pDC->TextOut( x, y+55 - difY - 6,  strTemp+strTempFmt );
	strTemp.LoadString( IDS_STOCKGRAPH_BEITE );
	pDC->TextOut( x, y+80 - difY - 12,  strTemp+strTempFmt );
	strTemp.LoadString( IDS_STOCKGRAPH_XIAPU );
	pDC->TextOut( x, y+105 - difY -18 , strTemp+strTempFmt );
	DrawLine( pDC, 1, clrBorder, m_rectReport.left, y+126 - difY-22, m_rectReport.right, y+126 - difY-22 );
#else

	 pDC->SetTextColor( clrText );
	 strTemp.LoadString( IDS_STOCKGRAPH_TIME2 );
	 pDC->TextOut( x , y + 8,   strTemp ); //时间
	 strTempFmt.Format( _T("-%d%s-"), AfxGetProfile().GetYieldAverageDays(), strDay );

    y = y - 75;
#endif    
	strTempFmt.LoadString( IDS_STOCKGRAPH_CHANGEHAND );
	strTemp.Format( _T("%s-%d"), strTempFmt, 1 );
	pDC->TextOut( x, y+130 - difY -24, strTemp );
	strTemp.Format( _T("%s-%d"), strTempFmt, 5 );
	pDC->TextOut( x, y+155 - difY -30, strTemp );
	strTemp.Format( _T("%s-%d"), strTempFmt, 10 );
	pDC->TextOut( x, y+180 - difY -36 , strTemp );
	strTemp.Format( _T("%s-%d"), strTempFmt, 20 );
	pDC->TextOut( x, y+205 - difY -42 , strTemp );

	DrawLine( pDC, 1, clrBorder, m_rectReport.left, y+226 - difY-46, m_rectReport.right, y+226  - difY-46);
	strTempFmt.LoadString( IDS_STOCKGRAPH_RATIOVOLUME );
	strTemp.Format( _T("%s-%d"), strTempFmt, 5 );
	pDC->TextOut( x, y+230 - difY-48, strTemp );
	strTemp.Format( _T("%s-%d"), strTempFmt, 10 );
	pDC->TextOut( x, y+255 - difY-54, strTemp );
	strTemp.Format( _T("%s-%d"), strTempFmt, 20 );
	pDC->TextOut( x, y+280 - difY-60, strTemp );

	DrawLine( pDC, 1, clrBorder, m_rectReport.left, y+301- difY-64, m_rectReport.right, y+301- difY-64 );
	strTempFmt.LoadString( IDS_STOCKGRAPH_RS );
	strTemp.Format( _T("%s-%d"), strTempFmt, 5 );
	pDC->TextOut( x, y+305- difY-66, strTemp );
	strTemp.Format( _T("%s-%d"), strTempFmt, 10 );
	pDC->TextOut( x, y+330- difY-72, strTemp );
	strTemp.Format( _T("%s-%d"), strTempFmt, 20 );
	pDC->TextOut( x, y+355- difY-78, strTemp );

	DrawLine( pDC, 1, clrBorder, m_rectReport.left, y+376- difY-82, m_rectReport.right, y+376- difY-82 );
	strTemp.LoadString( IDS_STOCKGRAPH_SHARETOTAL );
	pDC->TextOut( x, y+380- difY-84, strTemp );
	strTemp.LoadString( IDS_STOCKGRAPH_SHARECURRENCY );
	pDC->TextOut( x, y+405- difY-90, strTemp );
	strTemp.LoadString( IDS_STOCKGRAPH_NETASSETPS );
	pDC->TextOut( x, y+430- difY-96, strTemp );
	strTemp.LoadString( IDS_STOCKGRAPH_EPS );
	pDC->TextOut( x, y+455- difY-102, strTemp );
	strTemp.LoadString( IDS_STOCKGRAPH_PE );
	pDC->TextOut( x, y+480- difY-108, strTemp );

    strTemp.LoadString( IDS_PROTECTED_PRICE );
	pDC->TextOut( x, y+505- difY-114, strTemp );


	//  x = m_rectReport.left + 6;
    pDC->TextOut( x, y+530- difY-120, m_strBSDesc );
/*
	strTemp.LoadString( IDS_PREINDEX_A7 );
	pDC->TextOut( x, y+505- difY-114+16, strTemp );
	strTemp.LoadString( IDS_PREINDEX_A8 );
	pDC->TextOut( x, y+505- difY-114+32, strTemp );
	strTemp.LoadString( IDS_PREINDEX_A9 );
	pDC->TextOut( x, y+505- difY-114+48, strTemp );

*/

#if 1
	// Draw Param Values
	// Time
	y = y1;
	UINT nOldAlign = pDC->SetTextAlign( TA_RIGHT | TA_TOP );
	pDC->SetTextColor( clrPlane );
	x	=	m_rectReport.right - 5;
    strTemp	=	("----");
	
	y += nBuySellHeight;
#else
    info.m_time;

#endif
	
	// Change Hand
	strTemp	=	("-   ");
	// 计算1，5，10，15，20日换手率
#if 0
	CSPTime	sptime1, sptime2;
	int nIndex = kdata.GetSize()-1;
	int nDays = 1;
	if ( nIndex  > 2 )
	{
	   sptime1.FromStockTime( kdata.ElementAt(nIndex-nDays).m_date, TRUE );
	   sptime2.FromStockTime( kdata.ElementAt(nIndex).m_date, TRUE );
	   //DWORD dwDate  =  nIndex ;//sptime2.ToStockTimeDay();   //最近的交易日期
      // DWORD dwDate1 =  nIndex ;//sptime1.ToStockTimeDay();   //保存的交易日期
	  // sptime.Format( _T("%Y-%m-%d")
  	  //  strTemp.Format(_T("流通比=%d,%d -> %d!"),dwDate,dwDate1,CSPTimeSpan(nDays+8,0,0,0));
	    strTemp = sptime2.Format( _T("%Y-%m-%d") )+ sptime1.Format( _T("%Y-%m-%d") );
		//strTemp.Format(_T("%d"),sptime2 - sptime1);
	}
	else
	{
		strTemp.Format(_T("%d,%d"),nIndex,CSPTimeSpan(nDays+8,0,0,0));
	}
		//::MessageBox(NULL,strData,_T("计算换手率") ,MB_OK);
#endif

	CSPTime	sptime;
	if( CKData::IsDayOrMin( m_nCurKType ) && dateCurrent > 0 ) //add
	{
		sptime.FromStockTimeDay( dateCurrent );
		strTemp	=	sptime.Format( _T("%Y.%m.%d") );
	}
	else if ( dateCurrent > 0 ) // add
	{
		sptime.FromStockTimeMin( dateCurrent	);  // 日期有时为啥不对？，接口需要将接收到数据进行转换ToStockTimeMin();
		//CSPTime sptime1( dateDayCurrent );
		strTemp	=	sptime.Format( _T("%Y.%m.%d %H:%M") ); // 分
		//strTemp.Format(_T("%ld"),dateDayCurrent ); 
	}
	else // add 2008.03.22 显示最后交易日期
	{
		CSPTime tLatest( CSPTime::GetLatestTradeTime(time(NULL)) );
		strTemp	=	tLatest.Format( _T("%Y.%m.%d") );
	}
	pDC->TextOut( x, y+8, strTemp );

	y = y - 75;
    strTemp	=	_T("-   ");
	if( info.GetRatioChangeHand( kdata, &dValue, dateCurrent, 1) ) // 与前一天比较
		strTemp.Format( _T("%.2f%%"), dValue +1);
	pDC->TextOut( x, y+130- difY -24, strTemp );
	strTemp	=	_T("-   ");
	if( info.GetRatioChangeHand( kdata, &dValue, dateCurrent, 5) )
		strTemp.Format( _T("%.2f%%"), dValue +1);
	pDC->TextOut( x, y+155- difY -30, strTemp );
	strTemp	=	_T("-   ");
	if( info.GetRatioChangeHand( kdata, &dValue, dateCurrent, 10) )
		strTemp.Format( _T("%.2f%%"), dValue );
	pDC->TextOut( x, y+180- difY -36, strTemp );
	strTemp	=	_T("-   ");
	if( info.GetRatioChangeHand( kdata, &dValue, dateCurrent, 20) )
		strTemp.Format( _T("%.2f%%"), dValue );
	pDC->TextOut( x, y+205- difY -42, strTemp );

	// Ratio Volume
	strTemp	=	_T("-   ");
	if( kdata.GetRatioVolume(&dValue, dateCurrent, 5) )
		strTemp.Format( _T("%.2f"), dValue );
	pDC->SetTextColor( dValue >= 1.005 ? clrRise : (dValue<0.995?clrFall:clrPlane) );
	pDC->TextOut( x, y+230- difY -48, strTemp );
	strTemp	=	_T("-   ");
	if( kdata.GetRatioVolume(&dValue, dateCurrent, 10) )
		strTemp.Format( _T("%.2f"), dValue );
	pDC->SetTextColor( dValue >= 1.005 ? clrRise : (dValue<0.995?clrFall:clrPlane) );
	pDC->TextOut( x, y+255- difY -54, strTemp );
	strTemp	=	_T("-   ");
	if( kdata.GetRatioVolume(&dValue, dateCurrent, 20) )
		strTemp.Format( _T("%.2f"), dValue );
	pDC->SetTextColor( dValue >= 1.005 ? clrRise : (dValue<0.995?clrFall:clrPlane) );
	pDC->TextOut( x, y+280- difY -60, strTemp );

	// RS
	strTemp	=	_T("-   ");
	if( kdata.GetRS(&dValue, dateCurrent, 5) )
		strTemp.Format( _T("%.2f%%"), dValue );
	pDC->SetTextColor( dValue >= 1.005 ? clrRise : (dValue<0.995?clrFall:clrPlane) );
	pDC->TextOut( x, y+305- difY -66, strTemp );
	strTemp	=	_T("-   ");
	if( kdata.GetRS(&dValue, dateCurrent, 10) )
		strTemp.Format( _T("%.2f%%"), dValue );
	pDC->SetTextColor( dValue >= 1.005 ? clrRise : (dValue<0.995?clrFall:clrPlane) );
	pDC->TextOut( x, y+330- difY -72, strTemp );
	strTemp	=	_T("-   ");
	if( kdata.GetRS(&dValue, dateCurrent, 20) )
		strTemp.Format( _T("%.2f%%"), dValue );
	pDC->SetTextColor( dValue >= 1.005 ? clrRise : (dValue<0.995?clrFall:clrPlane) );
	pDC->TextOut( x, y+355- difY -78, strTemp );

	// Base Info
	pDC->SetTextColor( clrPlane );
	strTemp	=	_T("-   ");
	if( info.m_fShare_count_total > 1e-4 )
		strTemp.Format( _T("%.0f"), info.m_fShare_count_total );// *0.0001
	pDC->TextOut( x, y+380- difY -84, strTemp );
	strTemp	=	_T("-   ");
	if( info.GetShareCurrency( &dValue ) )
		strTemp.Format( _T("%.0f"), dValue*0.0001);//  
	pDC->TextOut( x, y+405- difY -90, strTemp );
	strTemp	=	_T("-   ");
	if( fabs(info.m_fNetasset_ps) > 1e-4 )
		strTemp.Format( _T("%.4f"), info.m_fNetasset_ps );
	pDC->TextOut( x, y+430- difY -96, strTemp );
	strTemp	=	_T("-   ");
	if( fabs(info.m_fEps) > 1e-4 )
		strTemp.Format( _T("%.4f"), info.m_fEps);
		//strTemp.Format( _T("%.4f(%d)"), info.m_fEps, info.m_reporttype );
	pDC->TextOut( x, y+455- difY -102, strTemp );
	strTemp	=	_T("-   ");
	if( info.GetPE(&dValue) )
		strTemp.Format( _T("%.4f"), dValue );
	pDC->TextOut( x, y+480- difY -108, strTemp );

	strTemp = _T("-   ");
	int  nSize = kday.GetSize();
	if (  nSize >= 3 && info.m_fOpen > 0)
	{
       double fProtectedPrice;  
	   fProtectedPrice = (kday.ElementAt(nSize-2).m_fClose+kday.ElementAt(nSize-3).m_fClose + info.m_fOpen)/3.0;
	   CString strPriceFmt;
       strPriceFmt.Format( _T("%%.%df"), info.DigitBit() );
       strTemp.Format( strPriceFmt, fProtectedPrice );
	   pDC->SetTextColor( clrRise );
	}

	pDC->TextOut( x, y+505- difY-114, strTemp );
	pDC->SetTextColor( clrPlane );
  
#if 0
	// Yield
	double	market_dayyield_average = 0.0, market_dayyield_d = 0.0;
	BOOL	bmarket_yield_ok	=	TRUE;
	if( !CalculateYieldDay( kdayMain, &market_dayyield_average, &market_dayyield_d, AfxGetProfile().GetYieldAverageDays(), nIndexDayMain ) )
		bmarket_yield_ok	=	FALSE;
	double	yield_average = 0., yield_d = 0., beite = 0., xiapu = 0. ;
	if( CalculateYieldDay( kday, &yield_average, &yield_d, AfxGetProfile().GetYieldAverageDays(), nIndexDay ) )
	{
		yield_average	=	( 100. * (pow(1+yield_average,STKLIB_DAYS_INONEYEAR)-1) );
		yield_d			=	( 100. * sqrt((double)STKLIB_DAYS_INONEYEAR) * yield_d );
		if( fabs(yield_d) > 0.01 )
		{
			xiapu	=	100. * yield_average / yield_d;
			strTemp.Format( _T("%.2f%%"), xiapu );
			pDC->TextOut( x, y+105- difY -18, strTemp );
		}

		strTemp.Format( _T("%.2f%%"), yield_average );
		pDC->TextOut( x, y+30- difY , strTemp );
		strTemp.Format( _T("%.2f%%"), yield_d );
		pDC->TextOut( x, y+55- difY -6, strTemp );
	}
	if( bmarket_yield_ok && CalculateBeiteDay( kday, kdayMain, market_dayyield_average, market_dayyield_d, &beite, AfxGetProfile().GetYieldAverageDays(), nIndexDay ) )
	{
		beite		=	beite;
		strTemp.Format( _T("%.2f"), beite );
		pDC->TextOut( x, y+80- difY -12, strTemp );
	}
#endif
	
	 pDC->SetTextAlign( nOldAlign );
	}
	pDC->SelectObject( pOldFont );

//	kday.ChangeCurFormat( m_nCurKFormat, AfxGetProfile().GetAutoResumeDRBegin(), AfxGetProfile().GetAutoResumeDRLimit() );
}

// 画 成本分布图/集中度,获利比例,平均成本
void CStockGraph::DrawReportCost( CDC * pDC )
{
#ifdef _IS_DEBUG
	return;
#endif

	DECLARE_COLOR_DEFINATION

	// Draw Rect
	pDC->FillSolidRect( &m_rectReport, clrBK );
	pDC->Draw3dRect( &m_rectReport, clrBorder, clrBorder );
	pDC->SetBkColor( clrBK );

	// Select Name And Code Font
	CString	strTemp, strFont, strTempFmt, strDay;
	strDay.LoadString( IDS_STOCKGRAPH_DAY );
	CFont	* pOldFont	=	AfxSelectDCFont( pDC, 18 );
	UINT nOldAlign = pDC->SetTextAlign( TA_LEFT | TA_TOP );

	// Draw Name And Code
	pDC->SetTextColor( clrTitle );
	pDC->TextOut( m_rectReport.left+1, m_rectAll.top+3, m_CurStock.GetStockInfo().GetStockName() );
	pDC->SetTextColor( clrText );
	pDC->TextOut( m_rectReport.left+96, m_rectAll.top+3, m_CurStock.GetStockInfo().GetStockCode() );

	// 成本分布图
	CKData	& kdata	=	m_CurStock.GetKData(m_nCurKType);
	double	dStep	=	(m_dMaxPrice-m_dMinPrice)*0.003;
	CSPDWordArray adwPrice, adwVolume;
	double	dMinVolume = 0, dMaxVolume = 0, dTotalVolume = 0, dVolPercent = 0;

	CCW	cw(&kdata);
	cw.m_dChangeHand	=	1.5;
	int	nEnd = m_nIndexCurrent;
	if( -1 == nEnd )
		nEnd	=	kdata.GetSize()-1;
	if( !cw.CalculatePastCW(nEnd, 0, m_CurStock.GetStockInfo(), dStep, adwPrice, adwVolume, &dMinVolume, &dMaxVolume, &dTotalVolume, &dVolPercent )
		|| dMinVolume < 0 || dMaxVolume < 1e-4 || dTotalVolume < 1e-4 )
		return;

	COLORREF	clrGain = RGB(255,128,0), clrBad = RGB(160,160,255);
	CPen	penGain( PS_SOLID, 1, clrGain );
	CPen	penBad( PS_SOLID, 1, clrBad );
	CPen * pOldPen = pDC->SelectObject( &penGain );

	double	dPriceSel = 0;
	if( nEnd >= 0 && nEnd < kdata.GetSize() )
		dPriceSel = kdata.ElementAt(nEnd).m_fClose;

	double	dVolume = 0, dPrice = 0;
	double	dVolFactor	=	-1;
	for( int k=0; k<adwPrice.GetSize() && k<adwVolume.GetSize(); k++ )
	{
		if( dVolFactor < 0 )
			dVolFactor	=	40.0/dTotalVolume;

		dPrice	=	double(adwPrice[k]) * 0.001;
		dVolume	=	adwVolume[k];
		if( dPrice > dPriceSel )
			pDC->SelectObject( &penBad );
		int	yPrice	=	int( m_rectKLineCenter.bottom - m_rectKLineCenter.Height() * (dPrice - m_dMinPrice) / (m_dMaxPrice-m_dMinPrice) );
		int	xVolume	=	int( m_rectReport.left + (m_rectReport.Width()*dVolume*dVolFactor) );
		if( yPrice > m_rectReport.top+1 && yPrice < m_rectReport.bottom-2 )
		{
			pDC->MoveTo( m_rectReport.left + 1, yPrice );
			pDC->LineTo( xVolume, yPrice );
		}
	}

	// Select Param Font
	AfxSelectDCFont( pDC, 12 );
	pDC->SetBkColor( clrBK );
	pDC->SetTextColor( clrText );
	int	x = m_rectReport.left+2;
	int y = m_rectReport.bottom-100;

	// Date
	CString	strDate;
	if( nEnd >= 0 && nEnd < kdata.GetSize() )
		strDate	=	::AfxGetTimeString( kdata.ElementAt(nEnd).m_time, _T("%Y.%m.%d"), TRUE );
	pDC->TextOut( x+10, y, strDate );

	// 获利比例： 
	double	dGainPercent = 0;
	CCW::StatGainPercent( &dGainPercent, adwPrice, adwVolume, dPriceSel );
	strTemp.LoadString( IDS_STOCKGRAPH_GAINPERCENT );
	pDC->TextOut( x, y+20, strTemp );
	strTemp.Format( _T("%.1f%%"), dGainPercent*100 );
	pDC->TextOut( x+100, y+20, strTemp );
	CRect	rectGain( x+60, y+20, x+160, y+32 );
	pDC->Draw3dRect( rectGain, clrBad, clrBad );
	rectGain.right	=	rectGain.left + rectGain.Width() * dGainPercent;
	pDC->Draw3dRect( rectGain, clrGain, clrGain );
	pDC->SetBkColor( clrBK );

	// 平均成本： %.3f元
	double	dCostAve = 0;
	CCW::StatCostAverage( &dCostAve, adwPrice, adwVolume );
	strTempFmt.LoadString( IDS_STOCKGRAPH_COSTAVE );
	strTemp.Format( strTempFmt, dCostAve );
	pDC->TextOut( x, y+36, strTemp );

	// 90%成本%.2f-%.2f，集中度%.1f%%
	double	dLow = 0, dHigh = 0, dMassPrice = 0;
	CCW::StatMass( &dLow, &dHigh, &dMassPrice, adwPrice, adwVolume, 0.9 );
	strTempFmt.LoadString( IDS_STOCKGRAPH_MASSFMT90 );
	strTemp.Format( strTempFmt, dLow, dHigh, dMassPrice*100 );
	pDC->TextOut( x, y+52, strTemp );

	// 70%成本%.2f-%.2f，集中度%.1f%%
	dLow = 0, dHigh = 0, dMassPrice = 0;
	CCW::StatMass( &dLow, &dHigh, &dMassPrice, adwPrice, adwVolume, 0.7 );
	strTempFmt.LoadString( IDS_STOCKGRAPH_MASSFMT70 );
	strTemp.Format( strTempFmt, dLow, dHigh, dMassPrice*100 );
	pDC->TextOut( x, y+68, strTemp );

	pDC->SelectObject( pOldPen );
	pDC->SetTextAlign( nOldAlign );
	pDC->SelectObject( pOldFont );
}

// 火焰山
void CStockGraph::DrawReportFlame( CDC * pDC )
{
#ifdef _IS_DEBUG
	return;
#endif
	DECLARE_COLOR_DEFINATION

	// Draw Rect
	pDC->FillSolidRect( &m_rectReport, clrBK );
	pDC->Draw3dRect( &m_rectReport, clrBorder, clrBorder );
	pDC->SetBkColor( clrBK );

	// Select Name And Code Font
	CString	strTemp, strFont, strTempFmt, strDay;
	strDay.LoadString( IDS_STOCKGRAPH_DAY );
	CFont	* pOldFont	=	AfxSelectDCFont( pDC, 18 );
	UINT nOldAlign = pDC->SetTextAlign( TA_LEFT | TA_TOP );

	// Draw Name And Code
	pDC->SetTextColor( clrTitle );
	pDC->TextOut( m_rectReport.left+1, m_rectAll.top+3, m_CurStock.GetStockInfo().GetStockName() );
	pDC->SetTextColor( clrText );
	pDC->TextOut( m_rectReport.left+96, m_rectAll.top+3, m_CurStock.GetStockInfo().GetStockCode() );

	// 火焰山
	CKData	& kdata	=	m_CurStock.GetKData(m_nCurKType);
	int	nEnd = m_nIndexCurrent;
	if( -1 == nEnd )
		nEnd	=	kdata.GetSize()-1;

	int	nBot = 5;
	CRect	rectDescript( m_rectReport.left+30, m_rectReport.bottom-nBot-100, m_rectReport.left+170, m_rectReport.bottom-nBot-84);
	DrawReportCW( pDC, FALSE, nEnd, 5, 1.5, RGB(255,0,0), rectDescript );
	
	rectDescript	=	CRect( m_rectReport.left+30, m_rectReport.bottom-nBot-84, m_rectReport.left+170, m_rectReport.bottom-nBot-68);
	DrawReportCW( pDC, FALSE, nEnd, 10, 1.5, RGB(255,0,128), rectDescript );

	rectDescript	=	CRect( m_rectReport.left+30, m_rectReport.bottom-nBot-68, m_rectReport.left+170, m_rectReport.bottom-nBot-52);
	DrawReportCW( pDC, FALSE, nEnd, 20, 1.5, RGB(255,128,0), rectDescript );
	
	rectDescript	=	CRect( m_rectReport.left+30, m_rectReport.bottom-nBot-52, m_rectReport.left+170, m_rectReport.bottom-nBot-36);
	DrawReportCW( pDC, FALSE, nEnd, 30, 1.5, RGB(255,128,128), rectDescript );

	rectDescript	=	CRect( m_rectReport.left+30, m_rectReport.bottom-nBot-36, m_rectReport.left+170, m_rectReport.bottom-nBot-20);
	DrawReportCW( pDC, FALSE, nEnd, 60, 1.5, RGB(255,255,0), rectDescript );

	// Select Param Font
	AfxSelectDCFont( pDC, 12 );

	// Date
	pDC->SetBkColor( clrBK );
	pDC->SetTextColor( clrText );
	CString	strDate;
	if( nEnd >= 0 && nEnd < kdata.GetSize() )
		strDate	=	::AfxGetTimeString( kdata.ElementAt(nEnd).m_time, _T("%Y.%m.%d"), TRUE );
	pDC->TextOut( m_rectReport.left+20, m_rectReport.bottom-20, strDate );

	pDC->SetTextAlign( nOldAlign );
	pDC->SelectObject( pOldFont );
}

// 显示活跃度
void CStockGraph::DrawReportActivity( CDC * pDC )
{
#ifdef _IS_DEBUG
  return;
#endif

	DECLARE_COLOR_DEFINATION

	// Draw Rect
	pDC->FillSolidRect( &m_rectReport, clrBK );
	pDC->Draw3dRect( &m_rectReport, clrBorder, clrBorder );
	pDC->SetBkColor( clrBK );

	// Select Name And Code Font
	CString	strTemp, strFont, strTempFmt, strDay;
	strDay.LoadString( IDS_STOCKGRAPH_DAY );
	CFont	* pOldFont	=	AfxSelectDCFont( pDC, 18 );
	UINT nOldAlign = pDC->SetTextAlign( TA_LEFT | TA_TOP );

	// Draw Name And Code
	pDC->SetTextColor( clrTitle );
	pDC->TextOut( m_rectReport.left+1, m_rectAll.top+3, m_CurStock.GetStockInfo().GetStockName() );
	pDC->SetTextColor( clrText );
	pDC->TextOut( m_rectReport.left+96, m_rectAll.top+3, m_CurStock.GetStockInfo().GetStockCode() );

	// 活跃度
	CKData	& kdata	=	m_CurStock.GetKData(m_nCurKType);
	int	nEnd = m_nIndexCurrent;
	if( -1 == nEnd )
		nEnd	=	kdata.GetSize()-1;

	int	nBot = 5;
	CRect	rectDescript( m_rectReport.left+30, m_rectReport.bottom-nBot-36, m_rectReport.left+170, m_rectReport.bottom-nBot-20);
	DrawReportCW( pDC, TRUE, nEnd, 60, 1, RGB(0,40,160), rectDescript );

	rectDescript	=	CRect( m_rectReport.left+30, m_rectReport.bottom-nBot-52, m_rectReport.left+170, m_rectReport.bottom-nBot-36);
	DrawReportCW( pDC, TRUE, nEnd, 30, 1, RGB(80,80,255), rectDescript );

	rectDescript	=	CRect( m_rectReport.left+30, m_rectReport.bottom-nBot-68, m_rectReport.left+170, m_rectReport.bottom-nBot-52);
	DrawReportCW( pDC, TRUE, nEnd, 20, 1, RGB(120,80,255), rectDescript );

	rectDescript	=	CRect( m_rectReport.left+30, m_rectReport.bottom-nBot-84, m_rectReport.left+170, m_rectReport.bottom-nBot-68);
	DrawReportCW( pDC, TRUE, nEnd, 10, 1, RGB(120,120,255), rectDescript );

	rectDescript	=	CRect( m_rectReport.left+30, m_rectReport.bottom-nBot-100, m_rectReport.left+170, m_rectReport.bottom-nBot-84);
	DrawReportCW( pDC, TRUE, nEnd, 5, 1, RGB(160,160,255), rectDescript );

	// Select Param Font
	AfxSelectDCFont( pDC, 12 );

	// Date
	pDC->SetBkColor( clrBK );
	pDC->SetTextColor( clrText );
	CString	strDate;
	if( nEnd >= 0 && nEnd < kdata.GetSize() )
		strDate	=	::AfxGetTimeString( kdata.ElementAt(nEnd).m_time, _T( "%Y.%m.%d"), TRUE );
	pDC->TextOut( m_rectReport.left+20, m_rectReport.bottom-20, strDate );

	pDC->SetTextAlign( nOldAlign );
	pDC->SelectObject( pOldFont );
}

//显示几日内成本百分比
void CStockGraph::DrawReportCW( CDC * pDC, BOOL bRecent, int nEnd, int nDays, double dChangeHand, COLORREF clr, CRect rectDescript )
{

	double	dVolFactor = -1;

	CKData	& kdata	=	m_CurStock.GetKData(m_nCurKType);
	double	dStep	=	(m_dMaxPrice-m_dMinPrice)*0.003;
	CSPDWordArray adwPrice, adwVolume;
	double	dMinVolume = 0, dMaxVolume = 0, dTotalVolume = 0, dVolPercent = 0;

	CCW	cw(&kdata);
	cw.m_dChangeHand	=	dChangeHand;
	int	yPriceLast	=	-1;

	BOOL	bOK	=	FALSE;
	if( bRecent )
		bOK	=	cw.CalculateRecentCW(nEnd, nDays, m_CurStock.GetStockInfo(), dStep, adwPrice, adwVolume, &dMinVolume, &dMaxVolume, &dTotalVolume, &dVolPercent );
	else
		bOK	=	cw.CalculatePastCW(nEnd, nDays, m_CurStock.GetStockInfo(), dStep, adwPrice, adwVolume, &dMinVolume, &dMaxVolume, &dTotalVolume, &dVolPercent );

	CPen	pen( PS_SOLID, 1, clr );
	CPen * pOldPen = pDC->SelectObject( &pen );

	if( bOK && dMinVolume >= 0 && dMaxVolume > 1e-4 && dTotalVolume > 1e-4
		&& m_dMaxPrice-m_dMinPrice > 1e-4 && m_dMinPrice > 1e-4 )
	{
		if( dVolFactor < 0 )
			dVolFactor	=	bRecent ? 15.0/dTotalVolume : 40.0/dTotalVolume;

		CArray <POINT, POINT &>	apt;
		apt.SetSize( 0, m_rectKLineCenter.Height() );
		double	dVolume = 0, dPrice = 0;
		for( int k=0; k<adwPrice.GetSize() && k<adwVolume.GetSize(); k++ )
		{
			dPrice	=	double(adwPrice[k]) * 0.001;
			dVolume	=	adwVolume[k];
			int	yPrice	=	int( m_rectKLineCenter.bottom - m_rectKLineCenter.Height() * (dPrice - m_dMinPrice) / (m_dMaxPrice-m_dMinPrice) );
			int	xVolume	=	int( m_rectReport.left + (m_rectReport.Width()*dVolume*dVolFactor) );
			if( yPrice > m_rectReport.top+1 && yPrice < m_rectReport.bottom-2 )
			{
				if( -1 == yPriceLast )
					yPriceLast	=	yPrice+1;
				for( int y=yPriceLast-1; y>=yPrice; y-- )
				{
					pDC->MoveTo( m_rectReport.left+2, y );
					pDC->LineTo( xVolume, y );
				}
				yPriceLast	=	yPrice;
				apt.Add( CPoint(xVolume, yPrice) );
			}
		}

		if( apt.GetSize() > 0 )
		{
			CPen	penEdge( PS_SOLID, 1, RGB(255,255,255) );
			pDC->SelectObject( &penEdge );
			pDC->MoveTo( apt[0] );
			pDC->PolylineTo( apt.GetData(), apt.GetSize() );
		}
	}

	// rectDescript
	pDC->Draw3dRect( rectDescript, clr, clr );
	pDC->FillSolidRect( rectDescript, clr );
	
	CString	sText, sTextFmt;
	if( bRecent )
		sTextFmt.LoadString( IDS_STOCKGRAPH_RECENTCWDISFMT );
	else
		sTextFmt.LoadString( IDS_STOCKGRAPH_PASTCWDISFMT );
	sText.Format( sTextFmt, nDays, dVolPercent*100 );
	CFont	* pOldFont	=	AfxSelectDCFont( pDC, 12 );
	pDC->SetTextColor( RGB(255,255,255) );
	pDC->SetBkColor( clr );
	pDC->TextOut( rectDescript.left+5, rectDescript.top+2, sText );

	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldFont );
}
