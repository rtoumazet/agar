#include "game.h"

#include "../utilities/converts.h"
#include "../utilities/lookups.h"

GameDlg::GameDlg() {

	CtrlLayout(*this, t_("Games list"));
	BTN_Close <<= THISBACK(DoClose);
	TAB_game.WhenBar = THISBACK(OwnMenu); // own menu
	
	//Sql sql;
	SQL * Select(SqlAll()).From(MAKER).OrderBy(MAKER_NAME);
	while(SQL.Fetch()) {
		manufacturer_.Add(SQL[0],SQL[1]);
	}
	

	
	TAB_game.SetTable(GAME);
	TAB_game.AddIndex(ID);
	TAB_game.AddColumn(MAKER_ID,"Manufacturer").SetConvert(Single<Lookup(MAKER,ID,MAKER_NAME)>()).Edit(manufacturer_).HeaderTab().WhenAction = THISBACK1(SortTable,0);
	TAB_game.AddColumn(GAME_NAME,"Game name").Edit(game_name_).HeaderTab().WhenAction = THISBACK1(SortTable,1);
	TAB_game.Appending().Removing();
	TAB_game.SetOrderBy(GAME_NAME);
	game_name_.WhenEnter = THISBACK(InsertCheck);
	
	TAB_game.Query();
	

	TAB_game.SetSortColumn(1);
}

void GameDlg::OwnMenu(Bar& bar) {
	
	bar.Add(t_("Add"),THISBACK(MenuAdd));
	bar.Add(t_("Edit"),THISBACK(MenuEdit));
	bar.Add(t_("Remove"),THISBACK(MenuRemove));
	
}

void GameDlg::MenuAdd() {
	
	// regular behaviour
	TAB_game.StartInsert();
}

void GameDlg::MenuEdit() {
	// regular behaviour
	TAB_game.DoEdit();	
}

void GameDlg::MenuRemove() {
	
	// behaviour is overwritten to perform checks before removing
	Sql sql;
	
	sql.Execute("select * from PCB where GAME_ID = ?",TAB_game.Get(ID));
	if(!sql.Fetch()) {
		// nothing's linked to that record, it can be deleted
		TAB_game.DoRemove();
	} else {
		PromptOK(t_("Entry can't be deleted, at least one record is using it."));
	}
}

void GameDlg::InsertCheck() {
	// checking if the entered value doesn't already exist in the database
	String gameName = ~game_name_;
	int makerId = ~manufacturer_;

	Sql sql;
	sql.Execute("select * from GAME where MAKER_ID = ? and GAME_NAME = ?",makerId,gameName);
	if(!sql.Fetch()) {
		// entry doesn't exist in the database, it can be added
		TAB_game.Accept();
	} else {
		PromptOK(t_("Entry can't be added as it already exists in the database."));
		TAB_game.Reject();
	}	
	TAB_game.Query();
}

void GameDlg::SortTable(const int& i) {
	TAB_game.ToggleSortColumn(i);
	TAB_game.DoColumnSort();	
}

void GameDlg::DoClose() {
	Close();
}