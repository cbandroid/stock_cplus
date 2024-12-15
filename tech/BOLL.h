#ifndef _TECH_BOLL_H_
#define _TECH_BOLL_H_
    
#include "Technique.h"
//	布林带BOLL
class  CBOLL : public CTechnique
{
public:
	// Constructors
	CBOLL( );
	CBOLL( CKData  * pKData );
	virtual ~CBOLL();

public:
    void Clear( ) override ;

	// Attributes
	float	m_dMultiUp;
	float	m_dMultiDown;
	int		m_nMADays;
	int		m_itsSupport;
	int		m_itsResistance;
    void SetDefaultParameters( )  override ;
	void	attach( CBOLL & src );
    bool IsValidParameters( )  override ;

	// Operations
    virtual	int		signal( int nIndex, uint32_t * pnCode =nullptr)  override  ;
    virtual	bool	min_max_info(int nStart, int nEnd, float *pdMin, float *pdMax)  override  ;
    virtual	bool	calc(float* pdMA, float* pdUp, float * pdDown, int nIndex, bool bUseLast)  override ;
};
#endif
