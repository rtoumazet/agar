#include "Pinout.h"

PinoutDlg::PinoutDlg() {
	
	CtrlLayoutOKCancel(*this, t_("Pinout"));

	ctrls // manual declaration
		(LABEL, LE_Label)
		(PIN_SIZE, LE_Size)
		(DETAIL, DE_Detail)
	;	
}