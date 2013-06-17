#include "pcb.h"
#include "action.h"

#define IMAGECLASS MyImages
#define IMAGEFILE  "agar/images.iml"
#include <Draw/iml_source.h>

#include "agar/utilities/converts.h"
#include "agar/utilities/lookups.h"

PcbDlg::PcbDlg() {

	CtrlLayoutOKCancel(*this, t_("Pcb"));
	
	// Hiding controls not to be displayed
	E_PcbId.Hide();
	ES_Faults.Hide();

	// Filling droplists data
	Sql sql;
	// Game droplist
	sql.Execute("select GAME.ID, MAKER_NAME, GAME_NAME from MAKER,GAME where GAME.MAKER_ID = MAKER.ID order by MAKER_NAME,GAME_NAME");
	while(sql.Fetch()) {
		String temp = sql[1].ToString() + ' - ' + sql[2].ToString();
		DL_Game.Add(
			sql[0],
			temp
		);
	}
	
	// type droplist
	sql.Execute("select ID,LABEL from PCB_TYPE");
	while(sql.Fetch()) {
		DL_Type.Add(
			sql[0],
			sql[1].ToString()
		);
	}	
	
	// state droplist
	sql.Execute("select ID,LABEL,PAPER,INK from PCB_STATE");
	while(sql.Fetch()) {
		DL_State.Add(
		    sql[0],
			AttrText(sql[1].ToString())
				.Paper(Color::FromRaw(static_cast<dword>(sql[2].To<int64>())))
				.Ink(Color::FromRaw(static_cast<dword>(sql[3].To<int64>())))
		);
	}	

	// location droplist
	DL_Location.Add(0,t_("Not selected"));
	sql.Execute("select ID,LABEL from LOCATION");
	while(sql.Fetch()) {
		DL_Location.Add(
			sql[0],
			sql[1].ToString()
		);
	}
	DL_Location.SetIndex(0);
	DL_Location.NotNull(true);

	// origin droplist
	DL_Origin.Add(0,t_("Not selected"));
	sql.Execute("select ID,ORIGIN from ORIGIN");
	while(sql.Fetch()) {
		DL_Origin.Add(
			sql[0],
			sql[1].ToString()
		);
	}
	DL_Origin.SetIndex(0);
	DL_Origin.NotNull(true);
	

	// Tree control
	TC_AnalysisAction.WhenBar = THISBACK(TreeControlMenu);
	
	ctrls // manual declaration
		(ID, E_PcbId)
		(PCB_STATE_ID, DL_State)
		(ORIGIN_ID, DL_Origin)
		(LOCATION_ID, DL_Location)
		(PCB_TYPE_ID, DL_Type)
		(GAME_ID, DL_Game)
		(LAST_TEST_DATE, D_LastTestDate)
		(TAG, ES_Tag)
		(PCB_FAULT_OPTION, ES_Faults)
	;
}

void PcbDlg::GenerateFaultData() {
	// builds the string containing checked values
	String faults = "";
	Value key, data;
	for (int i=0; i<option.GetCount(); i++) {
		key = option.GetKey(i);
		data = option[i].GetData();
		faults += key.ToString() +":";
		faults += data.ToString() +";";
	}
	ES_Faults = faults;
}

bool PcbDlg::GetFaultValue(const int& id) {
	// returns the option state for the fault id parameter

	Sql sql;
	int startPos = 0;
	int endPos = 0;
	bool ret = false;
	String str = ES_Faults;
	String subStr = "";
	endPos = str.Find(";",startPos);
	while (endPos != -1) {
		subStr = str.Mid(startPos, endPos - startPos);
		int end = subStr.Find(":");
		int val = StdConvertInt().Scan(subStr.Mid(0,end));
		if (val == id) {
			// id was found, getting the option value
			ret = StdConvertInt().Scan(subStr.Mid(end+1));
			break;	
		}

		// moving to the next chunk
		startPos = endPos + 1;
		endPos = str.Find(";",startPos);
	}	
	
	return ret;
}

void PcbDlg::LoadFaultData() {
	// fault option list
	Sql sql;
	Rect r = L_Faults.GetRect();
	int y = r.top + 20;
	int linecy = Draw::GetStdFontCy() + 4;
	int current = 0; 
	sql.Execute("select ID,LABEL from PCB_FAULT order by LABEL");
	while(sql.Fetch()) {
		//Add(option.Add(sql[0]).SetLabel(sql[1].ToString()).TopPos(y, linecy).RightPos(10, 150));
		Add(option.Add(sql[0]).SetLabel(sql[1].ToString()).TopPos(y, linecy).LeftPos(r.left+10, 150));
		int id = StdConvertInt().Scan(sql[0].ToString());
		option[current].SetData(GetFaultValue(id));
		y += linecy;
		current++;
	}	
}

void PcbDlg::TreeControlMenu(Bar& bar) {
	// Setting options to display
	int id = TC_AnalysisAction.GetCursor();
	if (id != -1 && id != 0) {
		SetAddActionMenuEntryVisible(true);
		SetEditMenuEntryVisible(true);
		if (TC_AnalysisAction.GetChildCount(id) || TC_AnalysisAction.GetCursor() == 0) {
			// At least one child exists for the selected node, removal isn't allowed
			// Root removal isn't allowed either
			SetRemoveMenuEntryVisible(false);
		} else {
			SetRemoveMenuEntryVisible(true);
		}
	} else {
		// Nothing's selected, menu options are greyed
		SetAddActionMenuEntryVisible(false);
		SetEditMenuEntryVisible(false);
		SetRemoveMenuEntryVisible(false);
	}

	bar.Add(t_("Add analysis"),THISBACK1(AddAnalysis, ~E_PcbId));
	bar.Add(DisplayAddActionMenuEntry(), t_("Link an action"), THISBACK1(AddAction, ~E_PcbId));
	bar.Add(DisplayEditMenuEntry(), t_("Edit"), THISBACK(Edit));
	bar.Add(DisplayRemoveMenuEntry(), t_("Remove"), THISBACK(Remove));
}

void PcbDlg::Edit() {
	// Edits currently selected analysis / action in the treecontrol

	int key = TC_AnalysisAction.Get();
	if(IsNull(key))
		return;
	
	ActionDlg dlg(~E_PcbId, key);
	
	if(dlg.Execute() != IDOK)
		return;
	
	SQL * dlg.ctrls.Update(PCB_ACTION).Where(ID == key);
	
	BuildActionTree(~E_PcbId);	
};

void PcbDlg::Remove() {
	// Removes currectly selected analysis / action in the treecontrol
	
	int key = TC_AnalysisAction.Get();
	if(IsNull(key) || !PromptYesNo(t_("Delete entry ?")))
	   return;
	SQL * SqlDelete(PCB_ACTION).Where(ID == key);	
	
	// Tree is rebuilt
	BuildActionTree(~E_PcbId);
}

void PcbDlg::AddAnalysis(const int& pcbId) {
	// Adds a new analysis
	ActionDlg dlg(pcbId, ActionDlg::ANALYSIS, ActionDlg::CREATION);
	if(dlg.Execute() != IDOK)
		return;

	SQL * dlg.ctrls.Insert(PCB_ACTION);
	if(SQL.WasError()){
    	PromptOK(SQL.GetLastError());
	}
	int id = SQL.GetInsertedId();
	
	BuildActionTree(pcbId);
};

void PcbDlg::AddAction(const int& pcbId) {
	// Adds a new action linked to an analysis
	ActionDlg dlg(pcbId, ActionDlg::ACTION, ActionDlg::CREATION, TC_AnalysisAction.GetCursor());
	
	if(dlg.Execute() != IDOK)
		return;

	SQL * dlg.ctrls.Insert(PCB_ACTION);
	if(SQL.WasError()){
    	PromptOK(SQL.GetLastError());
    	//PromptOK(SQL.GetErrorStatement());
	}
	int id = SQL.GetInsertedId();
	
	BuildActionTree(pcbId);	
};

int PcbDlg::GetRecordNumber(int const& pcbId) {
	// returns number of record from action table for the pcb id in parameter
	
	int count = 0;
	
	Sql sql;
	sql.Execute(Format("select count(*) from PCB_ACTION where PCB_ID =%i",pcbId));
	if (sql.Fetch()) {
		count = sql[0];
	}
	
	return count;
}

bool PcbDlg::DisplayAddActionMenuEntry() {
	
	return addActionMenuEntryVisible_;
}

bool PcbDlg::DisplayEditMenuEntry() {
	
	return editMenuEntryVisible_;	
}

bool PcbDlg::DisplayRemoveMenuEntry() {
	
	return removeMenuEntryVisible_;	
}

void PcbDlg::SetAddActionMenuEntryVisible(const bool& val) {
	addActionMenuEntryVisible_ = val;
}

void PcbDlg::SetEditMenuEntryVisible(const bool& val) {
	editMenuEntryVisible_ = val;
}

void PcbDlg::SetRemoveMenuEntryVisible(const bool& val) {
	removeMenuEntryVisible_ = val;
}

void PcbDlg::BuildActionTree(const int& pcbId) {
	// Fills the tree control with data from action file

	TC_AnalysisAction.Clear();
		
	Sql sql;
	sql.Execute(Format("select ID,PARENT_ID,COMMENTARY,FINISHED from PCB_ACTION where PCB_ID = %i order by ACTION_DATE",pcbId));
	
	TC_AnalysisAction.SetRoot(Null,0,t_("Analysis & Actions"));
	TC_AnalysisAction.NoRoot(false);

	while(sql.Fetch()) {
		Image img;
		if (sql[PARENT_ID]==0) {
			// Analysis
			img = MyImages::analysis();
		} else {
			if (sql[FINISHED]=="0") {
				img = MyImages::action();
			} else {
				 img = MyImages::actionDone();
			}
		}
		
		TC_AnalysisAction.Add(sql[PARENT_ID], img, sql[ID], sql[COMMENTARY]);
	}	
	
	TC_AnalysisAction.OpenDeep(0,true);
	
}