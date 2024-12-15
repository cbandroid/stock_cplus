#ifndef _MACD_TECH_H_
#define _MACD_TECH_H_


#include "Technique.h"

//class CKData;
/////////////////////////////////////////////////////////////////////////
//	趋势类
//	指数平滑异同移动平均线MACD
class  CMACD : public CTechnique
{
public:
	// Constructors
	CMACD( );
	CMACD( CKData * pKData );
	virtual ~CMACD();

public:
    virtual	void Clear( )  override ;

	// Attributes
	int		m_nEMA1Days;
	int		m_nEMA2Days;
	int  	m_nDIFDays;
	int		m_itsDeviateOnBottom;
	int		m_itsDeviateOnTop;
	int		m_itsGoldenFork;
	int		m_itsDeadFork;
    virtual	void	SetDefaultParameters( )  override ;
	void	attach( CMACD & src );
    virtual	bool	IsValidParameters( )  override ;

	// Operations
    virtual	int		signal( int nIndex, uint32_t * pnCode = nullptr )  override ;
    virtual	bool	min_max_info( int nStart, int nEnd, float *pdMin,float *pdMax )  override ;
	virtual	bool	calc(float *pdEMA1, float *pdEMA2, float *pdDIF,float*pdDEA,
        int nIndex, bool bUseLast )  override ;
};
#endif
