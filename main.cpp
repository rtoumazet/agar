#include "AGAR.h"

#include <Sql/sch_schema.h>
#include <Sql/sch_source.h>

#include "maker.h"

AGAR::AGAR()
{
	CtrlLayout(*this, "AGAR 1.0");
	//CtrlLayout(modify);
	
	/*Sqlite3Session sqlite3;
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

	S_MAKER maker;
	maker.MAKER_NAME = "SEGA";		
	SQL * Insert(maker);
	maker.MAKER_NAME = "CAPCOM";		
	SQL * Insert(maker);*/
	
	
		/*EditString name;
		
		tab_maker.SetTable(MAKER);
		//master.AddKey(ID);
		tab_maker.AddColumn(MAKER_NAME, "NAME").Edit(name);
		tab_maker.Appending().Removing();
		tab_maker.SetOrderBy(MAKER_NAME);
		
		tab_maker.Query();*/

	AddFrame(menu);
    menu.Set(THISBACK(MainMenu));
}

void AGAR::Exit() {

    if(PromptOKCancel("Exit AGAR ?"))

    Break();

}

void AGAR::SubMenu(Bar& bar) {
       bar.Add("Makers", THISBACK(MakerList));
       bar.Add("Exit", THISBACK(Exit));
}

void AGAR::MainMenu(Bar& bar) {
	bar.Add("Menu", THISBACK(SubMenu));
}

void AGAR::MakerList() {

	//WithMakerLayout<TopWindow> dlg;
	MakerDlg dlg;
	CtrlLayout(dlg, t_("New maker"));
	
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
/*void AGAR::MakerClose(Button& btn) {
	btn.Close();
}*/

GUI_APP_MAIN
{
	AGAR().Run();
}
