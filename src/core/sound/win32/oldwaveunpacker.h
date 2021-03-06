#ifndef TVP_IWAVEUNPACKER_DEFINED
#define TVP_IWAVEUNPACKER_DEFINED
class IWaveUnpacker
{
public:
// IUnknown 塗伏クラスではないので廣吭
	virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
	virtual ULONG STDMETHODCALLTYPE Release(void) = 0;

// IWaveUnpacker
	virtual HRESULT STDMETHODCALLTYPE GetTypeName(char *buf,long buflen)=0;
		/*
			buf に、この Wave 侘塀を燕す猟忖双を *buf に�O協してください。
			buflen は、buf に�_隠された猟忖双で、null terminater も根むので
			廣吭。
		*/

	virtual HRESULT STDMETHODCALLTYPE GetWaveFormat(long *samplepersec,
		long *channels,long *bitspersample)=0;
		/*
			竃薦する Wave の侘塀を *samplepersec, *channels, *bitspersample に
			卦してください。
		*/

	virtual HRESULT STDMETHODCALLTYPE Render(void *buffer,long bufsize,
		long *numwrite) =0;
		/*
			デコ�`ドしてください。
			bufsize には buffer のサイズがバイト�g了で峺協されます。
			numwrite には、バッファに��かれたデ�`タの方をバイト�g了で卦します。
			ただし、WaveUnpacker は、numwrite<bufsize の��栽は、火りを
			0 で托めてください。
		*/
	
	virtual HRESULT STDMETHODCALLTYPE GetLength(long *length)=0;
		/*
			デ�`タ�Lを ms �g了で *length に卦してください。
			��鬉任�ない��栽は E_NOTIMPL を卦してください。その��栽は
			WaveSoundBuffer の totalTime プロパティは 0 を燕すようになります。
		*/

	virtual HRESULT STDMETHODCALLTYPE GetCurrentPosition(long *pos)=0;
		/*
			�F壓のデコ�`ド了崔を *pos に卦してください。
			��鬉任�ない��栽は E_NOTIMPL を卦してください。その��栽は
			WaveSoundBuffer の position プロパティは吭龍のない方�､�
			幣すようになります。
		*/

	virtual HRESULT STDMETHODCALLTYPE SetCurrentPosition(long pos)=0;
		/*
			�F壓のデコ�`ド了崔を�O協してください。pos は ms �g了での
			了崔です。
			恷詰でも pos=0 として柵ばれたときに、枠�^への��き��しが
			竃栖ようにしてください。

			そのほかの��栽、��鬉任�ない��栽は E_NOTIMPL を卦してください。
			その��栽はWaveSoundBuffer の position プロパティへの旗秘は�o��されます。
		*/

	virtual HRESULT STDMETHODCALLTYPE Invoke(); // reserved
};

#endif
//---------------------------------------------------------------------------
