//---------------------------------------------------------------------------

#ifndef MainUnitH
#define MainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE ¹ÜÀí¤Î¥³¥ó¥Ý©`¥Í¥ó¥È
	TMemo *NoticeMemo;
	TListView *ListView;
	TButton *CheckButton;
	TButton *CloseButton;
	TLabel *NotYetCheckedLabel;
	TLabel *CheckingLabel;
	TLabel *BrokenLabel;
	TLabel *NotBrokenLabel;
	TLabel *MoreThanOneFileIsBrokenLabel;
	TLabel *CheckingButtonLabel;
	TButton *CopyResultButton;
	TLabel *TheResultWasCopiedIntoClipboardLabel;
	void __fastcall CloseButtonClick(TObject *Sender);
	void __fastcall CheckButtonClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall CopyResultButtonClick(TObject *Sender);
private:	// ¥æ©`¥¶©`ÐûÑÔ
	bool Checking; // ÊðÃû¤ò¥Á¥§¥Ã¥¯ÖÐ¤«¤É¤¦¤«
	bool Aborting; // é]¤¸¤ë¥Ü¥¿¥ó¤¬Ñº¤µ¤ì¤¿¤«
	AnsiString BaseDir;
	AnsiString PublicKey;
	TListItem *CurrentCheckingItem;
	AnsiString AllFileList;
	int AllFileCount;
	DWORD LastDisplayedTick;
public:		// ¥æ©`¥¶©`ÐûÑÔ
	__fastcall TMainForm(TComponent* Owner);
private:	// ¥æ©`¥¶©`ÐûÑÔ
	void __fastcall MakeFileList(AnsiString path, int baselen);
	bool __fastcall SigProgressNotify(int percent);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
