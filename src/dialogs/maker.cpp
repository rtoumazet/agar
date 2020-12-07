#include "maker.h"
#include <plugin/sqlite3/Sqlite3.h>

MakerDlg::MakerDlg()
{
	CtrlLayout(*this, t_("Manufacturers list"));
	BTN_Close <<= THISBACK(DoClose);
	TAB_maker.WhenBar = THISBACK(OwnMenu); // own menu
	
	TAB_maker.SetTable(MAKER);
	TAB_maker.AddIndex(ID);
	TAB_maker.AddColumn(MAKER_NAME, "NAME").Edit(ES_name_);
	TAB_maker.Appending().Removing();
	TAB_maker.SetOrderBy(MAKER_NAME);
	ES_name_.WhenEnter = THISBACK(InsertCheck);
	TAB_maker.Query();
}

void MakerDlg::OwnMenu(Bar& bar)
{
	bar.Add(t_("Add"),THISBACK(MenuAdd));
	bar.Add(t_("Edit"),THISBACK(MenuEdit));
	bar.Add(t_("Remove"),THISBACK(MenuRemove));
}

void MakerDlg::MenuAdd()
{
	// regular behaviour
	TAB_maker.StartInsert();
}

void MakerDlg::MenuEdit()
{
	// regular behaviour
    TAB_maker.DoEdit();
}

void MakerDlg::MenuRemove()
{
	// behaviour is overwritten to perform checks before removing
	Sql sql;
	sql.Execute("select * from GAME where MAKER_ID = ?",TAB_maker.Get(ID));
	if (!sql.Fetch()) {
		// nothing's linked to that record, it can be deleted
		TAB_maker.DoRemove();
	} else {
		PromptOK(t_("Entry can't be deleted, at least one record is using it."));
	}
}

void MakerDlg::InsertCheck()
{
	// checking if the entered value doesn't already exist in the database
	auto const str = ES_name_.GetData().ToString();

	Sql sql;
	sql.Execute("select * from MAKER where MAKER_NAME = ?",str);
	if (!sql.Fetch()) {
		// entry doesn't exist in the database, it can be added
		TAB_maker.Accept();
	} else {
		PromptOK(t_("Entry can't be added as it already exists in the database."));
		TAB_maker.Reject();
	}
	TAB_maker.Query();
}

void MakerDlg::DoClose()
{
	Close();
}