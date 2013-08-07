#include "pcbs.h"
#include "pcb.h"

//#include "../utilities/converts.h"
//#include "../utilities/lookups.h"

PcbsDlg::PcbsDlg() {

	CtrlLayout(*this, t_("Pcbs list"));
	BTN_Close <<= Breaker(999);
	TAB_pcbs.WhenBar = THISBACK(OwnMenu); // own menu
	
	TAB_pcbs.AddIndex(ID);
	TAB_pcbs.AddColumn(GAME,t_("Game")).HeaderTab().WhenAction = THISBACK1(SortTable, 0);
	TAB_pcbs.AddColumn(PCB_TYPE,t_("Type")).HeaderTab().WhenAction = THISBACK1(SortTable, 1);
	TAB_pcbs.AddColumn(TAG,t_("Tag")).HeaderTab().WhenAction = THISBACK1(SortTable, 2);
	TAB_pcbs.AddColumn(LOCATION,t_("Location")).HeaderTab().WhenAction = THISBACK1(SortTable, 3);
	TAB_pcbs.WhenLeftDouble = THISBACK1(Edit,0);
	TAB_pcbs.ColumnWidths("346 82 81 82");
	
	// Filters
	// state droplist
	filterState_ = 0; // state selected is reset
	DL_State.Add(0, t_("All")); // First line of the dropbox

	Sql sql;
	sql * Select(ID,LABEL,PAPER,INK).From(PCB_STATE);
	while(sql.Fetch()) {
		DL_State.Add(
		    sql[ID],
			AttrText(sql[LABEL].ToString())
				.Paper(Color::FromRaw(static_cast<dword>(sql[PAPER].To<int64>())))
				.Ink(Color::FromRaw(static_cast<dword>(sql[INK].To<int64>())))
		);
	}		
	DL_State.WhenAction = THISBACK(ExecuteFilter);
	DL_State.SetIndex(0); // first line is selected
	
	LoadFaultData();
	for (int i=0; i<option_.GetCount(); i++) {
		option_[i].WhenAction = THISBACK(ExecuteFilter);
	}
	
	// Table is filled and sorted
	isSortedAsc_ = true;
	ReloadTable(isSortedAsc_);
}

void PcbsDlg::OwnMenu(Bar& bar) {
	bar.Add(t_("Create"),THISBACK(Create));
	bar.Add(t_("Edit"),THISBACK1(Edit,0));
	bar.Add(t_("Remove"),THISBACK(Remove));
}

void PcbsDlg::Create() {
	PcbDlg dlg(OPENING_NEW);
	
	dlg.LoadFaultData();
	
	if(dlg.Execute() != IDOK)
		return;
	dlg.GenerateFaultData();
	
	dlg.ES_FaultsOrigin.SetData(~dlg.ES_Faults);
		
	SQL * dlg.ctrls.Insert(PCB); // record is inserted in the database
	int id = SQL.GetInsertedId();
	// Do we want an initial analysis to be created ?
	if (PromptYesNo(t_("Do you want to create an initial analysis ?"))) {
		// Creation of the initial analysis
		// TODO
		dlg.AddAnalysis(id);
		Edit(id);
	}
	
	ReloadTable(true);
	TAB_pcbs.FindSetCursor(id);
}

void PcbsDlg::Edit(int pcbId) {
	int id = pcbId;
	if (!id) id = TAB_pcbs.GetKey();
	if(IsNull(id))
		return;
	PcbDlg dlg(OPENING_EDIT);

	if(!dlg.ctrls.Load(PCB, ID == id))
		return;
	
	dlg.LoadFaultData();
	dlg.BuildActionTree(id);
	if (!dlg.GetRecordNumber(id)) {
		// no record for this pcb
		dlg.TC_AnalysisAction.NoRoot(true); // root of treecontrol is hidden as there's nothing to display
		dlg.SetAddActionMenuEntryVisible(false);
		dlg.SetEditMenuEntryVisible(false);
	}
	if(dlg.Execute() != IDOK)
		return;
	dlg.GenerateFaultData();
	
	SQL * dlg.ctrls.Update(PCB).Where(ID == id);

	ReloadTable(true);
}

void PcbsDlg::Remove() {
	//TAB_pcbs.DoRemove();
	int id = TAB_pcbs.GetKey();
	if(IsNull(id) || !PromptYesNo(t_("Delete PCB ?")))
	   return;
	SQL * SqlDelete(PCB_ACTION).Where(PCB_ID == id);
	SQL * SqlDelete(PCB).Where(ID == id);

	ReloadTable(true);
}

void PcbsDlg::SortTable(const int& i) {
	TAB_pcbs.ToggleSortColumn(i);
	TAB_pcbs.DoColumnSort();	
	if (!i) {
		// Sorting is manualle done through table reload for the first column as it's not possible to sort directly
		// AttrText data from a column directly.
		if (isSortedAsc_) isSortedAsc_ = false;
		else isSortedAsc_ = true;

		ReloadTable(isSortedAsc_);
	}
}

void PcbsDlg::ReloadTable(const bool& ascSort) {
	// reloads table content from the database
	TAB_pcbs.Clear();
	
	Sql sql;
	String statement = "select PCB.ID, MAKER.MAKER_NAME, GAME.GAME_NAME, PCB_STATE.INK, PCB_STATE.PAPER, PCB_TYPE.LABEL, TAG, LOCATION.LABEL ";
	statement += "from PCB,	GAME, MAKER, PCB_TYPE, PCB_STATE ";
	statement += "left outer join LOCATION ";
	statement += "on pcb.location_id = location.id ";
	statement += "where pcb.game_id = game.id ";
	statement += "and game.maker_id = maker.id ";
	statement += "and pcb.pcb_type_id = pcb_type.id ";
	statement += "and pcb.pcb_state_id = pcb_state.id ";
	//statement += "and pcb.location_id = location.id ";
	// State filter
	if (filterState_) statement += Format("and pcb.pcb_state_id = %1 ",filterState_);
	// Faults filter
	Value key;
	String str = "";
	for (int i=0; i<option_.GetCount(); i++) {
		if (option_[i]){
			// option is checked, statement is modified
			key = option_.GetKey(i);
			str = "\"%";
			str += key.ToString();
			str += ":1%\"";

			statement += Format(" and pcb.pcb_fault_option like % ",str);
		}
	}

	
	if (ascSort) statement += " order by MAKER_NAME asc,GAME_NAME asc";
	else statement += " order by MAKER_NAME desc,GAME_NAME desc";
	sql.Execute(statement);
	while (sql.Fetch()) {
		String game = sql[1].ToString()+" - "+sql[2].ToString();
		Color ink = Color::FromRaw(static_cast<dword>(sql[3].To<int64>()));
		Color paper = Color::FromRaw(static_cast<dword>(sql[4].To<int64>()));
		
		AttrText atGame = AttrText(game).Ink(ink).Paper(paper);
		TAB_pcbs.Add(
			sql[0], // pcb id.
			atGame, // maker + game formatted
			sql[5], // type
			sql[6], // tag
			sql[7]  // location
		);
	}	
}

void PcbsDlg::ExecuteFilter() {
	// State filter
	filterState_ = ~DL_State;
	
	// Faults filter
	
	
	// Table is reloaded
	ReloadTable(isSortedAsc_);
}

void PcbsDlg::LoadFaultData() {
	// fault option list
	Sql sql;
	Rect r = L_Faults.GetRect();
	int y = r.top + 20;
	int linecy = Draw::GetStdFontCy() + 4;
	int current = 0; 
	sql * Select(ID,LABEL).From(PCB_FAULT).OrderBy(LABEL);
	while(sql.Fetch()) {
		
		//Add(option_.Add(sql[ID]).SetLabel(sql[LABEL].ToString()).TopPos(y, linecy).RightPos(10, 200));
		Add(option_.Add(sql[ID]).SetLabel(sql[LABEL].ToString()).TopPos(y, linecy).LeftPos(r.left+10,200));
		int id = StdConvertInt().Scan(sql[ID].ToString());
		y += linecy;
		current++;
	}	
}