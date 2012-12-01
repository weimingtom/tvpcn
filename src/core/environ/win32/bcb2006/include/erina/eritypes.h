
//////////////////////////////////////////////////////////////////////////////
// 恵理ちゃん データ型定義ファイル                    (最終更新 2000/03/25)
//////////////////////////////////////////////////////////////////////////////

//
// 必要なデータ型やテキストマクロを定義します
//////////////////////////////////////////////////////////////////////////////
// コンパイルに必要なデータ型；
//		BYTE	: 符号無し8ビット整数
//		WORD	: 符号無し16ビット整数
//		SWORD	: 符号あり16ビット整数
//		DWORD	: 符号無し32ビット整数
//		SDWORD	: 符号有り32ビット整数
//		INT		: 符号有り16ビット以上整数
//		UINT	: 符号無し16ビット以上整数
//		LONG	: 符号有り32ビット以上整数
//		ULONG	: 符号無し32ビット以上整数
//		UINT64	: 符号無し64ビット整数
//		PVOID	: void 型へのポインタ
//		PBYTE	: 符号無し8ビット整数へのポインタ
//		PWORD	: 符号無し16ビット整数へのポインタ
//		PINT	: 符号有り16ビット以上整数へのポインタ
//		REAL32	: 32ビット浮動小数点


#if	!defined(ERITYPES_H_INCLUDED)
#define	ERITYPES_H_INCLUDED

#if	!defined(_WINDOWS_)

typedef	void *					PVOID ;
typedef	unsigned char			BYTE, * PBYTE ;
typedef	unsigned short int		WORD, * PWORD ;
typedef	unsigned long int		DWORD ;
typedef	signed int				INT, * PINT ;
typedef	unsigned int			UINT ;
typedef	signed long int			LONG, * PLONG ;
typedef	unsigned long int		ULONG ;

#endif

typedef	signed char				SBYTE ;
typedef	signed short int		SWORD ;
typedef	signed long int			SDWORD ;
typedef	unsigned __int64		UINT64 ;
typedef	float					REAL32 ;

#endif
