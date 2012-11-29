#include "maker.h"

MakerDlg::MakerDlg() {
	
	CtrlLayout(*this, t_("Makers list"));
	BTN_Close <<= Breaker(999);
	TAB_maker.WhenBar = THISBACK(OwnMenu); // own menu
	
	TAB_maker.SetTable(MAKER);
	TAB_maker.AddIndex(ID);
	TAB_maker.AddColumn(MAKER_NAME, "NAME").Edit(ES_name_);
	//TAB_maker.Appending().Removing();
	TAB_maker.SetOrderBy(MAKER_NAME);
	
	TAB_maker.Query();

}

void MakerDlg::OwnMenu(Bar& bar) {
	
	bar.Add("Insert record",THISBACK(MenuAdd));
	bar.Add("Edit record",THISBACK(MenuEdit));
	bar.Add("Remove record",THISBACK(MenuRemove));
	
}

void MakerDlg::MenuAdd() {
	
	// regular behaviour
	TAB_maker.DoAppend(); 
}

void MakerDlg::MenuEdit() {
	// regular behaviour
	TAB_maker.DoEdit();	
}

void MakerDlg::MenuRemove() {
	
	// behaviour is overwritten to perform checks before removing
	//Sql slq;
	
	//sql.Execute("select * from GAME where MAKER_ID = ");
	
	//TAB_maker.DoRemove();
}