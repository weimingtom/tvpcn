//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef RelFolderSelectorUnitH
#define RelFolderSelectorUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ShellCtrls.hpp"
#include <ComCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TRelFolderSelectorForm : public TForm
{
__published:	// IDE ¹ÜÀí¤Î¥³¥ó¥Ý©`¥Í¥ó¥È
	TShellTreeView *ShellTreeView;
	TButton *OKButton;
	TButton *CancelButton;
	TShellComboBox *ShellComboBox;
	TLabel *Label1;
	TStatusBar *StatusBar;
	TShellPopupMenu *ShellPopupMenu;
	TMenuItem *SelectMenuItem;
	TMenuItem *N2;
	void __fastcall ShellTreeViewChange(TObject *Sender, TTreeNode *Node);
	void __fastcall SelectMenuItemClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// ¥æ©`¥¶©`ÐûÑÔ
public:		// ¥æ©`¥¶©`ÐûÑÔ
	__fastcall TRelFolderSelectorForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TRelFolderSelectorForm *RelFolderSelectorForm;
//---------------------------------------------------------------------------
#endif
