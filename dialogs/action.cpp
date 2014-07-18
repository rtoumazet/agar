#include "action.h"


ActionDlg::ActionDlg(const int pcbId, const int actionType, int parentId) 
{
	InitializeFields(actionType);
    
	E_ParentId	<<= parentId; // 0 for analysis, param value for action
	
	// Setting up default values
	E_Time <<= GetSysTime();
	E_PcbId <<= pcbId;
	E_ActionType <<= actionType;
	
	ActiveFocus(DE_Comment);
}

ActionDlg::ActionDlg(const ActionRecord& ar)
{
    InitializeFields(ar.type);
	
	// Fields are initialized from constructor parameter
	E_PcbId.SetData(ar.pcbId);
	E_ParentId.SetData(ar.parentIndex);
	E_Time.SetData(ar.date);
	DE_Comment.SetData(ar.commentary);
	O_Finished.SetData(ar.finished);
	E_ActionType.SetData(ar.type);
	
	Record(ar); // initializing internal record

	ActiveFocus(DE_Comment); // focus is set to the comment field
}

void ActionDlg::DoOk()
{
    // Saves modified data to the current record
	record_.pcbId       = E_PcbId.GetData();
	record_.parentIndex = E_ParentId.GetData();
	record_.date        = E_Time.GetData();
	record_.commentary  = DE_Comment.GetData();
	record_.finished    = O_Finished.GetData();
	record_.type        = E_ActionType.GetData();

    Break(IDOK); // Breaking the modal loop
}

void ActionDlg::InitializeFields(const int type)
{
	// Fields not to be seen are hidden
	E_ActionType.Hide();
	E_PcbId.Hide();
	E_ParentId.Hide();    
	
	if (type == ActionDlg::ANALYSIS) 
	{
    	CtrlLayout(*this, t_("Analysis"));
		O_Finished.Hide();
		// no parent defined for an analysis
		record_.parentIndex = 0;
		//record_.parentKey   = 0;
	} else {
		CtrlLayout(*this, t_("Action"));
		O_Finished.Show(true);
	}	
	
    // Setting up buttons callbacks
    ok <<= THISBACK(DoOk);
    cancel <<= Rejector(IDCANCEL);
}