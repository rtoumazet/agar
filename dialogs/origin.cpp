#include "origin.h"

OriginDlg::OriginDlg() {
	
	CtrlLayoutOKCancel(*this, t_("Origin"));

	//ctrls(*this, ORIGIN); //	matches widgets to columns based on Layout and schema introspection
	
	ctrls // manual declaration
		(NAME, LE_Name)
		(ORIGIN, LE_Origin)
		(COMMENTARY, DE_Comment)
	;	
	
	//IterateFocusForward()
	
}

bool OriginDlg::Key(dword key, int count) {
	
/*if(key == K_TAB) {
	Ctrl *ctrl = GetFocusChildDeep();
	if(ctrl && IterateFocusForward(ctrl, this))
		return true;
}
	//if(GetFirstChild()->SetWantFocus()) 
return false;*/
}