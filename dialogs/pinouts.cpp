#include "pinouts.h"
#include "pinout.h"

PinoutsDlg::PinoutsDlg() {

	CtrlLayout(*this, t_("Pinouts list"));
	BTN_Close <<= Breaker(999);
	
	TAB_pinouts.WhenBar = THISBACK(OwnMenu); // own menu
	TAB_pinouts.SetTable(PINOUT);
	TAB_pinouts.AddIndex(ID);
	TAB_pinouts.AddColumn(LABEL, t_("Label"));
	TAB_pinouts.AddColumn(PIN_SIZE,t_("Pin size"));
	TAB_pinouts.AddColumn(DETAIL,t_("Detail"));
	TAB_pinouts.SetOrderBy(LABEL);
	TAB_pinouts.WhenLeftDouble = THISBACK(Edit);

	TAB_pinouts.Query();
}
                 
void PinoutsDlg::OwnMenu(Bar& bar) {
	bar.Add(t_("Create"),THISBACK(Create));
	bar.Add(t_("Edit"),THISBACK(Edit));
	bar.Add(t_("Remove"),THISBACK(Remove));
}

void PinoutsDlg::Create() {
	PinoutDlg dlg(OPENING_NEW);

	if(dlg.Execute() != IDOK)
		return;
	SQL * dlg.ctrls.Insert(PINOUT);
	int id = SQL.GetInsertedId();
	TAB_pinouts.ReQuery();
	TAB_pinouts.FindSetCursor(id);
}

void PinoutsDlg::Edit() {
	int id = TAB_pinouts.GetKey();
	if(IsNull(id))
		return;
	PinoutDlg dlg(OPENING_EDIT);

	if(!dlg.ctrls.Load(PINOUT, ID == id))
		return;
	if(dlg.Execute() != IDOK)
		return;
	SQL * dlg.ctrls.Update(PINOUT).Where(ID == id);
	TAB_pinouts.ReQuery();
}

void PinoutsDlg::Remove() {
	int id = TAB_pinouts.GetKey();
	if(IsNull(id) || !PromptYesNo(t_("Delete pinout ?")))
	   return;
	 SQL * SqlDelete(PINOUT).Where(ID == id);
	 TAB_pinouts.ReQuery();
}