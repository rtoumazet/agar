#include "pcbs.h"
#include "pcb.h"

//#include "../utilities/converts.h"
//#include "../utilities/lookups.h"

PcbsDlg::PcbsDlg() {

	CtrlLayout(*this, t_("Pcbs list"));
	BTN_Close <<= Breaker(999);
	TAB_pcbs.WhenBar = THISBACK(OwnMenu); // own menu
	
	TAB_pcbs.AddIndex(ID);
	TAB_pcbs.AddColumn(GAME,t_("Game"));
	TAB_pcbs.AddColumn(PCB_TYPE,t_("Type"));
	TAB_pcbs.AddColumn(TAG,t_("Tag"));
	TAB_pcbs.AddColumn(LOCATION,t_("Location"));
	TAB_pcbs.WhenLeftDouble = THISBACK1(Edit,0);
	
	ReloadTable();

}

void PcbsDlg::OwnMenu(Bar& bar) {
	bar.Add(t_("Create"),THISBACK(Create));
	bar.Add(t_("Edit"),THISBACK1(Edit,0));
	bar.Add(t_("Remove"),THISBACK(Remove));
}

void PcbsDlg::Create() {
	PcbDlg dlg;
	
	// Hiding controls
	dlg.ES_Faults.Hide(); // control is hidden as it contains data to fill fault options
	dlg.TC_AnalysisAction.NoRoot(true); // root of treecontrol is hidden as there's no entry in creation
	dlg.TC_AnalysisAction.Disable(); // no action allowed on the TC during creation
	
	dlg.Title(t_("New PCB"));
	dlg.ActiveFocus(dlg.DL_Game); // sets the focus to the first droplist 
	dlg.LoadFaultData();
	
	if(dlg.Execute() != IDOK)
		return;
	dlg.GenerateFaultData();
		
	SQL * dlg.ctrls.Insert(PCB); // record is inserted in the database
	int id = SQL.GetInsertedId();
	// Do we want an initial analysis to be created ?
	if (PromptYesNo(t_("Do you want to create an initial analysis ?"))) {
		// Creation of the initial analysis
		// TODO
		dlg.AddAnalysis(id);
		Edit(id);
	}
	
	ReloadTable();
	TAB_pcbs.FindSetCursor(id);
}

void PcbsDlg::Edit(int pcbId) {
	int id = pcbId;
	if (!id) id = TAB_pcbs.GetKey();
	if(IsNull(id))
		return;
	PcbDlg dlg;
	dlg.Title(t_("Edit PCB"));
	dlg.ActiveFocus(dlg.DL_Game); // sets the focus to the first droplist 
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

	ReloadTable();
}

void PcbsDlg::Remove() {
	//TAB_pcbs.DoRemove();
	int id = TAB_pcbs.GetKey();
	if(IsNull(id) || !PromptYesNo(t_("Delete PCB ?")))
	   return;
	SQL * SqlDelete(PCB_ACTION).Where(PCB_ID == id);
	SQL * SqlDelete(PCB).Where(ID == id);

	ReloadTable();
}

void PcbsDlg::ReloadTable() {
	// reloads table content from the database
	TAB_pcbs.Clear();
	
	Sql sql;
	String statement = "select PCB.ID, MAKER.MAKER_NAME, GAME.GAME_NAME, PCB_STATE.INK, PCB_STATE.PAPER, PCB_TYPE.LABEL, TAG, LOCATION.LABEL ";
	statement += "from PCB,	GAME, MAKER, PCB_TYPE, PCB_STATE, LOCATION ";
	statement += "where pcb.game_id = game.id ";
	statement += "and game.maker_id = maker.id ";
	statement += "and pcb.pcb_type_id = pcb_type.id ";
	statement += "and pcb.pcb_state_id = pcb_state.id ";
	statement += "and pcb.location_id = location.id ";
	statement += "order by MAKER_NAME,GAME_NAME";
	sql.Execute(statement);
	while (sql.Fetch()) {
		String game = sql[1].ToString()+" "+sql[2].ToString();
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