#include "location.h"

LocationDlg::LocationDlg() {
	
	CtrlLayoutOKCancel(*this, t_("Location"));

	//ctrls(*this, LOCATION); //	matches widgets to columns based on Layout and schema introspection
	
	ctrls // manual declaration
		(LABEL, LE_Label)
	;	
}

bool LocationDlg::Key(dword key, int count) {
	
	if (key == K_TAB && LE_Label.HasFocus()) {
	    //Ctrl
	    //GetFocusCtrl();
	    return IterateFocusForward(GetFocusCtrl(),GetTopWindow());
    }
	return true;
}