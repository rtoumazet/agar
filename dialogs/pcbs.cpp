#include "pcbs.h"
#include "pcb.h"

#include "../utilities/converts.h"
#include "../utilities/lookups.h"

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

	//TAB_pcbs.SetOrderBy(LABEL);
	
	TAB_pcbs.Query();
}

void PcbsDlg::OwnMenu(Bar& bar) {
	bar.Add(t_("Create"),THISBACK(Create));
	bar.Add(t_("Edit"),THISBACK(Edit));
	bar.Add(t_("Remove"),THISBACK(Remove));
}

void PcbsDlg::Create() {
	PcbDlg dlg;
	dlg.Title(t_("New PCB"));
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
                 