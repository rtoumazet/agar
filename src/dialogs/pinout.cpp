#include "pinout.h"

PinoutDlg::PinoutDlg(const OpeningType type) {
	
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
	
	switch (type) {
		case OpeningType::opening_new:
			Title(t_("New pinout"));
			break;	
		case OpeningType::opening_edit:
			Title(t_("Edit pinout"));
			break;
	}
	
	this->Sizeable();	
}