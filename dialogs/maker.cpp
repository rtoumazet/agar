#include "maker.h"
#include <plugin/sqlite3/Sqlite3.h>

MakerDlg::MakerDlg() {
	
	CtrlLayout(*this, t_("Makers list"));
	BTN_Close <<= Breaker(999);
	TAB_maker.WhenBar = THISBACK(OwnMenu); // own menu
	
	TAB_maker.SetTable(MAKER);
	TAB_maker.AddIndex(ID);
	TAB_maker.AddColumn(MAKER_NAME, "NAME").Edit(ES_name_);
	TAB_maker.Appending().Removing();
	TAB_maker.SetOrderBy(MAKER_NAME);
	//TAB_maker.AutoInsertId(true);
	
	TAB_maker.Query();

}

void MakerDlg::OwnMenu(Bar& bar) {
	
	bar.Add(t_("Insert"),THISBACK(MenuAdd));
	bar.Add(t_("Edit"),THISBACK(MenuEdit));
	bar.Add(t_("Remove"),THISBACK(MenuRemove));
	
}

void MakerDlg::MenuAdd() {
	
	// regular behaviour
	TAB_maker.StartInsert();
}

void MakerDlg::MenuEdit() {
	// regular behaviour
	TAB_maker.DoEdit();	
}

void MakerDlg::MenuRemove() {
	
	// behaviour is overwritten to perform checks before removing
	Sql sql;
	
	sql.Execute("select * from GAME where MAKER_ID = ?",TAB_maker.Get(ID));
	if(!sql.Fetch()) {
		// nothing's linked to that record, it can be deleted
		TAB_maker.DoRemove();
	} else {
		PromptOK(t_("Entry can't be deleted, at least one record is using it."));
	}

	//PromptOK(TAB_maker.Get(MAKER_NAME).ToString());
}