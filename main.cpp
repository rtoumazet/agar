#include "AGAR.h"

#include <Sql/sch_schema.h>
#include <Sql/sch_source.h>

#include "dialogs/maker.h"
#include "dialogs/game.h"
#include "dialogs/origins.h"
#include "dialogs/pcbtype.h"
#include "dialogs/pcbstate.h"
#include "dialogs/pcbfault.h"
#include "dialogs/pcbs.h"


SqlId count("count(*)");

AGAR::AGAR()
{
	SetDateFormat("%3:02d/%2:02d/%1:4d"); // set dd/mm/yyyy format
	SetDateScan("dmy"); // same but for input
	
	CtrlLayout(*this, "AGAR 1.0 alpha");
	
	AddFrame(MB_menu_);
    MB_menu_.Set(THISBACK(MainMenu));
    
    DatabaseInit();
}

void AGAR::DatabaseInit() {

	// Various database init
	Sql sql;
	
	// PCB_TYPE default values
	sql.Execute("select * from PCB_TYPE;");
	if(!sql.Fetch()) {
		S_PCB_TYPE type;
		type.LABEL = "Original";
		sql * Insert(type);
		type.LABEL = "Bootleg";
		sql * Insert(type);
		type.LABEL = "Conversion";
		sql * Insert(type);
	}

	// PCB_STATE default values	
	sql.Execute("select * from PCB_STATE;");
	if(!sql.Fetch()) {
		S_PCB_STATE state;
		state.LABEL = "Fully working";
		sql * Insert(state);
		state.LABEL = "Working with things to be done";
		sql * Insert(state);
		state.LABEL = "Incomplete";
		sql * Insert(state);
		state.LABEL = "Work in progress";
		sql * Insert(state);
		state.LABEL = "Difficulties";
		sql * Insert(state);
		state.LABEL = "Dead";
		sql * Insert(state);
		state.LABEL = "For parts";
		sql * Insert(state);
		state.LABEL = "Untested";
		sql * Insert(state);
	}
	
	// PCB_FAULT default values
	sql.Execute("select * from PCB_FAULT;");
	if(!sql.Fetch()) {
		S_PCB_FAULT fault;
		fault.LABEL = "Other";
		sql * Insert(fault);
		fault.LABEL = "Controls";
		sql * Insert(fault);
		fault.LABEL = "Graphics";
		sql * Insert(fault);
		fault.LABEL = "Logical";
		sql * Insert(fault);
		fault.LABEL = "Doesn't boot";
		sql * Insert(fault);
		fault.LABEL = "Sound";
		sql * Insert(fault);
	}
}

void AGAR::Exit() {

    if(PromptOKCancel(t_("Exit AGAR ?")))
    Break();
}

void AGAR::MainMenu(Bar& bar) {
	bar.Add(t_("File"), THISBACK(SubMenuFile));
	bar.Add(t_("PCB"), THISBACK(SubMenuPcb));
	bar.Add(t_("Options"), THISBACK(SubMenuOptions));
}

void AGAR::SubMenuFile(Bar& bar) {
    bar.Add(t_("Makers"), THISBACK(MakerList));
    bar.Add(t_("Games"), THISBACK(GameList));
    bar.Add(t_("Origins"), THISBACK(OriginList));
    bar.Separator();
    bar.Add(t_("Exit"), THISBACK(Exit));
}

void AGAR::SubMenuPcb(Bar& bar) {
    bar.Add(t_("PCB list"), THISBACK(PcbList));
}

void AGAR::SubMenuOptions(Bar& bar) {
	bar.Add(t_("Default values"), THISBACK(SubMenuOptionsDefaultvalues));
}

void AGAR::SubMenuOptionsDefaultvalues(Bar& bar) {
    bar.Add(t_("PCB Faults"), THISBACK(PcbFault));
    bar.Add(t_("PCB States"), THISBACK(PcbState));
    bar.Add(t_("PCB Types"), THISBACK(PcbType));
}

void AGAR::MakerList() {

	MakerDlg dlg;
	
	dlg.Run();
}

void AGAR::GameList() {
	
	// Displays game table records
	
	GameDlg dlg;
		
	dlg.Run();
}

void AGAR::OriginList() {
	
	// Displays origin table records
	
	OriginsDlg dlg;
		
	dlg.Run();
}

void AGAR::PcbList() {
	
	// Displays pcbs records
	
	PcbsDlg dlg;
		
	dlg.Run();
}

void AGAR::PcbFault() {
	
	// Displays PCB faults
	PcbFaultDlg dlg;
	
	dlg.Run();
}

void AGAR::PcbState() {
	
	// Displays PCB states
	PcbStateDlg dlg;
	
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
		Cout() << t_("Can't create or open database file\n");
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
