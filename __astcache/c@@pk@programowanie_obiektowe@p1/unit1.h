//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.VirtualImage.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TImage *road1;
	TImage *road2;
	TImage *road3;
	TImage *road4;
	TImage *road5;
	TImage *road6;
	TImage *road7;
	TImage *road8;
	TImage *road9;
	TImage *road10;
	TImage *road11;
	TImage *road12;
	TImage *road13;
	TImage *road14;
	TImage *road15;
	TTimer *GameClock;
	TImage *Image1;
	TImage *Image2;
	TImage *Image3;
	TImage *Image4;
	TImage *Image5;
	TImage *Image6;
	TImage *Image7;
	TImage *Image8;
	TImage *Image9;
	TImage *Image10;
	TImage *Image11;
	TImage *Image12;
	TImage *Image13;
	TImage *Image14;
	TImage *Image15;
	TImage *ImgRoadBitmap;
	TTimer *VehicleSpawner;
	TLabel *Label1;
	TLabel *Label2;
	void __fastcall GameClockTimer(TObject *Sender);
	void __fastcall VehicleSpawnerTimer(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
