#include "pinout.h"

PinoutDlg::PinoutDlg(const int& openingType) {
	
	CtrlLayoutOKCancel(*this, t_("Pinout"));

	ctrls // manual declaration
		(LABEL, LE_Label)
		(PIN_SIZE, EF_Size)
		(DETAIL, DE_Detail)
	;	
	
	// remove tab character insertion to allow tab navigation in the control
	LE_Label.NoProcessTab();
	//LE_Size.NoProcessTab();
	DE_Detail.NoProcessTab();
	DE_Detail.SetFont(Monospace());
	EF_Size.MaxChars(5);
	
	switch (openingType) {
		case OPENING_NEW:
			Title(t_("New pinout"));
			break;	
		case OPENING_EDIT:
			Title(t_("Edit pinout"));
			break;
	}
	
	this->Sizeable();	
}