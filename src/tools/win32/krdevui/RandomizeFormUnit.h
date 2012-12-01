//---------------------------------------------------------------------------

#ifndef RandomizeFormUnitH
#define RandomizeFormUnitH
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
#define _MSC_VER
#include <tomcrypt.h>
#undef _MSC_VER

//---------------------------------------------------------------------------
class TRandomizeForm : public TForm
{
__published:	// IDE ¹ÜÀí¤Î¥³¥ó¥Ý©`¥Í¥ó¥È
	TButton *CancelButton;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label5;
	TLabel *Label7;
	TTimer *Timer;
	void __fastcall TimerTimer(TObject *Sender);
private:	// ¥æ©`¥¶©`ÐûÑÔ
	prng_state *State;
	POINT PrevMousePos;
	__int64 Distance;
	DWORD StartTick;
	
public:		// ¥æ©`¥¶©`ÐûÑÔ
	__fastcall TRandomizeForm(TComponent* Owner, prng_state *state);
};
//---------------------------------------------------------------------------
extern PACKAGE TRandomizeForm *RandomizeForm;
//---------------------------------------------------------------------------
bool RandomizePRNG(prng_state *state);
bool RandomizePRNGSimple(prng_state *state);
//---------------------------------------------------------------------------

#endif
