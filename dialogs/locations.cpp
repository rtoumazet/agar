#include "Locations.h"
#include "Location.h"

LocationsDlg::LocationsDlg() {

	CtrlLayout(*this, t_("Locations list"));
	BTN_Close <<= Breaker(999);
	
	TAB_locations.WhenBar = THISBACK(OwnMenu); // own menu
	TAB_locations.SetTable(LOCATION);
	TAB_locations.AddIndex(ID);
	TAB_locations.AddColumn(LABEL, t_("Label"));
	TAB_locations.WhenLeftDouble = THISBACK(Edit);

	TAB_locations.Query();
}
                 
void LocationsDlg::OwnMenu(Bar& bar) {
	bar.Add(t_("Create"),THISBACK(Create));
	bar.Add(t_("Edit"),THISBACK(Edit));
	bar.Add(t_("Remove"),THISBACK(Remove));
}

void LocationsDlg::Create() {
	LocationDlg dlg;
	dlg.Title(t_("New origin"));
	dlg.ActiveFocus(dlg.LE_Label); // setting focus
	if(dlg.Execute() != IDOK)
		return;
	SQL * dlg.ctrls.Insert(LOCATION);
	int id = SQL.GetInsertedId();
	TAB_locations.ReQuery();
	TAB_locations.FindSetCursor(id);
}

void LocationsDlg::Edit() {
	int id = TAB_locations.GetKey();
	if(IsNull(id))
		return;
	LocationDlg dlg;
	dlg.Title(t_("Edit location"));
	dlg.ActiveFocus(dlg.LE_Label); // setting focus
	if(!dlg.ctrls.Load(LOCATION, ID == id))
		return;
	if(dlg.Execute() != IDOK)
		return;
	SQL * dlg.ctrls.Update(LOCATION).Where(ID == id);
	TAB_locations.ReQuery();
}

void LocationsDlg::Remove() {
	int id = TAB_locations.GetKey();
	if(IsNull(id) || PromptYesNo(t_("Delete location ?")))
	   return;
	 SQL * SqlDelete(LOCATION).Where(ID == id);
	 TAB_locations.ReQuery();
}