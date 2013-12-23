#include "origins.h"
#include "origin.h"

OriginsDlg::OriginsDlg() {

	CtrlLayout(*this, t_("Origins list"));
	BTN_Close <<= THISBACK(DoClose);
	
	TAB_origins.WhenBar = THISBACK(OwnMenu); // own menu
	TAB_origins.SetTable(ORIGIN);
	TAB_origins.AddIndex(ID);
	TAB_origins.AddColumn(NAME, t_("Name"));
	TAB_origins.AddColumn(ORIGIN,t_("Origin"));
	TAB_origins.AddColumn(COMMENTARY,t_("Comment"));
	TAB_origins.SetOrderBy(ORIGIN);
	TAB_origins.WhenLeftDouble = THISBACK(Edit);

	//TAB_pcbs.SetOrderBy(LABEL);
	
	TAB_origins.Query();
}
                 
void OriginsDlg::OwnMenu(Bar& bar) {
	bar.Add(t_("Create"),THISBACK(Create));
	bar.Add(t_("Edit"),THISBACK(Edit));
	bar.Add(t_("Remove"),THISBACK(Remove));
}

void OriginsDlg::Create() {
	OriginDlg dlg(OPENING_NEW);

	if(dlg.Execute() != IDOK)
		return;
	SQL * dlg.ctrls.Insert(ORIGIN);
	int id = SQL.GetInsertedId();
	TAB_origins.ReQuery();
	TAB_origins.FindSetCursor(id);
}

void OriginsDlg::Edit() {
	int id = TAB_origins.GetKey();
	if(IsNull(id))
		return;
	OriginDlg dlg(OPENING_EDIT);

	if(!dlg.ctrls.Load(ORIGIN, ID == id))
		return;
	if(dlg.Execute() != IDOK)
		return;
	SQL * dlg.ctrls.Update(ORIGIN).Where(ID == id);
	TAB_origins.ReQuery();
}

void OriginsDlg::Remove() {
	int id = TAB_origins.GetKey();
	if(IsNull(id) || !PromptYesNo(t_("Delete origin ?")))
	   return;
	 SQL * SqlDelete(ORIGIN).Where(ID == id);
	 TAB_origins.ReQuery();
}

void OriginsDlg::DoClose() {
	Close();
}