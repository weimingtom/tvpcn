//---------------------------------------------------------------------------
/*
	Risa [�ꤵ]      alias ���Ｊ��3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ��󥰥Хåե���g�F�����Լ��u�ƥ�ץ�`�ȥ��饹
//---------------------------------------------------------------------------
#ifndef RingBufferH
#define RingBufferH

#include <stddef.h>
/*
	��󥰥Хåե�, ring buffer, circular buffer, �h״�Хåե�
*/

//---------------------------------------------------------------------------
//! @brief		�̶��L��󥰥Хåե��Όgװ
//---------------------------------------------------------------------------
template <typename T>
class tRisaRingBuffer
{
	T * Buffer; //!< �Хåե�
	size_t Size; //!< �Хåե��Υ�����
	size_t WritePos; //!< �����z��λ��
	size_t ReadPos; //!< �i���z��λ��
	size_t DataSize; //!< �Хåե�����äƤ���ǩ`���Υ�����

public:
	//! @brief ���󥹥ȥ饯��
	tRisaRingBuffer(size_t size)
	{
		Size = size;
		Buffer = new T[Size];
		WritePos = ReadPos = 0;
		DataSize = 0;
	}

	//! @brief �ǥ��ȥ饯��
	~tRisaRingBuffer()
	{
		delete [] Buffer;
	}

	//! @brief	��������ä�
	size_t GetSize() { return Size; }

	//! @brief	�����z��λ�ä�ä�
	size_t GetWritePos() { return WritePos; }

	//! @brief	�i���z��λ�ä�ä�
	size_t GetReadPos() { return ReadPos; }

	//! @brief	�Хåե�����äƤ���ǩ`���Υ�������ä�
	size_t GetDataSize() { return DataSize; }

	//! @brief	�Хåե��οդ�������ä�
	size_t GetFreeSize() { return Size - DataSize; }

	//! @brief	�Хåե������i���z�ि��Υݥ��󥿤�ä�
	//! @param	readsize �i���z�ߤ����ǩ`���� ( 1 ���Ϥ�����; 0 ��ɤ��ʤ����� )
	//! @param	p1		�֥�å�1�����^�ؤΥݥ��󥿤��{���뤿��Ή���
	//! @param	p1size	p1�α��֥�å��Υ�����
	//! @param	p2		�֥�å�2�����^�ؤΥݥ��󥿤��{���뤿��Ή���(NULL������ä�)
	//! @param	p2size	p2�α��֥�å��Υ�����(0������ä�)
	//! @param	offset	ReadPos �˼��㤵��륪�ե��å�
	//! @note	�h״�Хåե��Ȥ��äƤ⡢�g�H�ϥ�˥����I��˥Хåե����_������Ƥ��롣
	//!			���Τ��ᡢ ReadPos + readsize ���Хåե��νK�ˤ򳬤��Ƥ�����ϡ��ä���
	//!			�֥�å��ϣ��Ĥ˷ֶϤ���뤳�Ȥˤʤ롣
	//!			���Υ᥽�åɤϡ�readsize���g�H�˥Хåե�����äƤ���ǩ`���Υ��������¤Ǥ��뤫
	//!			�ʤɤΥ����å��Ϥ��ä����Ф�ʤ�����ǰ�� GetDataSize ���{�١��i���z�ߤ���
	//!			���������g�H�˥Хåե��ˤ��뤫�ɤ���������å����뤳�ȡ�
	void GetReadPointer(size_t readsize,
						const T * & p1, size_t &p1size,
						const T * & p2, size_t &p2size,
						ptrdiff_t offset = 0)
	{
		size_t pos = ReadPos + offset;
		while(pos >= Size) pos -= Size;
		if(readsize + pos > Size)
		{
			// readsize + pos ���Хåե��νK�ˤ򳬤��Ƥ���
			//  �� �������֥�å���2��
			p1 = pos + Buffer;
			p1size = Size - pos;
			p2 = Buffer;
			p2size = readsize - p1size;
		}
		else
		{
			// readsize + pos ���Хåե��νK�ˤ򳬤��Ƥ��ʤ�
			//  �� �������֥�å���1��
			p1 = pos + Buffer;
			p1size = readsize;
			p2 = NULL;
			p2size = 0;
		}
	}

	//! @brief	�i���z�ߥݥ��󥿤��M���
	//! @param	advance		�M���Ҫ����
	//! @note	���Υ᥽�åɤόg�H�� advance < GetDataSize() �Ǥ��뤳�Ȥ�_�J���ʤ���
	//!			��Ҫ�ʤ�к��ӳ����Ȥǥ����å����뤳�ȡ�
	void AdvanceReadPos(size_t advance = 1)
	{
		ReadPos += advance;
		if(ReadPos >= Size) ReadPos -= Size;
		DataSize -= advance;
	}

	//! @brief	�����Ҫ�ؤ򷵤�
	//! @return	�����Ҫ�ؤؤβ���
	//! @note	�����Ҫ�ؤؤβ��դ����äƤ��롣Ҫ�ؤ��Хåե��ڤ˟o���Ȥ��ϟo����Ҫ��
	//!			(���������Ǥ��ʤ�Ҫ��)�����äƤ���Τǡ���ǰ�˥Хåե��ڤ�Ҫ�ؤ�1������
	//!			���ڤ��뤳�Ȥ�_�J���뤳�ȡ����Υ᥽�åɤ��i���z�ߥݥ��󥿤��ƄӤ��ʤ���
	const T & GetFirst() const
	{
		size_t pos = ReadPos;
		return Buffer[pos];
	}

	//! @brief	n��Ŀ��Ҫ�ؤ򷵤�
	//! @return	n��Ŀ��Ҫ�ؤؤβ���
	//! @note	n��Ŀ��Ҫ�ؤؤβ��դ����äƤ��롣Ҫ�ؤ��Хåե��ڤ˟o���Ȥ��习����Εr
	//!			�΄�����δ���x�Ǥ��롣���Υ᥽�åɤ��i���z�ߥݥ��󥿤��ƄӤ��ʤ���
	const T & GetAt(size_t n) const
	{
		size_t pos = ReadPos + n;
		while(pos >= Size) pos -= Size;
		return Buffer[pos];
	}

	//! @brief	�Хåե��˕����z�ि��Υݥ��󥿤�ä�
	//! @param	writesize �����z�ߤ����ǩ`���� ( 1 ���Ϥ�����; 0 ��ɤ��ʤ����� )
	//! @param	p1		�֥�å�1�����^�ؤΥݥ��󥿤��{���뤿��Ή���
	//! @param	p1size	p1�α��֥�å��Υ�����
	//! @param	p2		�֥�å�2�����^�ؤΥݥ��󥿤��{���뤿��Ή���(NULL������ä�)
	//! @param	p2size	p2�α��֥�å��Υ�����(0������ä�)
	//! @param	offset	WritePos �˼��㤵��륪�ե��å�
	//! @note	GetReadPointer���h������դΤ���
	void GetWritePointer(size_t writesize,
						T * & p1, size_t &p1size,
						T * & p2, size_t &p2size,
						ptrdiff_t offset = 0)
	{
		size_t pos = WritePos + offset;
		while(pos >= Size) pos -= Size;
		if(writesize + pos > Size)
		{
			// writesize + pos ���Хåե��νK�ˤ򳬤��Ƥ���
			//  �� �������֥�å���2��
			p1 = pos + Buffer;
			p1size = Size - pos;
			p2 = Buffer;
			p2size = writesize - p1size;
		}
		else
		{
			// writesize + pos ���Хåե��νK�ˤ򳬤��Ƥ��ʤ�
			//  �� �������֥�å���1��
			p1 = pos + Buffer;
			p1size = writesize;
			p2 = NULL;
			p2size = 0;
		}
	}

	//! @brief	�����z�ߥݥ��󥿤��M���
	//! @param	advance		�M���Ҫ����
	//! @note	���Υ᥽�åɤόg�H�� advance < GetFreeSize() �Ǥ��뤳�Ȥ�_�J���ʤ���
	//!			��Ҫ�ʤ�к��ӳ����Ȥǥ����å����뤳�ȡ�
	void AdvanceWritePos(size_t advance = 1)
	{
		WritePos += advance;
		if(WritePos >= Size) WritePos -= Size;
		DataSize += advance;
	}

	//! @brief	�����z�ߥݥ��󥿤��M�ᡢ�Хåե������դ줿�����^��ΤƤ�
	//! @param	advance		�M���Ҫ����
	//! @note	AdvanceWritePos �Ȯ��ʤꡢ�Хåե������դ줿�顢�ǩ`�������^��ΤƤ롣
	void AdvanceWritePosWithDiscard(size_t advance = 1)
	{
		WritePos += advance;
		if(WritePos >= Size) WritePos -= Size;
		DataSize += advance;
		if(DataSize > Size)
		{
			AdvanceReadPos(DataSize - Size);
		}
	}

	//! @brief	�����z��λ�ä�Ҫ�ؤ򷵤�
	//! @return	�����z��λ�ä�Ҫ�ؤؤβ���
	//! @note	�����z��λ�ä�Ҫ�ؤؤβ��դ����äƤ��롣���Υ᥽�åɤϥХåե��˿դ�
	//!			�����뤫�ɤ����Υ����å����Ф�ʤ��Τ�ע�⤹�뤳�ȡ�
	//!			���Υ᥽�åɤϥХåե��Ε����z��λ�ä��ƄӤ��ʤ���
	T & GetLast()
	{
		return Buffer[WritePos];
	}
};

#endif
