
//////////////////////////////////////////////////////////////////////////////
// �{������ �ǩ`���Ͷ��x�ե�����                    (��K���� 2000/03/25)
//////////////////////////////////////////////////////////////////////////////

//
// ��Ҫ�ʥǩ`���ͤ�ƥ����ȥޥ�����x���ޤ�
//////////////////////////////////////////////////////////////////////////////
// ����ѥ���˱�Ҫ�ʥǩ`���ͣ�
//		BYTE	: ���şo��8�ӥå�����
//		WORD	: ���şo��16�ӥå�����
//		SWORD	: ���Ť���16�ӥå�����
//		DWORD	: ���şo��32�ӥå�����
//		SDWORD	: �����Ф�32�ӥå�����
//		INT		: �����Ф�16�ӥå���������
//		UINT	: ���şo��16�ӥå���������
//		LONG	: �����Ф�32�ӥå���������
//		ULONG	: ���şo��32�ӥå���������
//		UINT64	: ���şo��64�ӥå�����
//		PVOID	: void �ͤؤΥݥ���
//		PBYTE	: ���şo��8�ӥå������ؤΥݥ���
//		PWORD	: ���şo��16�ӥå������ؤΥݥ���
//		PINT	: �����Ф�16�ӥå����������ؤΥݥ���
//		REAL32	: 32�ӥåȸ���С����


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
