#ifndef TVP_IWAVEUNPACKER_DEFINED
#define TVP_IWAVEUNPACKER_DEFINED
class IWaveUnpacker
{
public:
// IUnknown �������饹�ǤϤʤ��Τ�ע��
	virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
	virtual ULONG STDMETHODCALLTYPE Release(void) = 0;

// IWaveUnpacker
	virtual HRESULT STDMETHODCALLTYPE GetTypeName(char *buf,long buflen)=0;
		/*
			buf �ˡ����� Wave ��ʽ��������Ф� *buf ���O�����Ƥ���������
			buflen �ϡ�buf �˴_�����줿�����Фǡ�null terminater �⺬��Τ�
			ע�⡣
		*/

	virtual HRESULT STDMETHODCALLTYPE GetWaveFormat(long *samplepersec,
		long *channels,long *bitspersample)=0;
		/*
			�������� Wave ����ʽ�� *samplepersec, *channels, *bitspersample ��
			�����Ƥ���������
		*/

	virtual HRESULT STDMETHODCALLTYPE Render(void *buffer,long bufsize,
		long *numwrite) =0;
		/*
			�ǥ��`�ɤ��Ƥ���������
			bufsize �ˤ� buffer �Υ��������Х��ȅgλ��ָ������ޤ���
			numwrite �ˤϡ��Хåե��˕����줿�ǩ`��������Х��ȅgλ�Ƿ����ޤ���
			��������WaveUnpacker �ϡ�numwrite<bufsize �Έ��Ϥϡ��Ф��
			0 �����Ƥ���������
		*/
	
	virtual HRESULT STDMETHODCALLTYPE GetLength(long *length)=0;
		/*
			�ǩ`���L�� ms �gλ�� *length �˷����Ƥ���������
			����Ǥ��ʤ����Ϥ� E_NOTIMPL �򷵤��Ƥ������������Έ��Ϥ�
			WaveSoundBuffer �� totalTime �ץ�ѥƥ��� 0 ����褦�ˤʤ�ޤ���
		*/

	virtual HRESULT STDMETHODCALLTYPE GetCurrentPosition(long *pos)=0;
		/*
			�F�ڤΥǥ��`��λ�ä� *pos �˷����Ƥ���������
			����Ǥ��ʤ����Ϥ� E_NOTIMPL �򷵤��Ƥ������������Έ��Ϥ�
			WaveSoundBuffer �� position �ץ�ѥƥ�����ζ�Τʤ�������
			ʾ���褦�ˤʤ�ޤ���
		*/

	virtual HRESULT STDMETHODCALLTYPE SetCurrentPosition(long pos)=0;
		/*
			�F�ڤΥǥ��`��λ�ä��O�����Ƥ���������pos �� ms �gλ�Ǥ�
			λ�äǤ���
			��ͤǤ� pos=0 �Ȥ��ƺ��Ф줿�Ȥ��ˡ����^�ؤΎ���������
			�����褦�ˤ��Ƥ���������

			���Τۤ��Έ��ϡ�����Ǥ��ʤ����Ϥ� E_NOTIMPL �򷵤��Ƥ���������
			���Έ��Ϥ�WaveSoundBuffer �� position �ץ�ѥƥ��ؤδ���ϟoҕ����ޤ���
		*/

	virtual HRESULT STDMETHODCALLTYPE Invoke(); // reserved
};

#endif
//---------------------------------------------------------------------------
