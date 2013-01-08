#include "location.h"

LocationDlg::LocationDlg() {
	
	CtrlLayoutOKCancel(*this, t_("Location"));

	//ctrls(*this, LOCATION); //	matches widgets to columns based on Layout and schema introspection
	
	ctrls // manual declaration
		(LABEL, LE_Label)
	;	
}