#pragma once


#include "Technique.h"
//	随机指标KDJ
class  CKDJ : public CTechnique
{
public:
	// Constructors
	CKDJ( );
	CKDJ(CKData * pKData );
	virtual ~CKDJ();

protected:
	bool	CalculateRSV(float * pValue, int nIndex );

public:
    virtual	void Clear( )  override ;

	// Attributes
	enum modeJ	{
		mode3K2D	=	0x01,
		mode3D2K	=	0x02,
	};
	int		m_nRSVDays;
	int		m_nKDays;
	int		m_nDDays;
	int		m_nJ;		//	1 For 3K-2D, 2 For 3D-2K
	//K线和D线在25(first)以下在75(second)以上时信号较为准确;
	std::pair<int, int>     m_KDTrustValue;
	int		m_itsGoldenFork;
	int		m_itsDeadFork;
    virtual	void	SetDefaultParameters( )  override ;
	void	attach( CKDJ & src );
    virtual	bool	IsValidParameters( )  override ;

	// Operations
    virtual	int		signal(int nIndex, uint32_t * pnCode = nullptr)  override ;
    virtual	bool	min_max_info(int nStart, int nEnd, float *pdMin, float *pdMax )  override ;
    virtual	bool	calc(float *pValue1, float *pValue2, float* pValue3, int nIndex, bool bUseLast )  override ;
};
