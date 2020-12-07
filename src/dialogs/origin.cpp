#include "origin.h"

OriginDlg::OriginDlg(const OpeningType type)
{
	CtrlLayoutOKCancel(*this, t_("Origin"));

	ctrls // manual declaration
		(NAME, LE_Name)
		(ORIGIN, LE_Origin)
		(COMMENTARY, DE_Comment)
    ;
	
	// remove tab character insertion to allow tab navigation in the control
	LE_Name.NoProcessTab();
	LE_Origin.NoProcessTab();
	DE_Comment.NoProcessTab();
	
	switch (type) {
		case OpeningType::opening_new:
			Title(t_("New origin"));
			break;
		case OpeningType::opening_edit:
			Title(t_("Edit origin"));
			break;
	}
}