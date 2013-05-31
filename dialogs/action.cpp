#include "action.h"


ActionDlg::ActionDlg(const int& pcbId, const int& actionType, const int& openingType, int parentId) {
//ActionDlg::ActionDlg(const int& actionType) {

	E_ActionType.Hide();
	E_PcbId.Hide();
	E_ParentId.Hide();

	if (actionType == ActionDlg::ANALYSIS) {

		CtrlLayoutOKCancel(*this, t_("Analysis"));
		E_ParentId <<= 0; // analysis always at top level
		O_Finished.Hide();

	} else {
		CtrlLayoutOKCancel(*this, t_("Action"));
		O_Finished.Show(true);
	}
	
	if (openingType == ActionDlg::CREATION) {
		E_Time <<= GetSysTime();
		E_PcbId <<= pcbId;
		E_ActionType <<= actionType;
	}
	
	if (openingType == ActionDlg::CREATION && actionType == ActionDlg::ACTION) {
		E_ParentId	<<= parentId;
	}
	
	ctrls // manual declaration
		
		(PCB_ID, E_PcbId)
		(PARENT_ID, E_ParentId)
		(ACTION_DATE, E_Time)
		(COMMENTARY, DE_Comment)
		(FINISHED, O_Finished)
		(ACTION_TYPE, E_ActionType)
	;
	
	ActiveFocus(DE_Comment);
}

ActionDlg::ActionDlg(const int& pcbId, const int& actionId) {

	E_ActionType.Hide();
	E_PcbId.Hide();
	E_ParentId.Hide();

	ctrls // manual declaration
		
		(PCB_ID, E_PcbId)
		(PARENT_ID, E_ParentId)
		(ACTION_DATE, E_Time)
		(COMMENTARY, DE_Comment)
		(FINISHED, O_Finished)
		(ACTION_TYPE, E_ActionType)
	;
	
	if(!ctrls.Load(PCB_ACTION, ID == actionId))
		return;		

	if (E_ActionType == ActionDlg::ANALYSIS) {

		CtrlLayoutOKCancel(*this, t_("Analysis"));
		E_ParentId <<= 0; // analysis always at top level
		O_Finished.Hide();

	} else {
		CtrlLayoutOKCancel(*this, t_("Action"));
		O_Finished.Show(true);
	}

	ActiveFocus(DE_Comment);
}