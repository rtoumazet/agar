#include "locations.h"

LocationsDlg::LocationsDlg() {

	CtrlLayout(*this, t_("Locations list"));
	BTN_Close <<= THISBACK(DoClose);
	
	TAB_locations.SetTable(LOCATION);
	TAB_locations.AddIndex(ID);
	TAB_locations.AddColumn(LABEL, t_("Label")).Edit(location_);
	TAB_locations.SetOrderBy(LABEL);
	
	TAB_locations.WhenLeftDouble = THISBACK(MenuEdit);
	TAB_locations.WhenBar = THISBACK(OwnMenu);
	
	TAB_locations.Query();
}
                 
void LocationsDlg::OwnMenu(Bar& bar) {
	bar.Add(t_("Add"),THISBACK(MenuAdd));
	bar.Add(t_("Edit"),THISBACK(MenuEdit));
	bar.Add(t_("Remove"),THISBACK(MenuRemove));
}

void LocationsDlg::MenuAdd() {
	TAB_locations.StartInsert();
}

void LocationsDlg::MenuEdit() {
	TAB_locations.DoEdit();
}

void LocationsDlg::MenuRemove() {
	int id = TAB_locations.GetKey();
	if(IsNull(id) || !PromptYesNo(t_("Delete location ?")))
	   return;
	 SQL * SqlDelete(LOCATION).Where(ID == id);
	 TAB_locations.ReQuery();
}

void LocationsDlg::DoClose() {
	Close();
}