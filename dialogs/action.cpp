#include "action.h"


ActionDlg::ActionDlg(const int pcb_id, const ItemType type, int parent_id) {
	InitializeFields(type);
    
	E_ParentId	<<= parent_id; // 0 for analysis, param value for action
	
	// Setting up default values
	E_Time      <<= GetSysTime();
	E_PcbId     <<= pcb_id;
	E_ActionType<<= static_cast<int>(type);
	
	ActiveFocus(DE_Comment);
}

ActionDlg::ActionDlg(const ActionRecord& ar) {
    InitializeFields(ar.type);
	
	// Fields are initialized from constructor parameter
	E_PcbId         <<= ar.pcb_id;
	E_ParentId      <<= ar.parent_index;
	E_Time          <<= ar.date;
	DE_Comment      <<= ar.commentary;
	O_Finished      <<= ar.finished;
	E_ActionType    <<= static_cast<int>(ar.type);
	
	Record(ar); // initializing internal record

	ActiveFocus(DE_Comment); // focus is set to the comment field
}

void ActionDlg::DoOk() {
    // Saves modified data to the current record
	record_.pcb_id          = ~E_PcbId;
	record_.parent_index    = ~E_ParentId;
	record_.date            = ~E_Time;
	record_.commentary      = ~DE_Comment;
	record_.finished        = ~O_Finished;
	record_.type            = ItemType(static_cast<int>(~E_ActionType));

    Break(IDOK); // Breaking the modal loop
}

void ActionDlg::InitializeFields(const ItemType type) {
	// Fields not to be seen are hidden
	E_ActionType.Hide();
	E_PcbId.Hide();
	E_ParentId.Hide();
	
	if (type == ItemType::analysis)
	{
    	CtrlLayout(*this, t_("Analysis"));
		O_Finished.Hide();
		// no parent defined for an analysis
		record_.parent_index = 0;
		//record_.parentKey   = 0;
	} else {
		CtrlLayout(*this, t_("Action"));
		O_Finished.Show(true);
	}	
	
    // Setting up buttons callbacks
    ok <<= THISBACK(DoOk);
    cancel <<= Rejector(IDCANCEL);
}