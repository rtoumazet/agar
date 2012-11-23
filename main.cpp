#include "AGAR.h"

#include <Sql/sch_schema.h>
#include <Sql/sch_source.h>

#include "dialogs/maker.h"
#include "dialogs/game.h"
#include "dialogs/pcbtype.h"


SqlId count("count(*)");

AGAR::AGAR()
{
	CtrlLayout(*this, "AGAR 1.0 alpha");
	
	AddFrame(MB_menu_);
    MB_menu_.Set(THISBACK(MainMenu));
    
    DatabaseInit();
}

void AGAR::DatabaseInit() {

	// Various database init
	Sql sql;
	
	// PCB_TYPE default values
	sql.Execute("select count(*) from sqlite_master where type='table' and name='PCB_TYPE';");
	sql.Fetch();
	if(!sql[0]) {
		// table doesn't exist, it's created and populated
		sql.Execute("create table PCB_TYPE;");
		
		S_PCB_TYPE type;
		type.LABEL = "Original";
		sql * Insert(type);
		type.LABEL = "Bootleg";
		sql * Insert(type);
		type.LABEL = "Conversion";
		sql * Insert(type);
		
	}
	
	
}

void AGAR::Exit() {

    if(PromptOKCancel("Exit AGAR ?"))
    Break();
}

void AGAR::MainMenu(Bar& bar) {
	bar.Add("Menu", THISBACK(SubMenuMain));
	bar.Add("Options", THISBACK(SubMenuOptions));
}

void AGAR::SubMenuMain(Bar& bar) {
       bar.Add("Makers", THISBACK(MakerList));
       bar.Add("Games", THISBACK(GameList));
       bar.Add("Exit", THISBACK(Exit));
}

void AGAR::SubMenuOptions(Bar& bar) {
       bar.Add("PCB Types", THISBACK(PcbType));
}

void AGAR::MakerList() {

	MakerDlg dlg;
	
	dlg.Run();
}

void AGAR::RemoveMaker() {
	PromptOK("Remove ?");
}

void AGAR::GameList() {
	
	// Displays game table records
	
	GameDlg dlg;
		
	dlg.Run();
}

void AGAR::PcbType() {
	
	// Displays PCB types
	PcbTypeDlg dlg;
	
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
