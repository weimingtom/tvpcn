#ifndef KMP_PI_H
#define KMP_PI_H

/*

     KbMedia Player Plugin �إå��ե����루�ƥ��Ȱ棩

*/

#define KMPMODULE_VERSION 100 //KMPMODULE �ΥЩ`�����
#define KMP_GETMODULE     kmp_GetTestModule  //�ޤ��ƥ��Ȱ�...
#define SZ_KMP_GETMODULE "kmp_GetTestModule" //�ޤ��ƥ��Ȱ�...

typedef void* HKMP;//'K'b'M'edia 'P'layer Plugin �� Handle

typedef struct
{//���`�ץ󤷤�������ɥǩ`�������
    DWORD dwSamplesPerSec;//����ץ���ܲ���(44100, 22050 �ʤ�)
    DWORD dwChannels;     //�����ͥ���( mono = 1, stereo = 2)
    DWORD dwBitsPerSample;//���ӻ��ӥå���( 8 or 16)
    DWORD dwLength;       //�����L����SPC �Τ褦��Ӌ�㲻���ܤʈ��Ϥ� 0xFFFFFFFF��
    DWORD dwSeekable;     //���`���򥵥ݩ`�Ȥ��Ƥ�����Ϥ� 1�����ʤ����Ϥ� 0
                          //�����j�Ѥ˥��`���Ǥ��Ƥ⡢���`���������λ�ä�
                          // �����_�ʈ��Ϥ� 0 �Ȥ��룩
    DWORD dwUnitRender;   //Render �v���εڣ������Ϥ��΂��α������ɤ���루�ɤ�ʂ��Ǥ��������Ϥ� 0��
    DWORD dwReserved1;    //���� 0
    DWORD dwReserved2;    //���� 0
}SOUNDINFO;


typedef struct
{
    DWORD dwVersion;
    //�⥸��`��ΥЩ`�����DLL �ΥЩ`�����ǤϤʤ����ؤ� KMPMODULE_VERSION(=100) �ˤ��뤳�ȡ�
    //���΂��� KbMedia Player ���ڴ����낎��һ�¤��ʤ����Ϥϡ�KbMedia Player
    //�ˤ�ä�ֱ���� FreeLibrary �����Ф�롣
    //���Έ��ϡ�InitDll() �� DeinitDll() ����Ф�ʤ����Ȥ�ע�⡣

    DWORD dwPluginVersion;
    //�ץ饰����ΥЩ`�����
    //ͬ���ե��������Υץ饰����Ҋ�Ĥ��ä����Ϥϡ����֤��󤭤���Τ��ȵĤ�ʹ��


    const char  *pszCopyright;
    //������
    //�Щ`��������Ǥ��β��֤������Ф��ʾ�����趨
    //NULL �ˤ��Ƥ�褤


    const char  *pszDescription;
    //�h��
    //�Щ`��������Ǥ��β��֤������Ф��ʾ�����趨
    //NULL �ˤ��Ƥ�褤


    const char  **ppszSupportExts;
    //���ݩ`�Ȥ��뒈���Ӥ������Ф�����(�ԥꥪ�ɺ���)
    //NULL �ǽK���褦�ˤ���
    //����ppszSupportExts = {".mp1", ".mp2", ".mp3", "rmp", NULL};
    //
    //�����Ӥ����Ĥ�ʤ����Ϥϡ�KbMedia Player �ˤ�ä�ֱ���� FreeLibrary ����
    //�Ф�롣���Έ��Ϥ� InitDll() �� DeinitDll() ����Ф�ʤ����Ȥ�ע�⡣


    DWORD dwReentrant;
    //�ץ饰�����}��ͬ�r���������ܤ��˘��ʈ��Ϥ� 1, �����ܤʈ��Ϥ� 0
    //�}���Β����Ӥ򥵥ݩ`�Ȥ��Ƥ��ơ��ض��Β����Ӥ��}��ͬ�r���������ܤǤ��ä�
    //�⡢���ĤǤ��}��ͬ�r�����������ܤʤ�Τ�������Ϥ� 0 �ˤ��뤳�ȡ�
    //���΂��� 0 �Ǥ��äƤ⡢Open �v���Ǒ��ä��ϥ�ɥ뤬�Є����g�� Open �����
    //�����Ƥ⡢�F�������ФνY����Ӱ푤򼰤ܤ��ʤ��褦�ˤ��뤳�ȡ�
    //�����Ĥ˥����ե��`�ɤˌ��ꤷ���Ȥ��ˤ��΂�����դ����趨


    void  (WINAPI *Init)(void);
    //DLL���ڻ���Open �Ȥ���ӳ���ǰ�� KbMedia Player �ˤ�ä�һ�Ȥ������Ф�롣
    //��Ҫ�ʤ����Ϥ� NULL �ˤ��Ƥ�������
    //��������� DllMain �� PROCESS_ATTACH ��ꤳ����ǳ��ڻ����루���Ӥθ��ٻ��Τ��ᣩ


    void  (WINAPI *Deinit)(void);
    //DLL����ʼĩ��FreeLibrary ��ֱǰ��һ�Ȥ������Ф�롣
    //��Ҫ�ʤ����Ϥ� NULL �ˤ��Ƥ�������
    //InitDll() ��һ�Ȥ���Ф��� DeinitDll() ����ֿ����Ԥ⤢�뤳�Ȥ�ע��


    HKMP (WINAPI *Open)(const char *cszFileName, SOUNDINFO *pInfo);
    //�ե�������_�����ؤ��gװ���뤳�ȡ�
    //����`�Έ��Ϥ� NULL �򷵤���
    //����`�Ǥʤ����Ϥ� pInfo ���m�Ф��������뤳�ȡ��m�Ф������ä�
    //���ʤ����ϣ�dwBits �� 0 �ʤɣ��� KbMedia Player �ˤ�ä�ֱ���� Close
    //�����Ф�롣
    //pInfo �� NULL �Έ��ϤϺΤ⤷�ʤ��� NULL �򷵤�����


    HKMP (WINAPI *OpenFromBuffer)(const BYTE *Buffer, DWORD dwSize, SOUNDINFO *pInfo);
    //���꤫���_�������ݩ`�Ȥ��ʤ����Ϥ� NULL �ˤ��뤳�ȡ�
    //���ݩ`�Ȥ��ʤ�����Ȥ��äơ����� NULL �򷵤��褦�ˌgװ���ƤϤʤ�ʤ���
    //�}���Β����Ӥˌ��ꤷ�Ƥ��Ƥ⡢���ĤǤ⥵�ݩ`�Ȥ��ʤ������Ӥ�������Ϥ�
    //NULL �ˤ��ʤ���Фʤ�ʤ���
    //pInfo �� NULL �Έ��ϤϺΤ⤷�ʤ��� NULL �򷵤�����
    //��ΤȤ���KbMedia Player ���夬����ˌ��ꤷ�Ƥޤ���(^_^;

    void   (WINAPI *Close)(HKMP hKMP);
    //�ϥ�ɥ���]���롣�ؤ��gװ���뤳�ȡ�
    //hKMP �� NULL ��ɤ��Ƥ���ɷ�ʤ褦�ˤ��뤳�ȡ�


    DWORD  (WINAPI *Render)(HKMP hKMP, BYTE* Buffer, DWORD dwSize);
    //Buffer �� PCM �����롣�ؤ��gװ���뤳�ȡ�
    //dwSize �ϥХåե��Υ������ΥХ�������������ץ����ǤϤʤ���
    //���ꂎ�� Buffer �˕����z����Х�������������ץ����ǤϤʤ���
    //dwSize ���С�������򷵤���������K�ˡ�
    //dwSize �� SOUNDINFO::dwUnitRender �α������ɤ���롣
    //��dwUnitRender �΂����Τ�ΤǤϤʤ��������ο����Ԥ⤢�뤳�Ȥ�ע�⣩
    //
    //Buffer �� NULL �� dwSize �� 0 �Ǥʤ����Ϥϡ�dwSize �֤�������λ��
    //���M��뤳�ȡ����ƽK�ˤ��ƤϤ����ʤ���
    //���`���򥵥ݩ`�Ȥ��ʤ����Ϥˡ����ߩ`�� Render ���R�귵���g�Ф���
    //�����˥��`������I�����ٻ����뤿�ᡣ�Ȥ��äƤ⡢�ޤ�����η���
    //Buffer �� NULL ��ɤ����ȤϤʤ����ɡ�(^^;
    //
    //hKMP �� NULL ��ɤ������ϤϺΤ⤷�ʤ��� 0 �򷵤��褦�ˤ��뤳�ȡ�


    DWORD  (WINAPI *SetPosition)(HKMP hKMP, DWORD dwPos);
    //���`�����ؤ��gװ���뤳�ȡ�
    //dwPos �ϥ��`���Ȥ�����λ�á����ꂎ�ϥ��`���������λ�á��gλ�ϥߥ��롣
    //dwPos �ȑ��ꂎ����ȫ��һ�¤����Ҫ�Ϥʤ������ꂎ�ȱ���������λ�ä�
    //�`��󤭤��ʤ루���~�Ȥ�ͬ�������r��֧�Ϥ򤭤��������Ϥ� Open �r��
    //SOUNDINFO �� dwSeekable �� 0 �ˤ��Ƥ������ȡ��`��ʤ��������äƤ�
    //�ǳ���С�������Ϥ� dwSeekable �� 1 �ˤ��Ƥ������ȡ����ꂎ�����_�ʤ�
    //�С�dwPos �ȑ��ꂎ�β�󤭤��Ƥ� dwSeekable=1 �Ȥ���������
    //��ΤȤ���KbMeida Player ����� dwSeekable �΂���Ҋ�Ƥޤ���(^_^;
    //dwSeekable �΂��ˤ�äơ����~�ǩ`����ͬ����������Ȥ��Ȥ��ʤ��Ȥ���
    //���`���I���䤨���趨��
    //
    //���`����ȫ�����ݩ`�Ȥ��ʤ����Ϥϡ����^λ�äˑ����� 0 �򷵤����ȡ�
    //hKMP �� NULL ��ɤ������Ϥ� 0 �򷵤��褦�ˤ��뤳�ȡ�


}KMPMODULE;

typedef KMPMODULE* (WINAPI *pfnGetKMPModule)(void);

#endif
