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
#include "tjsCommHead.h"

#include "WaveSegmentQueue.h"


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Clear()
{
	Labels.clear();
	Segments.clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Enqueue(const tTVPWaveSegmentQueue & queue)
{
	Enqueue(queue.Labels); // Labels �򥨥󥭥�`(���ä����Ȥˤ��ʤ��Ȥ���)
	Enqueue(queue.Segments); // segments �򥭥�`(���ä�����)
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Enqueue(const tTVPWaveSegment & segment)
{
	if(Segments.size() > 0)
	{
		// �ȴ�Υ������Ȥ� 1 �����Ϥ���
		tTVPWaveSegment & last = Segments.back();
		// ����Υ������ȤȤ��줫��׷�Ӥ��褦�Ȥ��륻�����Ȥ��B�A���Ƥ뤫��
		if(last.Start + last.Length == segment.Start &&
			(double)last.FilteredLength / last.Length ==
			(double)segment.FilteredLength / segment.Length)
		{
			// �B�A���Ƥ��ơ����ġ����ʤ���ȫ��ͬ���ʤΤ�
			// �ȴ������Υ������Ȥ����L����
			// (���ʤߤˤ����Ǳ��ʤα��^���H���`��������Ȥ��Ƥ�
			//  �󤭤ʆ��}�ȤϤʤ�ʤ�)
			last.FilteredLength += segment.FilteredLength;
			last.Length += segment.Length;
			return ; // �����
		}
	}

	// �g���������Ҫ�ؤ�׷��
	Segments.push_back(segment);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Enqueue(const tTVPWaveLabel & Label)
{
	Labels.push_back(Label);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Enqueue(const std::deque<tTVPWaveSegment> & segments)
{
	// segment ��׷��
	for(std::deque<tTVPWaveSegment>::const_iterator i = segments.begin();
		i != segments.end(); i++)
		Enqueue(*i);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Enqueue(const std::deque<tTVPWaveLabel> & Labels)
{
	// ���ե��åȤ˼��㤹�낎��ä�
	tjs_int64 Label_offset = GetFilteredLength();

	// Label ��׷��
	for(std::deque<tTVPWaveLabel>::const_iterator i = Labels.begin();
		i != Labels.end(); i++)
	{
		tTVPWaveLabel one_Label(*i);
		one_Label.Offset += Label_offset; // offset ������
		Enqueue(one_Label);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Dequeue(tTVPWaveSegmentQueue & dest, tjs_int64 length)
{
	tjs_int64 remain;
	// dest �򥯥ꥢ
	dest.Clear();

	// Segments ���Ф����
	remain = length;
	while(Segments.size() > 0 && remain > 0)
	{
		if(Segments.front().FilteredLength <= remain)
		{
			// Segments.front().FilteredLength �� remain ����
			// �� ����Ҫ�ؤ� dest �˥��󥭥�`���� this ��������
			remain -= Segments.front().FilteredLength;
			dest.Enqueue(Segments.front());
			Segments.pop_front();
		}
		else
		{
			// Segments.front().FilteredLength �� remain ����󤭤�
			// �� Ҫ�ؤ�;�ФǤ֤ä����ä� dest �˥��󥭥�`
			// FilteredLength ��Ԫ���Ф�������ФäƤ�Τ�
			// Length �� �����a�g���Ф�
			tjs_int64 newlength =
				static_cast<tjs_int64>(
					(double)Segments.front().Length / (double)Segments.front().FilteredLength * remain);
			if(newlength > 0)
				dest.Enqueue(tTVPWaveSegment(Segments.front().Start, newlength, remain));

			// Segments.front() �� Start, Length �� FilteredLength ������
			Segments.front().Start += newlength;
			Segments.front().Length -= newlength;
			Segments.front().FilteredLength -= remain;
			if(Segments.front().Length == 0 || Segments.front().FilteredLength == 0)
			{
				// �֤ä��Фä��Y�� (�����a�ꤷ���Y�����`���)
				// �L����0�ˤʤäƤ��ޤä�
				Segments.pop_front(); // �������Ȥ�ΤƤ�
			}
			remain = 0; // ��`�פ�i����
		}
	}

	// Labels ���Ф����
	size_t Labels_to_dequeue = 0;
	for(std::deque<tTVPWaveLabel>::iterator i = Labels.begin();
		i != Labels.end(); i++)
	{
		tjs_int64 newoffset = i->Offset - length;
		if(newoffset < 0)
		{
			// newoffset ��ؓ �ʤΤ� dest ������
			dest.Enqueue(*i);
			Labels_to_dequeue ++; // ���Ȥ� dequeue
		}
		else
		{
			// *i �Υ��ե��åȤ�����
			i->Offset = newoffset;
		}
	}

	while(Labels_to_dequeue--) Labels.pop_front(); // ���ԩ`����Labels ������
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tjs_int64 tTVPWaveSegmentQueue::GetFilteredLength() const
{
	// ����`���L���� ���٤Ƥ� Segments ��FilteredLength�κ�Ӌ
	tjs_int64 length = 0;
	for(std::deque<tTVPWaveSegment>::const_iterator i = Segments.begin();
		i != Segments.end(); i++)
		length += i->FilteredLength;

	return length;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Scale(tjs_int64 new_total_filtered_length)
{
	// ����`�� FilteredLength ��仯������
	tjs_int64 total_length_was = GetFilteredLength(); // �仯ǰ���L��

	if(total_length_was == 0) return; // Ԫ���L�����ʤ��Τǥ����`������ʤ�

	// Segments ������
	tjs_int64 offset_was = 0; // �仯ǰ�Υ��ե��å�
	tjs_int64 offset_is = 0; // �仯��Υ��ե��å�

	for(std::deque<tTVPWaveSegment>::iterator i = Segments.begin();
		i != Segments.end(); i++)
	{
		tjs_int64 old_end = offset_was + i->FilteredLength;
		offset_was += i->FilteredLength;

		// old_end ��ȫ�夫��Ҋ�Ƥɤ�λ�äˤ��룿
		double ratio = static_cast<double>(old_end) /
						static_cast<double>(total_length_was);

		// �¤��� old_end �Ϥɤ�λ�äˤ���٤���
		tjs_int64 new_end = static_cast<tjs_int64>(ratio * new_total_filtered_length);

		// FilteredLength ������
		i->FilteredLength = new_end - offset_is;

		offset_is += i->FilteredLength;
	}

	// ����äݤ�Segments �γ�ȥ
	for(std::deque<tTVPWaveSegment>::iterator i = Segments.begin();
		i != Segments.end() ; )
	{
		if(i->FilteredLength == 0 || i->Length == 0)
			i = Segments.erase(i);
		else
			i++;
	}

	// Labels ������
	double ratio = (double)new_total_filtered_length / (double)total_length_was;
	for(std::deque<tTVPWaveLabel>::iterator i = Labels.begin();
		i != Labels.end(); i++)
	{
		i->Offset = static_cast<tjs_int64>(i->Offset * ratio);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tjs_int64 tTVPWaveSegmentQueue::FilteredPositionToDecodePosition(tjs_int64 pos) const
{
	// Segments ������
	tjs_int64 offset_filtered = 0;

	for(std::deque<tTVPWaveSegment>::const_iterator i = Segments.begin();
		i != Segments.end(); i++)
	{
		if(offset_filtered <= pos && pos < offset_filtered + i->FilteredLength)
		{
			// ���ꤹ�����g��Ҋ�Ĥ��ä��ΤǾ��Τ��a�ꤷ�Ʒ���
			return (tjs_int64)(i->Start + (pos - offset_filtered) *
				(double)i->Length / (double)i->FilteredLength );
		}

		offset_filtered += i->FilteredLength;
	}

	// ���ꤹ�����g��Ҋ�Ĥ���ʤ��Τǡ����餫��ؓ�Ǥ���� 0 ��
	// �����Ǥʤ���������λ�ä򷵤�
	if(pos<0) return 0;
	if(Segments.size() == 0) return 0;
	return Segments[Segments.size()-1].Start + Segments[Segments.size()-1].Length;
}
//---------------------------------------------------------------------------


