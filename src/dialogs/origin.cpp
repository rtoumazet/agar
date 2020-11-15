#include "origin.h"

OriginDlg::OriginDlg(const int& openingType) {
	
	CtrlLayoutOKCancel(*this, t_("Origin"));

	//ctrls(*this, ORIGIN); //	matches widgets to columns based on Layout and schema introspection
	
	ctrls // manual declaration
		(NAME, LE_Name)
		(ORIGIN, LE_Origin)
		(COMMENTARY, DE_Comment)
	;	
	
	// remove tab character insertion to allow tab navigation in the control
	LE_Name.NoProcessTab();
	LE_Origin.NoProcessTab();
	DE_Comment.NoProcessTab();
	
	switch (openingType) {
		case OPENING_NEW:
			Title(t_("New origin"));
			break;	
		case OPENING_EDIT:
			Title(t_("Edit origin"));
			break;
	}
	
}