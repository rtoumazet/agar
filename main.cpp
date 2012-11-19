#include "AGAR.h"

#include <Sql/sch_schema.h>
#include <Sql/sch_source.h>

#include "converts.h"

#include "maker.h"
#include "game.h"
#include "lookups.h"

AGAR::AGAR()
{
	CtrlLayout(*this, "AGAR 1.0 alpha");
	
	AddFrame(menu);
    menu.Set(THISBACK(MainMenu));
}

void AGAR::Exit() {

    if(PromptOKCancel("Exit AGAR ?"))

    Break();

}

void AGAR::SubMenu(Bar& bar) {
       bar.Add("Makers", THISBACK(MakerList));
       bar.Add("Games", THISBACK(GameList));
       bar.Add("Exit", THISBACK(Exit));
}

void AGAR::MainMenu(Bar& bar) {
	bar.Add("Menu", THISBACK(SubMenu));
}

void AGAR::MakerList() {

	MakerDlg dlg;
	CtrlLayout(dlg, t_("Makers list"));
	

	/*S_MAKER maker;
	maker.MAKER_NAME = "SEGA";		
	SQL * Insert(maker);
	maker.MAKER_NAME = "CAPCOM";		
	SQL * Insert(maker);*/
	
	
	EditString name;
	
	dlg.tab_maker.SetTable(MAKER);
	dlg.tab_maker.AddIndex(ID);
	dlg.tab_maker.AddColumn(MAKER_NAME, "NAME").Edit(name);
	dlg.tab_maker.Appending().Removing();
	dlg.tab_maker.SetOrderBy(MAKER_NAME);
	
	dlg.tab_maker.Query();

	dlg.Run();

}

void AGAR::GameList() {
	// Displays game table records
	
	GameDlg dlg;
	CtrlLayout(dlg, t_("Games list"));
	
	DropList maker;
	EditString gameName;

	SQL * Select(SqlAll()).From(MAKER);
	while(SQL.Fetch()) {
		maker.Add(SQL[0],SQL[1]);
	}
	
	dlg.tab_game.SetTable(GAME);
	dlg.tab_game.AddIndex(ID);
	dlg.tab_game.AddColumn(MAKER_ID,"Maker").SetConvert(Single<Lookup(MAKER,ID,MAKER_NAME)>()).Edit(maker);
	dlg.tab_game.AddColumn(GAME_NAME,"Game name").Edit(gameName);
	dlg.tab_game.Appending().Removing();
	dlg.tab_game.SetOrderBy(GAME_NAME);
	
	dlg.tab_game.Query();
	
	dlg.Run();
	
	
	
	
}

GUI_APP_MAIN
{
	// Connecting to the main database
	Sqlite3Session sqlite3;
	if(!sqlite3.Open(ConfigFile("AGAR.db"))) {
		Cout() << "Can't create or open database file\n";
		return;
	}

	#ifdef _DEBUG
	sqlite3.SetTrace();
	#endif
	
	SQL = sqlite3;
	
	SqlSchema sch(SQLITE3);
	All_Tables(sch);
	SqlPerformScript(sch.Upgrade());
	SqlPerformScript(sch.Attributes());
	SQL.ClearError();	
	

	// Entering the main window modal loop
	AGAR().Run();
}
