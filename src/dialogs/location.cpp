#include "location.h"
#include "../enums.h"

LocationDlg::LocationDlg(const OpeningType type) {
	
	CtrlLayoutOKCancel(*this, t_("Location"));

	ctrls // manual declaration
		(LABEL, LE_Label)
	;	
	
	ActiveFocus(LE_Label); // setting focus
	
	switch (type) {
		case OpeningType::opening_new:
			Title(t_("New location"));
			break;
		case OpeningType::opening_edit:
			Title(t_("Edit location"));
			break;
	}
}