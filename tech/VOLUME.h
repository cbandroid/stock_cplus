#ifndef _TECH_VOLUME_H_
#define _TECH_VOLUME_H_

#include "Technique.h"

/////////////////////////////////////////////////////////////////////////
//	能量类
//	成交量
class  CVOLUME : public  CTechnique
{
public:
	// Constructors
	CVOLUME( );
    CVOLUME( CKData * pKData );
	virtual ~CVOLUME();

public:
    virtual	void Clear( ) override ;

	// Attributes
	std::vector<uint32_t>	m_adwMADays;//时间
	int		m_itsDeviateOnBottom;
	int		m_itsDeviateOnTop;
	int		m_itsLong;
	int		m_itsShort;
    virtual	void	SetDefaultParameters( ) override ;
	void	attach( CVOLUME & src );
    virtual	bool	IsValidParameters( ) override ;

	// Operations
    virtual	int		signal( int nIndex, uint32_t * pnCode = NULL ) override ;
    virtual	bool	min_max_info(int nStart, int nEnd, float *pdMin, float *pdMax ) override ;
    virtual	bool	calc(float * pValue, int nIndex, int nDays, bool bUseLast ) override ;
};
#endif
