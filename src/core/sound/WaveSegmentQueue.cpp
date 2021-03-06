//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 耳戦耳戦3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveセグメント/ラベルキュ�`砿尖
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
	Enqueue(queue.Labels); // Labels をエンキュ�`(こっちを枠にしないとだめ)
	Enqueue(queue.Segments); // segments をキュ�`(こっちは瘁)
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Enqueue(const tTVPWaveSegment & segment)
{
	if(Segments.size() > 0)
	{
		// 屡贋のセグメントが 1 ��參貧ある
		tTVPWaveSegment & last = Segments.back();
		// 恷瘁のセグメントとこれから弖紗しようとするセグメントが�B�Aしてるか��
		if(last.Start + last.Length == segment.Start &&
			(double)last.FilteredLength / last.Length ==
			(double)segment.FilteredLength / segment.Length)
		{
			// �B�Aしていて、かつ、曳楕も頼畠に揖じなので
			// 屡贋の恷瘁のセグメントを决�Lする
			// (ちなみにここで曳楕の曳�^の�Hに�`餓が伏じたとしても
			//  寄きな���}とはならない)
			last.FilteredLength += segment.FilteredLength;
			last.Length += segment.Length;
			return ; // おわり
		}
	}

	// �g��に恷瘁に勣殆を弖紗
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
	// segment の弖紗
	for(std::deque<tTVPWaveSegment>::const_iterator i = segments.begin();
		i != segments.end(); i++)
		Enqueue(*i);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Enqueue(const std::deque<tTVPWaveLabel> & Labels)
{
	// オフセットに紗麻する�､魑辰�
	tjs_int64 Label_offset = GetFilteredLength();

	// Label の弖紗
	for(std::deque<tTVPWaveLabel>::const_iterator i = Labels.begin();
		i != Labels.end(); i++)
	{
		tTVPWaveLabel one_Label(*i);
		one_Label.Offset += Label_offset; // offset の俐屎
		Enqueue(one_Label);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Dequeue(tTVPWaveSegmentQueue & dest, tjs_int64 length)
{
	tjs_int64 remain;
	// dest をクリア
	dest.Clear();

	// Segments を俳り竃す
	remain = length;
	while(Segments.size() > 0 && remain > 0)
	{
		if(Segments.front().FilteredLength <= remain)
		{
			// Segments.front().FilteredLength が remain 參和
			// ★ この勣殆を dest にエンキュ�`して this から��茅
			remain -= Segments.front().FilteredLength;
			dest.Enqueue(Segments.front());
			Segments.pop_front();
		}
		else
		{
			// Segments.front().FilteredLength が remain よりも寄きい
			// ★ 勣殆を余嶄でぶったぎって dest にエンキュ�`
			// FilteredLength を圷に俳り竃しを佩ってるので
			// Length は ��侘�a�gを佩う
			tjs_int64 newlength =
				static_cast<tjs_int64>(
					(double)Segments.front().Length / (double)Segments.front().FilteredLength * remain);
			if(newlength > 0)
				dest.Enqueue(tTVPWaveSegment(Segments.front().Start, newlength, remain));

			// Segments.front() の Start, Length と FilteredLength を俐屎
			Segments.front().Start += newlength;
			Segments.front().Length -= newlength;
			Segments.front().FilteredLength -= remain;
			if(Segments.front().Length == 0 || Segments.front().FilteredLength == 0)
			{
				// ぶった俳った�Y惚 (��侘�a頼した�Y惚の�`餓で)
				// �Lさが0になってしまった
				Segments.pop_front(); // セグメントを�里討�
			}
			remain = 0; // ル�`プを�iける
		}
	}

	// Labels を俳り竃す
	size_t Labels_to_dequeue = 0;
	for(std::deque<tTVPWaveLabel>::iterator i = Labels.begin();
		i != Labels.end(); i++)
	{
		tjs_int64 newoffset = i->Offset - length;
		if(newoffset < 0)
		{
			// newoffset が�� なので dest に秘れる
			dest.Enqueue(*i);
			Labels_to_dequeue ++; // あとで dequeue
		}
		else
		{
			// *i のオフセットを俐屎
			i->Offset = newoffset;
		}
	}

	while(Labels_to_dequeue--) Labels.pop_front(); // コピ�`したLabels を��茅
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tjs_int64 tTVPWaveSegmentQueue::GetFilteredLength() const
{
	// キュ�`の�Lさは すべての Segments のFilteredLengthの栽��
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
	// キュ�`の FilteredLength を�篁�させる
	tjs_int64 total_length_was = GetFilteredLength(); // �篁�念の�Lさ

	if(total_length_was == 0) return; // 圷の�Lさがないのでスケ�`ル竃栖ない

	// Segments の俐屎
	tjs_int64 offset_was = 0; // �篁�念のオフセット
	tjs_int64 offset_is = 0; // �篁�瘁のオフセット

	for(std::deque<tTVPWaveSegment>::iterator i = Segments.begin();
		i != Segments.end(); i++)
	{
		tjs_int64 old_end = offset_was + i->FilteredLength;
		offset_was += i->FilteredLength;

		// old_end は畠悶から��てどの了崔にある��
		double ratio = static_cast<double>(old_end) /
						static_cast<double>(total_length_was);

		// 仟しい old_end はどの了崔にあるべき��
		tjs_int64 new_end = static_cast<tjs_int64>(ratio * new_total_filtered_length);

		// FilteredLength の俐屎
		i->FilteredLength = new_end - offset_is;

		offset_is += i->FilteredLength;
	}

	// からっぽのSegments の茅肇
	for(std::deque<tTVPWaveSegment>::iterator i = Segments.begin();
		i != Segments.end() ; )
	{
		if(i->FilteredLength == 0 || i->Length == 0)
			i = Segments.erase(i);
		else
			i++;
	}

	// Labels の俐屎
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
	// Segments の俐屎
	tjs_int64 offset_filtered = 0;

	for(std::deque<tTVPWaveSegment>::const_iterator i = Segments.begin();
		i != Segments.end(); i++)
	{
		if(offset_filtered <= pos && pos < offset_filtered + i->FilteredLength)
		{
			// ��鬉垢詛��gが��つかったので��侘で�a頼して卦す
			return (tjs_int64)(i->Start + (pos - offset_filtered) *
				(double)i->Length / (double)i->FilteredLength );
		}

		offset_filtered += i->FilteredLength;
	}

	// ��鬉垢詛��gが��つからないので、苧らかに��であれば 0 を、
	// そうでなければ恷瘁の了崔を卦す
	if(pos<0) return 0;
	if(Segments.size() == 0) return 0;
	return Segments[Segments.size()-1].Start + Segments[Segments.size()-1].Length;
}
//---------------------------------------------------------------------------


