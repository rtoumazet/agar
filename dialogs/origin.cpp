#include "origin.h"

OriginDlg::OriginDlg() {
	
	CtrlLayoutOKCancel(*this, t_("Origin"));

	//ctrls(*this, ORIGIN); //	matches widgets to columns based on Layout and schema introspection
	
	ctrls // manual declaration
		(NAME, LE_Name)
		(ORIGIN, LE_Origin)
		(COMMENTARY, DE_Comment)
	;	
}