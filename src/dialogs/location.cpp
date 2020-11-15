#include "location.h"

LocationDlg::LocationDlg(const int& openingType) {
	
	CtrlLayoutOKCancel(*this, t_("Location"));

	//ctrls(*this, LOCATION); //	matches widgets to columns based on Layout and schema introspection
	
	ctrls // manual declaration
		(LABEL, LE_Label)
	;	
	
	ActiveFocus(LE_Label); // setting focus
	
	switch (openingType) {
		case OPENING_NEW:
			Title(t_("New location"));
			break;	
		case OPENING_EDIT:
			Title(t_("Edit location"));
			break;
	}	
}