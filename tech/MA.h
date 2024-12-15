#ifndef _TECH_MA_H_
#define _TECH_MA_H_

#include "Technique.h"
#include <vector>
/////////////////////////////////////////////////////////////////////////
//	K线属性类;
//	移动平均线MA;
class  CMA : public CTechnique
{
public:
	// Constructors
	CMA( );
	CMA(  CKData  * pKData );
	virtual ~CMA();

public:
    virtual	void Clear( )  override ;

	// Attributes
	enum MATypes {
		typeMA		=	0x01,
		typeEXPMA	=	0x02,
		typeSMA		=	0x03,
		typeDMA		=	0x04,
	};
	int				m_nType;
	std::vector<uint32_t>	m_adwMADays;
	int		m_itsGoldenFork;
	int		m_itsDeadFork;
	int		m_itsLong;
	int		m_itsShort;
	float  m_dWeight;
    virtual	void	SetDefaultParameters( )  override ;
	void	attach( CMA & src );
    virtual	bool	IsValidParameters( )  override ;

	// Operations;
    virtual	int		signal( int nIndex, uint32_t * pnCode = nullptr )  override ;
    virtual	bool	min_max_info( int nStart, int nEnd, float *pdMin, float *pdMax )  override ;
    virtual	bool	calc(float * pValue, int nIndex, int nDays, bool bUseLast )  override ;
};
#endif
