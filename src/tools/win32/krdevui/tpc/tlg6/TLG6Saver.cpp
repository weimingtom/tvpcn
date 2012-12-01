 //---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <stdio.h>

#include "TLG6Saver.h"
#include "TLG6BS.h"
#include "GolombTable.h"

#include "slide.h"

#define MAX_COLOR_COMPONENTS 4

#define FILTER_TRY_COUNT 16

#define W_BLOCK_SIZE 8
#define H_BLOCK_SIZE 8

//------------------------------ FOR DEBUG
//#define FILTER_TEST
//#define WRITE_ENTROPY_VALUES
//#define WRITE_VSTXT
//------------------------------


/*
	shift-jis

	TLG6 �R�s���르�ꥺ��(��Ҫ)

	TLG6 �ϼ��Ｊ����ä����뻭��R�s�ե��`�ޥåȤǤ�������`�����`�롢
	24bit�ӥåȥޥåס�8bit����ե������ͥ븶��24bit�ӥåȥޥåפˌ��ꤷ
	�Ƥ��ޤ����ǥ�����Υ��󥻥ץȤϡ��߈R�s�ʤǤ��뤳�ȡ�������չ�_�ˤ�����
	ʮ�֤˸��٤ʌgװ���Ǥ��뤳�ȡ��Ǥ���

	TLG6 �����¤����ǻ����R�s���ޤ���

	1.   MED �ޤ��� ƽ���� �ˤ���x����y
	2.   �ꥪ�`����󥰤ˤ����������p
	3.   ɫ���v�ե��륿�ˤ����������p
	4.   �����饤�����Ťˤ�륨��ȥ�ԩ`���Ż�



	1.   MED �ޤ��� ƽ���� �ˤ���x����y

		TLG6 �� MED (Median Edge Detector) ���뤤��ƽ�����ˤ���x����y����
		������y���Ȍg�H�΂����`���ӛ�h���ޤ���

		MED �ϡ���y����Ȥ���ԥ�����λ��(��xλ��)�ˌ��������O�Υԥ�����
		���x�Ȥ�a�����O�Υԥ�������x�Ȥ�b�����ϤΥԥ�������x�Ȥ�c�Ȥ�����
		�y����Υԥ�������x�� x �����¤Τ褦����y���ޤ���

			+-----+-----+
			|  c  |  b  |
			+-----+-----+
			|  a  |  x  |
			+-----+-----+

			x = min(a,b)    (c > max(a,b) �Έ���)
			x = max(a,b)    (c < min(a,b) �Έ���)
			x = a + b - c   (�������Έ���)

		MED�ϡ��g���ʡ��܇�Υԥ�������x�Ȥ�ƽ���ˤ����y�ʤɤȱȤ١�����
		�οk����Υ��å��Ǥ��ϤΥԥ�����򡢺᷽��Υ��å��Ǥ���Υԥ������
		���դ�����y���Ф��褦�ˤʤ뤿�ᡢ���ʤΤ褤��y�����ܤǤ���

		ƽ�����Ǥϡ�a �� b ���x�Ȥ�ƽ��������y����Υԥ�������x�� x ����
		�y���ޤ������¤�ʽ���ä��ޤ���

			x = (a + b + 1) >> 1

		MED �� ƽ�����ϡ�8x8 �����Ф�줿����֥�å��ڤǁI���ˤ��R�s�ʤ�
		���^�����R�s�ʤθߤ��ä����η������x�k����ޤ���

		R G B ����� A R G B ����ˌ����ƤϤ����ɫ����ݩ`�ͥ�Ȥ��Ȥ��Ф�
		�ޤ���

	2.   �ꥪ�`����󥰤ˤ����������p

		�`��ΤߤȤʤä��ǩ`���ϡ��ꥪ�`�����(�K���椨)���Ф��ޤ���
		TLG6�Ǥϡ��ޤ������8��8��С���ʥ֥�å��˷ָ�ޤ����֥�å��Ϻ�
		����ˡ���Υ֥�å�����혤��ҤΥ֥�å����򤫤���һ�з֤Υ֥�å�
		�΄I�����ˤ���С�����һ���¤Υ֥�å����Ȥ�������ǄI����Ф���
		����
		���΂����Υ֥�å��ڤǤϡ��ԥ���������¤�혷��ˁK���椨�ޤ���

		��λ�ä�ż���Υ֥�å�      ��λ�ä������Υ֥�å�
		 1  2  3  4  5  6  7  8     57 58 59 60 61 62 63 64
		16 15 14 13 12 11 10  9     56 55 54 53 52 51 50 49
		17 18 19 20 21 22 23 24     41 42 43 44 45 46 47 48
		32 31 30 29 28 27 26 25     40 39 38 37 36 35 34 33
		33 34 35 36 37 38 39 40     25 26 27 28 29 30 31 32
		48 47 46 45 44 43 42 41     24 23 22 21 20 19 18 17
		49 50 51 52 53 54 55 56      9 10 11 12 13 14 15 16
		64 63 62 61 60 59 58 57      8  7  6  5  4  3  2  1

		���Τ褦�ʡ�����������״�΁K���椨���Ф����Ȥˤ�ꡢ���|(�x�Ȥ�ɫ�ࡢ
		���å��ʤ�)���Ƥ��ԥ����뤬�B�A��������Ԥ��ߤ��ʤꡢ��ΤΥ���ȥ�
		�ԩ`���Ż��ΤǤΥ������󥰥��R�s�΄�����ߤ�뤳�Ȥ��Ǥ��ޤ���

	3.   ɫ���v�ե��륿�ˤ����������p

		R G B ���� ( A R G B �����ͬ���� ) �ˤ����Ƥϡ�ɫ����ݩ`�ͥ���g
		�����v�Ԥ����ä����������p�餻������Ԥ�����ޤ���TLG6�Ǥϡ����¤�
		16�NΥե��륿��֥�å����Ȥ��m�ä�����ΤΥ����饤�����Ťˤ�
		��R�s�᥵������ԇ�㤷����äȤ⥵������С�����ʤ�ե��륿���m�ä���
		�����ե��륿���Ť� MED/ƽ�����΄e�� LZSS ���ˤ��R�s���졢��������
		�ޤ���

		0          B        G        R              (�ե��륿�o��)
		1          B-G      G        R-G
		2          B        G-B      R-B-G
		3          B-R-G    G-R      R
		4          B-R      G-B-R    R-B-R-G
		5          B-R      G-B-R    R
		6          B-G      G        R
		7          B        G-B      R
		8          B        G        R-G
		9          B-G-R-B  G-R-B    R-B
		10         B-R      G-R      R
		11         B        G-B      R-B
		12         B        G-R-B    R-B
		13         B-G      G-R-B-G  R-B-G
		14         B-G-R    G-R      R-B-G-R
		15         B        G-(B<<1) R-(B<<1)

		�����Υե��륿�ϡ��य�λ���ǥƥ��Ȥ����С��ե��륿��ʹ���l�Ȥ��{
		�١���äȤ��m�äζफ�ä�16���Υե��륿���x�����ޤ�����

	4.   �����饤�����Ťˤ�륨��ȥ�ԩ`���Ż�

		��K�Ĥʂ��ϥ����饤�����Ťˤ��ӥå����Ф˸�{����ޤ���������
		��K�Ĥʂ��ˤ��B�A���를���ǳ��˶त���ᡢ����ȷǥ�����B�A����
		����޷��Ť��ä��Ʒ��Ż����ޤ�(�������󥰥�)�����Τ��ᡢ�ӥ�
		�������Фˤϻ����Ĥ�

		1 �ǥ�����B�A��(����󥰥�)�Υ���޷���
		2 �ǥ���΂��Υ����饤������ (�B�A���� �R�귵�����)
		3 ������B�A��(����󥰥�)�Υ���޷���

		���}���جF��뤳�Ȥˤʤ�ޤ���

		����޷��Ť����¤���ʽ�ǡ�0�򱣴椹���Ҫ���ʤ�����(�L��0�Ϥ���ä�
		���Τ�)����F�Ǥ�����ͤ΂���1�ȤʤäƤ��ޤ���

		                  MSB ��    �� LSB
		(v=1)                            1
		(2<=v<=3)                      x10          x = v-2
		(4<=v<=7)                    xx100         xx = v-4
		(8<=v<=15)                 xxx1000        xxx = v-8
		(16<=v<=31)             x xxx10000       xxxx = v-16
		(32<=v<=63)           xxx xx100000      xxxxx = v-32
		(64<=v<=127)        xxxxx x1000000     xxxxxx = v-64
		(128<=v<=255)     xxxxxxx 10000000    xxxxxxx = v-128
		(256<=v<=511)    xxxxxxx1 00000000   xxxxxxxx = v-256
		      :                    :                  :
		      :                    :                  :

		�����饤�����Ť����¤η����Ƿ��Ż����ޤ����ޤ������Ż����褦�Ȥ�
		�낎��e�Ȥ��ޤ���e�ˤ�1���Ϥ�����������-1���¤�ؓ�����������ڤ��ޤ���
		�����0���Ϥ������ˉ�Q���ޤ�������Ĥˤ����¤�ʽ�ǉ�Q���Ф��ޤ���
		(��Q��΂���m�Ȥ���)

		m = ((e >= 0) ? 2*e : -2*e-1) - 1

		���Ή�Q�ˤ�ꡢ�¤α�Τ褦�ˡ�ؓ��e��ż����m�ˡ�����e��������m�ˉ�
		�Q����ޤ���

		 e     m
		-1     0
		 1     1
		-2     2
		 2     3
		-3     4
		 3     5
		 :     :
		 :     :

		���΂� m ���ɤ줰�餤�Υӥå��L�Ǳ�F�Ǥ��뤫����y���ޤ������Υӥ�
		���L�� k �Ȥ��ޤ���k �� 0 ���Ϥ������Ǥ�����������y���Ϥ��졢m �� k
		�ӥåȤǱ�F�Ǥ��ʤ����Ϥ�����ޤ������Έ��Ϥϡ�k �ӥåȤǱ�F�Ǥ�
		�ʤ��ä��֤� m���Ĥޤ� k>>m ���֤Υ���������������ˤ��νK��ӛ�Ť�
		���� 1 ��������ޤ���
		m �� k�ӥåȤǱ�F�Ǥ������Ϥϡ������g�� 1 ��������ޤ������Τ��ȡ�
		m ����λ�ӥåȤ��� k �ӥåȷֳ������ޤ���

		���Ȥ��� e �� 66 �� k �� 5 �Έ��Ϥ����¤Τ褦�ˤʤ�ޤ���

		m �� m = ((e >= 0) ? 2*e : -2*e-1) - 1 ��ʽ�ˤ�� 131 �Ȥʤ�ޤ���
		��y�����ӥå��� 5 �Ǳ�F�Ǥ��낎����󂎤� 31 �Ǥ��Τǡ�131 �Ǥ���
		m �� k �ӥåȤǱ�F�Ǥ��ʤ��Ȥ������Ȥˤʤ�ޤ����Ǥ��Τǡ�m >> k
		�Ǥ��� 4 ���� 0 ��������ޤ���

		0000

		�Τˡ�����4���� 0 �νK��ӛ�ŤȤ��� 1 ��������ޤ���

		10000

		����� m ����λ�ӥåȤ��� k �ӥåȷֳ������ޤ���131 ���M���Ǳ�F
		����� 10000011 �Ȥʤ�ޤ������������λ���� 5 �ӥåȷ֤�������ޤ���


        MSB��  ��LSB
		00011 1 0000      (Ӌ10�ӥå�)
		~~~~~ ~ ~~~~
		  c   b   a

		a   (m >> k) ���� 0
		b   a �� �K��ӛ��
		c   m ����λ k �ӥåȷ�

		���줬 e=66 k=5 ����Ż������ӥå��ФȤʤ�ޤ���

		ԭ��Ĥˤ� k ����y�ϡ���ޤǤ˳������� e �ν~�����κ�Ӌ�Ȥ���ޤǤ�
		��������������Ԫ�ˡ����餫����g�H�λ���νyӋ�������ɤ��줿�Ʃ`�֥�
		����դ��뤳�Ȥ�������ޤ��������� C++ ���Z�L��ӛ����������¤Τ褦
		�ˤʤ�ޤ���

		n = 3; // ������
		a = 0; // e �ν~�����κ�Ӌ

		while(e ���i���z��)
		{
			k = table[a][n]; // �Ʃ`�֥뤫�� k �����
			m = ((e >= 0) ? 2*e : -2*e-1) - 1;
			e �� m �� k �Ƿ��Ż�;
			a += m >> 1; // e �ν~�����κ�Ӌ (������ m >>1 �Ǵ���)
			if(--n < 0) a >>= 1, n = 3;
		}

		4 �ؤ��Ȥ� a ����p���� n ���ꥻ�åȤ���뤿�ᡢǰ�΂���Ӱ푤Ϸ��Ż�
		���M��ˤĤ�Ʊ����ʤꡢֱ���΂����鄿�ʵĤ� k ����y���뤳�Ȥ��Ǥ�
		�ޤ���


	�ο� :
		Golomb codes
		http://oku.edu.mie-u.ac.jp/~okumura/compression/golomb/
		���ش�ѧ���� �´������Ϥˤ�르�����Ťκ��g�ʽ��h
*/

//---------------------------------------------------------------------------
#ifdef WRITE_VSTXT
FILE *vstxt = fopen("vs.txt", "wt");
#endif
#define GOLOMB_GIVE_UP_BYTES 4
void CompressValuesGolomb(TLG6BitStream &bs, char *buf, int size)
{
	// golomb encoding, -- http://oku.edu.mie-u.ac.jp/~okumura/compression/golomb/

	// run-length golomb method
	bs.PutValue(buf[0]?1:0, 1); // initial value state

	int count;

	int n = TVP_TLG6_GOLOMB_N_COUNT - 1; // �����Υ�����
	int a = 0; // ��y�`��ν~�����κ�

	count = 0;
	for(int i = 0; i < size; i++)
	{
		if(buf[i])
		{
			// write zero count
			if(count) bs.PutGamma(count);

			// count non-zero values
			count = 0;
			int ii;
			for(ii = i; ii < size; ii++)
			{
				if(buf[ii]) count++; else break;
			}

			// write non-zero count
			bs.PutGamma(count);

			// write non-zero values
			for(; i < ii; i++)
			{
				int e = buf[i];
#ifdef WRITE_VSTXT
				fprintf(vstxt, "%d ", e);
#endif
				int k = TVPTLG6GolombBitLengthTable[a][n];
				int m = ((e >= 0) ? 2*e : -2*e-1) - 1;
				int store_limit = bs.GetBytePos() + GOLOMB_GIVE_UP_BYTES;
				bool put1 = true;
				for(int c = (m >> k); c > 0; c--)
				{
					if(store_limit == bs.GetBytePos())
					{
						bs.PutValue(m >> k, 8);
#ifdef WRITE_VSTXT
						fprintf(vstxt, "[ %d ] ", m>>k);
#endif
						put1 = false;
						break;
					}
					bs.Put1Bit(0);
				}
				if(store_limit == bs.GetBytePos())
				{
					bs.PutValue(m >> k, 8);
#ifdef WRITE_VSTXT
					fprintf(vstxt, "[ %d ] ", m>>k);
#endif
					put1 = false;
				}
				if(put1) bs.Put1Bit(1);
				bs.PutValue(m, k);
				a += (m>>1);
				if (--n < 0) {
					a >>= 1; n = TVP_TLG6_GOLOMB_N_COUNT - 1;
				}
			}

#ifdef WRITE_VSTXT
			fprintf(vstxt, "\n");
#endif

			i = ii - 1;
			count = 0;
		}
		else
		{
			// zero
			count ++;
		}
	}

	if(count) bs.PutGamma(count);
}
//---------------------------------------------------------------------------
class TryCompressGolomb
{
	int TotalBits; // total bit count
	int Count; // running count
	int N;
	int A;
	bool LastNonZero;

public:
	TryCompressGolomb()
	{
		Reset();
	}

	TryCompressGolomb(const TryCompressGolomb &ref)
	{
		Copy(ref);
	}

	~TryCompressGolomb() { ; }

	void Copy(const TryCompressGolomb &ref)
	{
		TotalBits = ref.TotalBits;
		Count = ref.Count;
		N = ref.N;
		A = ref.A;
		LastNonZero = ref.LastNonZero;
	}

	void Reset()
	{
		TotalBits = 1;
		Count = 0;
		N = TVP_TLG6_GOLOMB_N_COUNT - 1;
		A = 0;
		LastNonZero = false;
	}

	int Try(char *buf, int size)
	{
		for(int i = 0; i < size; i++)
		{
			if(buf[i])
			{
				// write zero count
				if(!LastNonZero)
				{
					if(Count)
						TotalBits +=
							TLG6BitStream::GetGammaBitLength(Count);

					// count non-zero values
					Count = 0;
				}

				// write non-zero values
				for(; i < size; i++)
				{
					int e = buf[i];
					if(!e) break;
					Count ++;
					int k = TVPTLG6GolombBitLengthTable[A][N];
					int m = ((e >= 0) ? 2*e : -2*e-1) - 1;
					int unexp_bits = (m>>k);
					if(unexp_bits >= (GOLOMB_GIVE_UP_BYTES*8-8/2))
						unexp_bits = (GOLOMB_GIVE_UP_BYTES*8-8/2)+8;
					TotalBits += unexp_bits + 1 + k;
					A += (m>>1);
					if (--N < 0) {
						A >>= 1; N = TVP_TLG6_GOLOMB_N_COUNT - 1;
					}
				}

				// write non-zero count

				i--;
				LastNonZero = true;
			}
			else
			{
				// zero
				if(LastNonZero)
				{
					if(Count)
					{
						TotalBits += TLG6BitStream::GetGammaBitLength(Count);
						Count = 0;
					}
				}

				Count ++;
				LastNonZero = false;
			}
		}
		return TotalBits;
	}

	int Flush()
	{
		if(Count)
		{
			TotalBits += TLG6BitStream::GetGammaBitLength(Count);
			Count = 0;
		}
		return TotalBits;
	}
};
//---------------------------------------------------------------------------
#define DO_FILTER \
	len -= 4; \
	for(d = 0; d < len; d+=4) \
	{ FILTER_FUNC(0); FILTER_FUNC(1); FILTER_FUNC(2); FILTER_FUNC(3); } \
	len += 4; \
	for(; d < len; d++) \
	{ FILTER_FUNC(0); }

void ApplyColorFilter(char * bufb, char * bufg, char * bufr, int len, int code)
{
	int d;
	unsigned char t;
	switch(code)
	{
	case 0:
		break;
	case 1:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufg[d+n], \
			bufb[d+n] -= bufg[d+n];
		DO_FILTER
		break;
	case 2:
		for(int d = 0; d < len; d++)
			bufr[d] -= bufg[d],
			bufg[d] -= bufb[d];
		break;
	case 3:
		for(int d = 0; d < len; d++)
			bufb[d] -= bufg[d],
			bufg[d] -= bufr[d];
		break;
	case 4:
		for(int d = 0; d < len; d++)
			bufr[d] -= bufg[d],
			bufg[d] -= bufb[d],
			bufb[d] -= bufr[d];
		break;
	case 5:
		for(int d = 0; d < len; d++)
			bufg[d] -= bufb[d],
			bufb[d] -= bufr[d];
		break;
	case 6:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			bufb[d+n] -= bufg[d+n];
		DO_FILTER
		break;
	case 7:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufb[d+n];
		DO_FILTER
		break;
	case 8:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufg[d+n];
		DO_FILTER
		break;
	case 9:
		for(int d = 0; d < len; d++)
			bufb[d] -= bufg[d],
			bufg[d] -= bufr[d],
			bufr[d] -= bufb[d];
		break;
	case 10:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufr[d+n], \
			bufb[d+n] -= bufr[d+n];
		DO_FILTER
		break;
	case 11:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufb[d+n], \
			bufg[d+n] -= bufb[d+n];
		DO_FILTER
		break;
	case 12:
		for(int d = 0; d < len; d++)
			bufg[d] -= bufr[d],
			bufr[d] -= bufb[d];
		break;
	case 13:
		for(int d = 0; d < len; d++)
			bufg[d] -= bufr[d],
			bufr[d] -= bufb[d],
			bufb[d] -= bufg[d];
		break;
	case 14:
		for(int d = 0; d < len; d++)
			bufr[d] -= bufb[d],
			bufb[d] -= bufg[d],
			bufg[d] -= bufr[d];
		break;
	case 15:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			t = bufb[d+n]<<1; \
			bufr[d+n] -= t, \
			bufg[d+n] -= t;
		DO_FILTER
		break;
	case 16:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufr[d+n];
		DO_FILTER
		break;
	case 17:
		for(int d = 0; d < len; d++)
			bufr[d] -= bufb[d],
			bufb[d] -= bufg[d];
		break;
	case 18:
		for(int d = 0; d < len; d++)
			bufr[d] -= bufb[d];
		break;
	case 19:
		for(int d = 0; d < len; d++)
			bufb[d] -= bufr[d],
			bufr[d] -= bufg[d];
		break;
	case 20:
		for(int d = 0; d < len; d++)
			bufb[d] -= bufr[d];
		break;
	case 21:
		for(int d = 0; d < len; d++)
			bufb[d] -= bufg[d]>>1;
		break;
	case 22:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufb[d+n]>>1;
		DO_FILTER
		break;
	case 23:
		for(int d = 0; d < len; d++)
			bufg[d] -= bufb[d],
			bufb[d] -= bufr[d],
			bufr[d] -= bufg[d];
		break;
	case 24:
		for(int d = 0; d < len; d++)
			bufb[d] -= bufr[d],
			bufr[d] -= bufg[d],
			bufg[d] -= bufb[d];
		break;
	case 25:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufr[d+n]>>1;
		DO_FILTER
		break;
	case 26:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufg[d+n]>>1;
		DO_FILTER
		break;
	case 27:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			t = bufr[d+n]>>1; \
			bufg[d+n] -= t, \
			bufb[d+n] -= t;
		DO_FILTER
		break;
	case 28:
		for(int d = 0; d < len; d++)
			bufr[d] -= bufb[d]>>1;
		break;
	case 29:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			t = bufg[d+n]>>1; \
			bufr[d+n] -= t, \
			bufb[d+n] -= t;
		DO_FILTER
		break;
	case 30:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			t = bufb[d+n]>>1; \
			bufr[d+n] -= t, \
			bufg[d+n] -= t;
		DO_FILTER
		break;
	case 31:
		for(int d = 0; d < len; d++)
			bufb[d] -= bufr[d]>>1;
		break;
	case 32:
		for(int d = 0; d < len; d++)
			bufr[d] -= bufb[d]<<1;
		break;
	case 33:
		for(int d = 0; d < len; d++)
			bufb[d] -= bufg[d]<<1;
		break;
	case 34:
		#undef FILTER_FUNC
		#define FILTER_FUNC(n) \
			t = bufr[d+n]<<1; \
			bufg[d+n] -= t, \
			bufb[d+n] -= t;
		DO_FILTER
		break;
	case 35:
		for(int d = 0; d < len; d++)
			bufg[d] -= bufb[d]<<1;
		break;
	case 36:
		for(int d = 0; d < len; d++)
			bufr[d] -= bufg[d]<<1;
		break;
	case 37:
		for(int d = 0; d < len; d++)
			bufr[d] -= bufg[d]<<1,
			bufb[d] -= bufg[d]<<1;
		break;
	case 38:
		for(int d = 0; d < len; d++)
			bufg[d] -= bufr[d]<<1;
		break;
	case 39:
		for(int d = 0; d < len; d++)
			bufb[d] -= bufr[d]<<1;
		break;

	}

}
//---------------------------------------------------------------------------
int DetectColorFilter(char *b, char *g, char *r, int size, int &outsize)
{
#ifndef FILTER_TEST
	int minbits = -1;
	int mincode = -1;

	char bbuf[H_BLOCK_SIZE*W_BLOCK_SIZE];
	char gbuf[H_BLOCK_SIZE*W_BLOCK_SIZE];
	char rbuf[H_BLOCK_SIZE*W_BLOCK_SIZE];
	TryCompressGolomb bc, gc, rc;

	for(int code = 0; code < FILTER_TRY_COUNT; code++)   // 17..27 are currently not used
	{
		// copy bbuf, gbuf, rbuf into b, g, r.
		memcpy(bbuf, b, sizeof(char)*size);
		memcpy(gbuf, g, sizeof(char)*size);
		memcpy(rbuf, r, sizeof(char)*size);

		// copy compressor
		bc.Reset();
		gc.Reset();
		rc.Reset();

		// Apply color filter
		ApplyColorFilter(bbuf, gbuf, rbuf, size, code);

		// try to compress
		int bits;
		bits  = (bc.Try(bbuf, size), bc.Flush());
		if(minbits != -1 && minbits < bits) continue;
		bits += (gc.Try(gbuf, size), gc.Flush());
		if(minbits != -1 && minbits < bits) continue;
		bits += (rc.Try(rbuf, size), rc.Flush());

		if(minbits == -1 || minbits > bits)
		{
			minbits = bits, mincode = code;
		}
	}

	outsize = minbits;

	return mincode;
#else
	static int filter = 0;

	int f = filter;

	filter++;
	if(filter >= FILTER_TRY_COUNT) filter = 0;

	outsize = 0;

	return f;
#endif
}
//---------------------------------------------------------------------------
static void WriteInt32(long num, TStream *out)
{
	char buf[4];
	buf[0] = num & 0xff;
	buf[1] = (num >> 8) & 0xff;
	buf[2] = (num >> 16) & 0xff;
	buf[3] = (num >> 24) & 0xff;
	out->WriteBuffer(buf, 4);
}
//---------------------------------------------------------------------------
static void TLG6InitializeColorFilterCompressor(SlideCompressor &c)
{
	unsigned char code[4096];
	unsigned char dum[4096];
	unsigned char *p = code;
	for(int i = 0; i < 32; i++)
	{
		for(int j = 0; j < 16; j++)
		{
			p[0] = p[1] = p[2] = p[3] = i;
			p += 4;
			p[0] = p[1] = p[2] = p[3] = j;
			p += 4;
		}
	}

	long dumlen;
	c.Encode(code, 4096, dum, dumlen);
}
//---------------------------------------------------------------------------
// int ftfreq[256] = {0};
void SaveTLG6(Graphics::TBitmap *bmp, TStream *out)
{
	DWORD medstart, medend;
	DWORD reordertick;
#ifdef WRITE_ENTROPY_VALUES
	FILE *vs = fopen("vs.bin", "wb");
#endif

	int colors;

	TVPTLG6InitGolombTable();

	// check pixel format
	switch(bmp->PixelFormat)
	{
	case pfDevice:		bmp->PixelFormat = pf24bit;		break;
	case pf1bit:		bmp->PixelFormat = pf8bit;		break;
	case pf4bit:		bmp->PixelFormat = pf8bit;		break;
	case pf8bit:										break;
	case pf15bit:		bmp->PixelFormat = pf24bit;		break;
	case pf16bit:		bmp->PixelFormat = pf24bit;		break;
	case pf24bit:										break;
	case pf32bit:										break;
	default: throw Exception("SaveTLG6: Unsupported Color Type");
	}

	switch(bmp->PixelFormat)
	{
	case pf8bit:		colors = 1;		break;
	case pf24bit:		colors = 3;		break;
	case pf32bit:		colors = 4;		break;
	}

	// output stream header
	{
		out->WriteBuffer("TLG6.0\x00raw\x1a\x00", 11);
		out->WriteBuffer(&colors, 1);
		int n = 0;
		out->WriteBuffer(&n, 1); // data flag (0)
		out->WriteBuffer(&n, 1); // color type (0)
		out->WriteBuffer(&n, 1); // external golomb table (0)
		int width = bmp->Width;
		int height = bmp->Height;
		WriteInt32(width, out);
		WriteInt32(height, out);
	}


/*
	// Near lossless filter
	if(colors == 3) bmp->PixelFormat = pf32bit;
	NearLosslessFilter(bmp);
	if(colors == 3) bmp->PixelFormat = pf24bit;
*/

	// compress
	long max_bit_length = 0;

	unsigned char *buf[MAX_COLOR_COMPONENTS];
	for(int i = 0; i < MAX_COLOR_COMPONENTS; i++) buf[i] = NULL;
	char *block_buf[MAX_COLOR_COMPONENTS];
	for(int i = 0; i < MAX_COLOR_COMPONENTS; i++) block_buf[i] = NULL;
	unsigned char *filtertypes = NULL;
	TMemoryStream *memstream = NULL;

	try
	{
		memstream = new TMemoryStream();

		TLG6BitStream bs(memstream);

//		int buf_size = bmp->Width * bmp->Height;

		// allocate buffer
		for(int c = 0; c < colors; c++)
		{
			buf[c] = new unsigned char [W_BLOCK_SIZE * H_BLOCK_SIZE * 3];
			block_buf[c] = new char [H_BLOCK_SIZE * bmp->Width];
		}
		int w_block_count = (int)((bmp->Width - 1) / W_BLOCK_SIZE) + 1;
		int h_block_count = (int)((bmp->Height - 1) / H_BLOCK_SIZE) + 1;
		filtertypes = new unsigned char [w_block_count * h_block_count];

		int fc = 0;
		for(int y = 0; y < bmp->Height; y += H_BLOCK_SIZE)
		{
			int ylim = y + H_BLOCK_SIZE;
			if(ylim > bmp->Height) ylim = bmp->Height;
			int gwp = 0;
			int xp = 0;
			for(int x = 0; x < bmp->Width; x += W_BLOCK_SIZE, xp++)
			{
				int xlim = x + W_BLOCK_SIZE;
				if(xlim > bmp->Width) xlim = bmp->Width;
				int bw = xlim - x;

				int p0size; // size of MED method (p=0)
				int minp = 0; // most efficient method (0:MED, 1:AVG)
				int ft; // filter type
				int wp; // write point
				for(int p = 0; p < 2; p++)
				{
					int dbofs = (p+1) * (H_BLOCK_SIZE * W_BLOCK_SIZE);

					// do med(when p=0) or take average of upper and left pixel(p=1)
					for(int c = 0; c < colors; c++)
					{
						int wp = 0;
						for(int yy = y; yy < ylim; yy++)
						{
							const unsigned char * sl = x*colors +
								c + (const unsigned char *)bmp->ScanLine[yy];
							const unsigned char * usl;
							if(yy >= 1)
								usl = x*colors + c + (const unsigned char *)bmp->ScanLine[yy-1];
							else
								usl = NULL;
							for(int xx = x; xx < xlim; xx++)
							{
								unsigned char pa = xx > 0 ? sl[-colors] : 0;
								unsigned char pb = usl ? *usl : 0;
								unsigned char px = *sl;

								unsigned char py;

//								py = 0;
								if(p == 0)
								{
									unsigned char pc = (xx > 0 && usl) ? usl[-colors] : 0;
									unsigned char min_a_b = pa>pb?pb:pa;
									unsigned char max_a_b = pa<pb?pb:pa;

									if(pc >= max_a_b)
										py = min_a_b;
									else if(pc < min_a_b)
										py = max_a_b;
									else
										py = pa + pb - pc;
								}
								else
								{
									py = (pa+pb+1)>>1;
								}
								
								buf[c][wp] = (unsigned char)(px - py);

								wp++;
								sl += colors;
								if(usl) usl += colors;
							}
						}
					}

					// reordering
					// Transfer the data into block_buf (block buffer).
					// Even lines are stored forward (left to right),
					// Odd lines are stored backward (right to left).

					wp = 0;
					DWORD reorderstart = GetTickCount();
					for(int yy = y; yy < ylim; yy++)
					{
						int ofs;
						if(!(xp&1))
							ofs = (yy - y)*bw;
						else
							ofs = (ylim - yy - 1) * bw;
						bool dir; // false for forward, true for backward
						if(!((ylim-y)&1))
						{
							// vertical line count per block is even
							dir = (yy&1) ^ (xp&1);
						}
						else
						{
							// otherwise;
							if(xp & 1)
							{
								dir = (yy&1);
							}
							else
							{
								dir = (yy&1) ^ (xp&1);
							}
						}

						if(!dir)
						{
							// forward
							for(int xx = 0; xx < bw; xx++)
							{
								for(int c = 0; c < colors; c++)
									buf[c][wp + dbofs] =
									buf[c][ofs + xx];
								wp++;
							}
						}
						else
						{
							// backward
							for(int xx = bw - 1; xx >= 0; xx--)
							{
								for(int c = 0; c < colors; c++)
									buf[c][wp + dbofs] =
									buf[c][ofs + xx];
								wp++;
							}
						}
					}
					reordertick += GetTickCount() - reorderstart;
				}


				for(int p = 0; p < 2; p++)
				{
					int dbofs = (p+1) * (H_BLOCK_SIZE * W_BLOCK_SIZE);
					// detect color filter
					int size = 0;
					int ft_;
					if(colors >= 3)
						ft_ = DetectColorFilter(
							buf[0] + dbofs,
							buf[1] + dbofs,
							buf[2] + dbofs, wp, size);
					else
						ft_ = 0;

					// select efficient mode of p (MED or average)
					if(p == 0)
					{
						p0size = size;
						ft = ft_;
					}
					else
					{
						if(p0size >= size)
							minp = 1, ft = ft_;
					}
				}

				// Apply most efficient color filter / prediction method
				wp = 0;
				int dbofs = (minp + 1)  * (H_BLOCK_SIZE * W_BLOCK_SIZE);
				for(int yy = y; yy < ylim; yy++)
				{
					for(int xx = 0; xx < bw; xx++)
					{
						for(int c = 0; c < colors; c++)
							block_buf[c][gwp + wp] = buf[c][wp + dbofs];
						wp++;
					}
				}

				ApplyColorFilter(block_buf[0] + gwp,
					block_buf[1] + gwp, block_buf[2] + gwp, wp, ft);

				filtertypes[fc++] = (ft<<1) + minp;
//				ftfreq[ft]++;
				gwp += wp;
			}

			// compress values (entropy coding)
			for(int c = 0; c < colors; c++)
			{
				int method;
				CompressValuesGolomb(bs, block_buf[c], gwp);
				method = 0;
#ifdef WRITE_ENTROPY_VALUES
				fwrite(block_buf[c], 1, gwp, vs);
#endif
				long bitlength = bs.GetBitLength();
				if(bitlength & 0xc0000000)
					throw Exception("SaveTLG6: Too large bit length (given image may be too large)");
				// two most significant bits of bitlength are
				// entropy coding method;
				// 00 means Golomb method,
				// 01 means Gamma method (implemented but not used),
				// 10 means modified LZSS method (not yet implemented),
				// 11 means raw (uncompressed) data (not yet implemented).
				if(max_bit_length < bitlength) max_bit_length = bitlength;
				bitlength |= (method << 30);
				WriteInt32(bitlength, memstream);
				bs.Flush();
			}

		}


		// write max bit length
		WriteInt32(max_bit_length, out);

		// output filter types
		{
			SlideCompressor comp;
			TLG6InitializeColorFilterCompressor(comp);
			unsigned char *outbuf = new unsigned char[fc * 2];
			try
			{
				long outlen;
				comp.Encode(filtertypes, fc, outbuf, outlen);
				WriteInt32(outlen, out);
				out->WriteBuffer(outbuf, outlen);
			}
			catch(...)
			{
				delete [] outbuf;
				throw;
			}
			delete [] outbuf;
/*
			FILE *f = fopen("ft.txt", "wt");
			int n = 0;
			for(int y = 0; y < h_block_count; y++)
			{
				for(int x = 0; x < w_block_count; x++)
				{
					int t = filtertypes[n++];
					char b;
					if(t & 1) b = 'A'; else b = 'M';
					t >>= 1;
					fprintf(f, "%c%x", b, t);
				}
				fprintf(f, "\n");
			}
			fclose(f);
*/
		}

		// copy memory stream to output stream
		out->CopyFrom(memstream, 0);
	}
	catch(...)
	{
		for(int i = 0; i < MAX_COLOR_COMPONENTS; i++)
		{
			if(buf[i]) delete [] (buf[i]);
			if(block_buf[i]) delete [] (block_buf[i]);
		}
		if(filtertypes) delete [] filtertypes;
		if(memstream) delete memstream;
		throw;
	}

	for(int i = 0; i < MAX_COLOR_COMPONENTS; i++)
	{
		if(buf[i]) delete [] (buf[i]);
		if(block_buf) delete [] (block_buf[i]);
	}
	if(filtertypes) delete [] filtertypes;
	if(memstream) delete memstream;

#ifdef WRITE_ENTROPY_VALUES
	fclose(vs);
#endif
//	printf("med : %d ms\n", medend - medstart);
//	printf("reorder : %d ms\n", reordertick);
/*
	for(int i = 0; i < 256; i++)
	{
		if(ftfreq[i])
		{
			printf("%3d : %d\n", i, ftfreq[i]);
		}
	}
*/
}
//---------------------------------------------------------------------------


