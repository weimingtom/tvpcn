//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief リングバッファを実現する自家製テンプレートクラス
//---------------------------------------------------------------------------
#ifndef RingBufferH
#define RingBufferH

#include <stddef.h>
/*
	リングバッファ, ring buffer, circular buffer, 環状バッファ
*/

//---------------------------------------------------------------------------
//! @brief		固定長リングバッファの実装
//---------------------------------------------------------------------------
template <typename T>
class tRisaRingBuffer
{
	T * Buffer; //!< バッファ
	size_t Size; //!< バッファのサイズ
	size_t WritePos; //!< 書き込み位置
	size_t ReadPos; //!< 読み込み位置
	size_t DataSize; //!< バッファに入っているデータのサイズ

public:
	//! @brief コンストラクタ
	tRisaRingBuffer(size_t size)
	{
		Size = size;
		Buffer = new T[Size];
		WritePos = ReadPos = 0;
		DataSize = 0;
	}

	//! @brief デストラクタ
	~tRisaRingBuffer()
	{
		delete [] Buffer;
	}

	//! @brief	サイズを得る
	size_t GetSize() { return Size; }

	//! @brief	書き込み位置を得る
	size_t GetWritePos() { return WritePos; }

	//! @brief	読み込み位置を得る
	size_t GetReadPos() { return ReadPos; }

	//! @brief	バッファに入っているデータのサイズを得る
	size_t GetDataSize() { return DataSize; }

	//! @brief	バッファの空き容量を得る
	size_t GetFreeSize() { return Size - DataSize; }

	//! @brief	バッファから読み込むためのポインタを得る
	//! @param	readsize 読み込みたいデータ数 ( 1 以上の整数; 0 を渡さないこと )
	//! @param	p1		ブロック1の先頭へのポインタを格納するための変数
	//! @param	p1size	p1の表すブロックのサイズ
	//! @param	p2		ブロック2の先頭へのポインタを格納するための変数(NULLがあり得る)
	//! @param	p2size	p2の表すブロックのサイズ(0があり得る)
	//! @param	offset	ReadPos に加算されるオフセット
	//! @note	環状バッファといっても、実際はリニアな領域にバッファが確保されている。
	//!			そのため、 ReadPos + readsize がバッファの終端を超えている場合、得たい
	//!			ブロックは２つに分断されることになる。
	//!			このメソッドは、readsizeが実際にバッファに入っているデータのサイズ以下であるか
	//!			などのチェックはいっさい行わない。事前に GetDataSize を調べ、読み込みたい
	//!			サイズが実際にバッファにあるかどうかをチェックすること。
	void GetReadPointer(size_t readsize,
						const T * & p1, size_t &p1size,
						const T * & p2, size_t &p2size,
						ptrdiff_t offset = 0)
	{
		size_t pos = ReadPos + offset;
		while(pos >= Size) pos -= Size;
		if(readsize + pos > Size)
		{
			// readsize + pos がバッファの終端を超えている
			//  → 返されるブロックは2つ
			p1 = pos + Buffer;
			p1size = Size - pos;
			p2 = Buffer;
			p2size = readsize - p1size;
		}
		else
		{
			// readsize + pos がバッファの終端を超えていない
			//  → 返されるブロックは1つ
			p1 = pos + Buffer;
			p1size = readsize;
			p2 = NULL;
			p2size = 0;
		}
	}

	//! @brief	読み込みポインタを進める
	//! @param	advance		進める要素数
	//! @note	このメソッドは実際に advance < GetDataSize() であることを確認しない。
	//!			必要ならば呼び出し側でチェックすること。
	void AdvanceReadPos(size_t advance = 1)
	{
		ReadPos += advance;
		if(ReadPos >= Size) ReadPos -= Size;
		DataSize -= advance;
	}

	//! @brief	最初の要素を返す
	//! @return	最初の要素への参照
	//! @note	最初の要素への参照が帰ってくる。要素がバッファ内に無いときは無効な要素
	//!			(アクセスできない要素)が帰ってくるので、事前にバッファ内に要素が1つ以上
	//!			存在することを確認すること。このメソッドは読み込みポインタを移動しない。
	const T & GetFirst() const
	{
		size_t pos = ReadPos;
		return Buffer[pos];
	}

	//! @brief	n番目の要素を返す
	//! @return	n番目の要素への参照
	//! @note	n番目の要素への参照が帰ってくる。要素がバッファ内に無いときや範囲外の時
	//!			の動作は未定義である。このメソッドは読み込みポインタを移動しない。
	const T & GetAt(size_t n) const
	{
		size_t pos = ReadPos + n;
		while(pos >= Size) pos -= Size;
		return Buffer[pos];
	}

	//! @brief	バッファに書き込むためのポインタを得る
	//! @param	writesize 書き込みたいデータ数 ( 1 以上の整数; 0 を渡さないこと )
	//! @param	p1		ブロック1の先頭へのポインタを格納するための変数
	//! @param	p1size	p1の表すブロックのサイズ
	//! @param	p2		ブロック2の先頭へのポインタを格納するための変数(NULLがあり得る)
	//! @param	p2size	p2の表すブロックのサイズ(0があり得る)
	//! @param	offset	WritePos に加算されるオフセット
	//! @note	GetReadPointerの説明も参照のこと
	void GetWritePointer(size_t writesize,
						T * & p1, size_t &p1size,
						T * & p2, size_t &p2size,
						ptrdiff_t offset = 0)
	{
		size_t pos = WritePos + offset;
		while(pos >= Size) pos -= Size;
		if(writesize + pos > Size)
		{
			// writesize + pos がバッファの終端を超えている
			//  → 返されるブロックは2つ
			p1 = pos + Buffer;
			p1size = Size - pos;
			p2 = Buffer;
			p2size = writesize - p1size;
		}
		else
		{
			// writesize + pos がバッファの終端を超えていない
			//  → 返されるブロックは1つ
			p1 = pos + Buffer;
			p1size = writesize;
			p2 = NULL;
			p2size = 0;
		}
	}

	//! @brief	書き込みポインタを進める
	//! @param	advance		進める要素数
	//! @note	このメソッドは実際に advance < GetFreeSize() であることを確認しない。
	//!			必要ならば呼び出し側でチェックすること。
	void AdvanceWritePos(size_t advance = 1)
	{
		WritePos += advance;
		if(WritePos >= Size) WritePos -= Size;
		DataSize += advance;
	}

	//! @brief	書き込みポインタを進め、バッファがあふれたら先頭を捨てる
	//! @param	advance		進める要素数
	//! @note	AdvanceWritePos と異なり、バッファがあふれたら、データの先頭を捨てる。
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

	//! @brief	書き込み位置の要素を返す
	//! @return	書き込み位置の要素への参照
	//! @note	書き込み位置の要素への参照が帰ってくる。このメソッドはバッファに空き
	//!			があるかどうかのチェックは行わないので注意すること。
	//!			このメソッドはバッファの書き込み位置を移動しない。
	T & GetLast()
	{
		return Buffer[WritePos];
	}
};

#endif
