//---------------------------------------------------------------------------
/*
	Risa [�ꤵ]      alias ���Ｊ��3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Wave��������/��٥륭��`����
//---------------------------------------------------------------------------
#ifndef WAVESEGMENTH
#define WAVESEGMENTH


#include <deque>


//---------------------------------------------------------------------------
//! @brief ���������������
//---------------------------------------------------------------------------
struct tTVPWaveSegment
{
	//! @brief ���󥹥ȥ饯��
	tTVPWaveSegment(tjs_int64 start, tjs_int64 length)
		{ Start = start; Length = FilteredLength = length; }
	tTVPWaveSegment(tjs_int64 start, tjs_int64 length, tjs_int64 filteredlength)
		{ Start = start; Length = length; FilteredLength = filteredlength; }
	tjs_int64 Start; //!< ���ꥸ�ʥ�ǥ��`���ϤǤΥ������ȤΥ����`��λ�� (PCM ����ץ륰��˥�`�����gλ)
	tjs_int64 Length; //!< ���ꥸ�ʥ�ǥ��`���ϤǤΥ������Ȥ��L�� (PCM ����ץ륰��˥�`�����gλ)
	tjs_int64 FilteredLength; //!< �ե��륿����L�� (PCM ����ץ륰��˥�`�����gλ)
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief ������٥����
//---------------------------------------------------------------------------
struct tTVPWaveLabel
{
	//! @brief ���󥹥ȥ饯��
	tjs_int64 Position; //!< ���ꥸ�ʥ�ǥ��`���ϤǤΥ�٥�λ�� (PCM ����ץ륰��˥�`�����gλ)
	ttstr Name; //!< ��٥���
	tjs_int Offset;
		/*!< ���ե��å�
			@note
			This member will be set in tTVPWaveLoopManager::Render,
			and will contain the sample granule offset from first decoding
			point at call of tTVPWaveLoopManager::Render().
		*/

#ifdef TVP_IN_LOOP_TUNER
	// these are only used by the loop tuner
	tjs_int NameWidth; // display name width
	tjs_int Index; // index
#endif

	struct tSortByPositionFuncObj
	{
		bool operator()(
			const tTVPWaveLabel &lhs,
			const tTVPWaveLabel &rhs) const
		{
			return lhs.Position < rhs.Position;
		}
	};

	struct tSortByOffsetFuncObj
	{
		bool operator()(
			const tTVPWaveLabel &lhs,
			const tTVPWaveLabel &rhs) const
		{
			return lhs.Offset < rhs.Offset;
		}
	};

#ifdef TVP_IN_LOOP_TUNER
	struct tSortByIndexFuncObj
	{
		bool operator()(
			const tTVPWaveLabel &lhs,
			const tTVPWaveLabel &rhs) const
		{
			return lhs.Index < rhs.Index;
		}
	};
#endif

	//! @brief ���󥹥ȥ饯��
	tTVPWaveLabel()
	{
		Position = 0;
		Offset = 0;
#ifdef TVP_IN_LOOP_TUNER
		NameWidth = 0;
		Index = 0;
#endif
	}

	//! @brief ���󥹥ȥ饯��
	tTVPWaveLabel(tjs_int64 position, const ttstr & name, tjs_int offset)
		: Position(position), Name(name), Offset(offset)
	{
#ifdef TVP_IN_LOOP_TUNER
		NameWidth = 0;
		Index = 0;
#endif
	}
};
//---------------------------------------------------------------------------
bool inline operator < (const tTVPWaveLabel & lhs, const tTVPWaveLabel & rhs)
{
	return lhs.Position < rhs.Position;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief Wave�Υ������ȥ�٥�Υ���`������륯�饹
//---------------------------------------------------------------------------
class tTVPWaveSegmentQueue
{
	// deque �ˤ�� Segments �� Labels �����С�
	// �g���Ϥϡ����������Ф˴����Υǩ`������뤳�ȤϤޤ��ʤ��Τ�
	// vector ��ʮ�֤ʤΤ��⤷��ʤ��� ...
	std::deque<tTVPWaveSegment> Segments; //!< �������Ȥ�����
	std::deque<tTVPWaveLabel> Labels; //!< ��٥������

public:
	//! @brief		���ݤ򥯥ꥢ����
	void Clear();

	//! @brief		�������Ȥ����Ф�ä�
	//! @return		�������Ȥ�����
	const std::deque<tTVPWaveSegment> & GetSegments() const { return Segments; }

	//! @brief		��٥�����Ф�ä�
	//! @return		��٥������
	const std::deque<tTVPWaveLabel> & GetLabels() const { return Labels; }

	//! @brief		tTVPWaveSegmentQueue�򥨥󥭥�`����
	//! @param		queue		���󥭥�`������tTVPWaveSegmentQueue���֥�������
	void Enqueue(const tTVPWaveSegmentQueue & queue);

	//! @brief		tTVPWaveSegment�򥨥󥭥�`����
	//! @param		queue		���󥭥�`������tTVPWaveSegment���֥�������
	void Enqueue(const tTVPWaveSegment & segment);

	//! @brief		tTVPWaveLabel�򥨥󥭥�`����
	//! @param		queue		���󥭥�`������tTVPWaveLabel���֥�������
	//! @note		Offset ����������ʤ��Τ�ע��
	void Enqueue(const tTVPWaveLabel & Label);

	//! @brief		tTVPWaveSegment�����Ф򥨥󥭥�`����
	//! @param		queue		���󥭥�`������ std::dequeue<tTVPWaveSegment>���֥�������
	void Enqueue(const std::deque<tTVPWaveSegment> & segments);

	//! @brief		tTVPWaveLabel�����Ф򥨥󥭥�`����
	//! @param		queue		���󥭥�`������ std::dequeue<tTVPWaveLabel>���֥�������
	void Enqueue(const std::deque<tTVPWaveLabel> & Labels);

	//! @brief		���^����ָ���L���֤�ǥ���`����
	//! @param		dest		��{�ȥ���`(���ݤϥ��ꥢ�����)
	//! @param		length		�Ф�����L��(����ץ륰��˥�`��gλ)
	void Dequeue(tTVPWaveSegmentQueue & dest, tjs_int64 length);

	//! @brief		���Υ���`��ȫ����L����ä�
	//! @return		���Υ���`���L�� (����ץ륰��˥�`��gλ)
	tjs_int64 GetFilteredLength() const;

	//! @brief		���Υ���`���L����仯������
	//! @param		new_total_filtered_length �¤�������`���L�� (����ץ륰��˥�`��gλ)
	//! @note		����`�Ф�Segments �ʤɤ��L���� Labels��λ�äϾ����a�g�����
	void Scale(tjs_int64 new_total_length);

	//! @brief		�ե��륿���줿λ�ä���ǥ��`��λ�ä؉�Q���Ф�
	//! @param		pos �ե��륿���줿λ��
	//! @note		�ǥ��`��λ��
	tjs_int64 FilteredPositionToDecodePosition(tjs_int64 pos) const;
};
//---------------------------------------------------------------------------

#endif

