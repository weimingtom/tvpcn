
/*****************************************************************************
                         E R I N A - L i b r a r y
                                                      ��K���� 2001/05/29
 ----------------------------------------------------------------------------
         Copyright (C) 2000-2001 Leshade Entis. All rights reserved.
 *****************************************************************************/


#if	!defined(ERIMATRIX_H_INCLUDE)
#define	ERIMATRIX_H_INCLUDE


/*****************************************************************************
                            DCT���������ö���
 *****************************************************************************/

#define	MIN_DCT_DEGREE	2
#define	MAX_DCT_DEGREE	12


/*****************************************************************************
                              �g������v��
 *****************************************************************************/

extern	"C"
{
	int eriRoundR32ToInt( REAL32 r ) ;
	void eriRoundR32ToWordArray
		( SWORD * ptrDst, int nStep, const REAL32 * ptrSrc, int nCount ) ;
} ;


/*****************************************************************************
                             ��ܞ���И�����
 *****************************************************************************/

struct	SIN_COS
{
	REAL32	rSin ;
	REAL32	rCos ;
} ;


/*****************************************************************************
                             DCT���������v��
 *****************************************************************************/

extern	"C"
{
	void eriInitializeMatrixDCT( void ) ;

	void eriScalarMultiply
		(
			REAL32 *		ptrDst,
			REAL32			rScalar,
			unsigned int	nCount
		) ;

	bool eriFastDCT
		(
			REAL32 *		ptrDst,
			unsigned int	nDstInterval,
			REAL32 *		ptrSrc,
			REAL32 *		ptrWorkBuf,
			unsigned int	nDegreeDCT
		) ;

	bool eriFastIDCT
		(
			REAL32 *		ptrDst,
			REAL32 *		ptrSrc,
			unsigned int	nSrcInterval,
			REAL32 *		ptrWorkBuf,
			unsigned int	nDegreeDCT
		) ;

	SIN_COS * eriCreateRevolveParameter
		(
			unsigned int nDegreeDCT
		) ;

	void eriFastPLOT
		(
			REAL32 *		ptrSrc,
			unsigned int	nDegreeDCT
		) ;

	void eriFastLOT
		(
			REAL32 *		ptrDst,
			const REAL32 *	ptrSrc1,
			const REAL32 *	ptrSrc2,
			unsigned int	nDegreeDCT
		) ;

	void eriOddGivensMatrix
		(
			REAL32 *		ptrDst,
			const SIN_COS *	ptrRevolve,
			unsigned int	nDegreeDCT
		) ;

	void eriOddGivensInverseMatrix
		(
			REAL32 *		ptrSrc,
			const SIN_COS *	ptrRevolve,
			unsigned int	nDegreeDCT
		) ;

	void eriFastIPLOT
		(
			REAL32 *		ptrSrc,
			unsigned int	nDegreeDCT
		) ;

	void eriFastILOT
		(
			REAL32 *		ptrDst,
			const REAL32 *	ptrSrc1,
			const REAL32 *	ptrSrc2,
			unsigned int	nDegreeDCT
		) ;

	void eriSquareDCT
		(
			REAL32 *		ptrDst,
			REAL32 *		ptrSrc,
			unsigned int	nDegreeDCT,
			REAL32 *		ptrWorkBuf1,
			REAL32 *		ptrWorkBuf2
		) ;

	void eriSquareIDCT
		(
			REAL32 *		ptrDst,
			REAL32 *		ptrSrc,
			unsigned int	nDegreeDCT,
			REAL32 *		ptrWorkBuf1,
			REAL32 *		ptrWorkBuf2
		) ;

} ;


#endif
