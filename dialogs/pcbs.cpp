#include "pcbs.h"
#include "pcb.h"

//#include "../utilities/converts.h"
//#include "../utilities/lookups.h"

PcbsDlg::PcbsDlg() {

	CtrlLayout(*this, t_("Pcbs list"));
	BTN_Close <<= Breaker(999);
	TAB_pcbs.WhenBar = THISBACK(OwnMenu); // own menu
	
	TAB_pcbs.SetTable(PCB);
	TAB_pcbs.AddIndex(ID);
	TAB_pcbs.AddColumn(GAME_ID,t_("Game"));
	TAB_pcbs.AddColumn(PCB_TYPE_ID,t_("Type"));
	TAB_pcbs.AddColumn(PCB_STATE_ID, t_("State"));
	TAB_pcbs.AddColumn(TAG,t_("Tag"));
	TAB_pcbs.AddColumn(LOCATION_ID,t_("Location"));
	TAB_pcbs.WhenLeftDouble = THISBACK(Edit);

	TAB_pcbs.Query();
	
	//TAB_pcbs.HeaderObject().HideTab(2); // state is hidden
	// formatting text
	String str;
	for (int i=0; i<TAB_pcbs.GetCount(); i++) {
		str = TAB_pcbs.Get(i,1).ToString();
		
		
	}
	
/*	Sql sql;
	sql.Execute("select ID,GAME_ID,PCB_TYPE_ID,PCB_STATE_ID,TAG,LOCATION_ID from PCB);
	while (sql.Fetch() {
		
		
		
		TAB_pcbs.Add(
			sql[0], // record id.
			
	}
	
	sql.Execute("select ID,LABEL,INK,PAPER from PCB_STATE");
	while(sql.Fetch()) {
		ink = Color::FromRaw(static_cast<dword>(sql[2].To<int64>()));
		paper = Color::FromRaw(static_cast<dword>(sql[3].To<int64>()));
		
		TAB_pcbStateArray.Add(
			sql[0], // record id
			AttrText(sql[1].ToString()).Ink(ink).Paper(paper), // formatted text
			sql[1].ToString(), // raw text
			paper, // background color
			ink // text color
		);
	}*/
}

void PcbsDlg::OwnMenu(Bar& bar) {
	bar.Add(t_("Create"),THISBACK(Create));
	bar.Add(t_("Edit"),THISBACK(Edit));
	bar.Add(t_("Remove"),THISBACK(Remove));
}

void PcbsDlg::Create() {
	PcbDlg dlg;
	dlg.Title(t_("New PCB"));
	dlg.ActiveFocus(dlg.DL_Game); // sets the focus to the first droplist 
	if(dlg.Execute() != IDOK)
		return;
	SQL * dlg.ctrls.Insert(PCB);
	int id = SQL.GetInsertedId();
	TAB_pcbs.ReQuery();
	TAB_pcbs.FindSetCursor(id);
}

void PcbsDlg::Edit() {
	int id = TAB_pcbs.GetKey();
	if(IsNull(id))
		return;
	PcbDlg dlg;
	dlg.Title(t_("Edit PCB"));
	dlg.ActiveFocus(dlg.DL_Game); // sets the focus to the first droplist 
	if(!dlg.ctrls.Load(PCB, ID == id))
		return;
	if(dlg.Execute() != IDOK)
		return;
	SQL * dlg.ctrls.Update(PCB).Where(ID == id);
	TAB_pcbs.ReQuery();
}

void PcbsDlg::Remove() {
	//TAB_pcbs.DoRemove();
	int id = TAB_pcbs.GetKey();
	if(IsNull(id) || PromptYesNo(t_("Delete PCB ?")))
	   return;
	 SQL * SqlDelete(PCB).Where(ID == id);
	 TAB_pcbs.ReQuery();
}
                 