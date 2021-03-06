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
#ifndef WAVESEGMENTH
#define WAVESEGMENTH


#include <deque>


//---------------------------------------------------------------------------
//! @brief 壅伏セグメント秤��
//---------------------------------------------------------------------------
struct tTVPWaveSegment
{
	//! @brief コンストラクタ
	tTVPWaveSegment(tjs_int64 start, tjs_int64 length)
		{ Start = start; Length = FilteredLength = length; }
	tTVPWaveSegment(tjs_int64 start, tjs_int64 length, tjs_int64 filteredlength)
		{ Start = start; Length = length; FilteredLength = filteredlength; }
	tjs_int64 Start; //!< オリジナルデコ�`ダ貧でのセグメントのスタ�`ト了崔 (PCM サンプルグラニュ�`ル方�g了)
	tjs_int64 Length; //!< オリジナルデコ�`ダ貧でのセグメントの�Lさ (PCM サンプルグラニュ�`ル方�g了)
	tjs_int64 FilteredLength; //!< フィルタ瘁の�Lさ (PCM サンプルグラニュ�`ル方�g了)
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief 壅伏ラベル秤��
//---------------------------------------------------------------------------
struct tTVPWaveLabel
{
	//! @brief コンストラクタ
	tjs_int64 Position; //!< オリジナルデコ�`ダ貧でのラベル了崔 (PCM サンプルグラニュ�`ル方�g了)
	ttstr Name; //!< ラベル兆
	tjs_int Offset;
		/*!< オフセット
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

	//! @brief コンストラクタ
	tTVPWaveLabel()
	{
		Position = 0;
		Offset = 0;
#ifdef TVP_IN_LOOP_TUNER
		NameWidth = 0;
		Index = 0;
#endif
	}

	//! @brief コンストラクタ
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
//! @brief Waveのセグメントラベルのキュ�`を砿尖するクラス
//---------------------------------------------------------------------------
class tTVPWaveSegmentQueue
{
	// deque による Segments と Labels の塘双。
	// �g喘貧は、これらの塘双に寄楚のデ�`タが秘ることはまずないので
	// vector で噴蛍なのかもしれないが ...
	std::deque<tTVPWaveSegment> Segments; //!< セグメントの塘双
	std::deque<tTVPWaveLabel> Labels; //!< ラベルの塘双

public:
	//! @brief		坪否をクリアする
	void Clear();

	//! @brief		セグメントの塘双を誼る
	//! @return		セグメントの塘双
	const std::deque<tTVPWaveSegment> & GetSegments() const { return Segments; }

	//! @brief		ラベルの塘双を誼る
	//! @return		ラベルの塘双
	const std::deque<tTVPWaveLabel> & GetLabels() const { return Labels; }

	//! @brief		tTVPWaveSegmentQueueをエンキュ�`する
	//! @param		queue		エンキュ�`したいtTVPWaveSegmentQueueオブジェクト
	void Enqueue(const tTVPWaveSegmentQueue & queue);

	//! @brief		tTVPWaveSegmentをエンキュ�`する
	//! @param		queue		エンキュ�`したいtTVPWaveSegmentオブジェクト
	void Enqueue(const tTVPWaveSegment & segment);

	//! @brief		tTVPWaveLabelをエンキュ�`する
	//! @param		queue		エンキュ�`したいtTVPWaveLabelオブジェクト
	//! @note		Offset は俐屎されないので廣吭
	void Enqueue(const tTVPWaveLabel & Label);

	//! @brief		tTVPWaveSegmentの塘双をエンキュ�`する
	//! @param		queue		エンキュ�`したい std::dequeue<tTVPWaveSegment>オブジェクト
	void Enqueue(const std::deque<tTVPWaveSegment> & segments);

	//! @brief		tTVPWaveLabelの塘双をエンキュ�`する
	//! @param		queue		エンキュ�`したい std::dequeue<tTVPWaveLabel>オブジェクト
	void Enqueue(const std::deque<tTVPWaveLabel> & Labels);

	//! @brief		枠�^から峺協�Lさ蛍をデキュ�`する
	//! @param		dest		鯉�{枠キュ�`(坪否はクリアされる)
	//! @param		length		俳り竃す�Lさ(サンプルグラニュ�`ル�g了)
	void Dequeue(tTVPWaveSegmentQueue & dest, tjs_int64 length);

	//! @brief		このキュ�`の畠悶の�Lさを誼る
	//! @return		このキュ�`の�Lさ (サンプルグラニュ�`ル�g了)
	tjs_int64 GetFilteredLength() const;

	//! @brief		このキュ�`の�Lさを�篁�させる
	//! @param		new_total_filtered_length 仟しいキュ�`の�Lさ (サンプルグラニュ�`ル�g了)
	//! @note		キュ�`嶄のSegments などの�Lさや Labelsの了崔は��侘�a�gされる
	void Scale(tjs_int64 new_total_length);

	//! @brief		フィルタされた了崔からデコ�`ド了崔へ���Qを佩う
	//! @param		pos フィルタされた了崔
	//! @note		デコ�`ド了崔
	tjs_int64 FilteredPositionToDecodePosition(tjs_int64 pos) const;
};
//---------------------------------------------------------------------------

#endif

