#include "agar.h"

#include <Sql/sch_schema.h>
#include <Sql/sch_source.h>

#define IMAGECLASS MyImages
#define IMAGEFILE  "agar/images.iml"
#include <Draw/iml_source.h>

#include "dialogs/maker.h"
#include "dialogs/game.h"
#include "dialogs/locations.h"
#include "dialogs/origins.h"
#include "dialogs/pinouts.h"
#include "dialogs/pcbtype.h"
#include "dialogs/pcbstate.h"
#include "dialogs/pcbfault.h"
#include "dialogs/pcbs.h"
#include "dialogs/about.h"


SqlId count("count(*)");

AGAR::AGAR()
{
	version_ = "AGAR 1.1.8a";
	
	SetDateFormat("%3:02d/%2:02d/%1:4d"); // set dd/mm/yyyy format
	SetDateScan("dmy"); // same but for input
	
	CtrlLayout(*this, version_);
	
	AddFrame(MB_menu_);
    MB_menu_.Set(THISBACK(MainMenu));
    
    DatabaseInit();

	md_ = 0;
	gd_ = 0;
	ld_ = 0;
	od_ = 0;
	pd_ = 0;
    
}

AGAR::~AGAR()
{
	if (md_ != 0) delete md_;	
	if (gd_ != 0) delete gd_;	
	if (ld_ != 0) delete ld_;	
	if (od_ != 0) delete od_;	
	if (pd_ != 0) delete pd_;	
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
		state.PAPER = Green().GetRaw();
		state.INK	= White().GetRaw();
		sql * Insert(state);
		state.LABEL = "Working with things to be done";
		state.PAPER = LtGreen().GetRaw();
		state.INK	= Black().GetRaw();
		sql * Insert(state);
		state.LABEL = "Incomplete";
		state.PAPER = Magenta().GetRaw();
		state.INK	= White().GetRaw();
		sql * Insert(state);
		state.LABEL = "Work in progress";
		state.PAPER = Yellow().GetRaw();
		state.INK	= Black().GetRaw();
		sql * Insert(state);
		state.LABEL = "Difficulties";
		state.PAPER = Red().GetRaw();
		state.INK	= White().GetRaw();
		sql * Insert(state);
		state.LABEL = "Dead";
		state.PAPER = Black().GetRaw();
		state.INK	= White().GetRaw();
		sql * Insert(state);
		state.LABEL = "For parts";
		state.PAPER = LtMagenta().GetRaw();
		state.INK	= Black().GetRaw();
		sql * Insert(state);
		state.LABEL = "Untested";
		state.PAPER = LtGray().GetRaw();
		state.INK	= Black().GetRaw();
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

    if(PromptOKCancel(t_("Exit AGAR ?"))) {
    	Break();
    }
}

void AGAR::MainMenu(Bar& bar) {
	bar.Add(t_("File"), THISBACK(SubMenuFile));
	bar.Add(t_("PCB"), THISBACK(SubMenuPcb));
	bar.Add(t_("Options"), THISBACK(SubMenuOptions));
	bar.Add(t_("?"), THISBACK(About));
}

void AGAR::SubMenuFile(Bar& bar) {
    bar.Add(t_("Manufacturers"), THISBACK(MakerList));
    bar.Add(t_("Games"), THISBACK(GameList));
    bar.Add(t_("Locations"), THISBACK(LocationList));
    bar.Add(t_("Origins"), THISBACK(OriginList));
    bar.Add(t_("Pinouts"), THISBACK(PinoutList));
    bar.Separator();
    bar.Add(t_("Exit"), THISBACK(Exit));
}

void AGAR::SubMenuPcb(Bar& bar) {
    bar.Add(t_("PCB list"), THISBACK(PcbList));
}

void AGAR::SubMenuOptions(Bar& bar) {
	bar.Add(t_("Default values"), THISBACK(SubMenuOptionsDefaultvalues));
	bar.Add(t_("Reset initial faults"), THISBACK(ResetInitialFault));
}

void AGAR::SubMenuOptionsDefaultvalues(Bar& bar) {
    bar.Add(t_("PCB Faults"), THISBACK(PcbFault));
    bar.Add(t_("PCB States"), THISBACK(PcbState));
    bar.Add(t_("PCB Types"), THISBACK(PcbType));
}

void AGAR::MakerList() {

	if (md_ == 0) md_ = new MakerDlg();
	
	if (md_->IsOpen()) md_->Close();
	else md_->Open(this);
}

void AGAR::GameList() {
	
	// Displays game table records
	if (gd_ == 0) gd_ = new GameDlg();
	
	if (gd_->IsOpen()) gd_->Close();
	else gd_->Open(this);
	
}

void AGAR::OriginList() {
	
	// Displays origin table records
	if (od_ == 0) od_ = new OriginsDlg();
	
	if (od_->IsOpen()) od_->Close();
	else od_->Open(this);
	
}

void AGAR::LocationList() {
	
	// Displays location table records
	if (ld_ == 0) ld_ = new LocationsDlg();
	
	if (ld_->IsOpen()) ld_->Close();
	else ld_->Open(this);
	
}

void AGAR::PinoutList() {
	
	// Displays pinout table records
	if (pd_ == 0) pd_ = new PinoutsDlg();
	
	if (pd_->IsOpen()) pd_->Close();
	else pd_->Open(this);
	
}

void AGAR::PcbList() {
	
	// Displays pcbs records
	
	PcbsDlg dlg;
	dlg.Sizeable();
		
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

void AGAR::ResetInitialFault() {
	// For every PCB record, origin fault data is replaced by current fault data
	if (PromptYesNo(t_("Do you want to replace initial fault data for each pcb by its current fault value ?"))) {
		Sql sql;
		sql.Execute("update PCB set PCB_ORIGIN_FAULT_OPTION = PCB_FAULT_OPTION");
		PromptOK(t_("Done."));
	} else {
		PromptOK(t_("Operation cancelled"));	
	}
}

GUI_APP_MAIN
{
	// Connecting to the main database
	Sqlite3Session sqlite3;
	String databaseFullPath = ConfigFile("AGAR.db");
	if(!sqlite3.Open(databaseFullPath)) {
		Cout() << t_("Can't create or open database file\n");
		return;
	}


	#ifdef _DEBUG
	sqlite3.SetTrace();
	#endif
	
	SQL = sqlite3;

	Sql sql;
	String userVersion = "";
	sql.Execute("PRAGMA user_version;");
	if (sql.Fetch()) {
		//PromptOK(sql[0].ToString());
		userVersion = sql[0].ToString();
	}

	//sql.Execute("VACUUM");
	
	// strings initializations
	String databaseDirectory = GetFileDirectory(databaseFullPath);

	String backupDirectory = NativePath(databaseDirectory);
	backupDirectory += "backup";
	RealizeDirectory(backupDirectory);

	String schemaDirectory = NativePath(databaseDirectory);
	schemaDirectory += "schema";
	RealizeDirectory(schemaDirectory);	

	SqlSchema sch(SQLITE3);
	All_Tables(sch);
    if(sch.ScriptChanged(SqlSchema::UPGRADE, schemaDirectory)) {
	   // schema was updated, database is saved
	   String backupFilename = "backup/AGAR.db.bck";
	   backupFilename += userVersion;
	   FileCopy(databaseFullPath, ConfigFile(backupFilename));
	   SqlPerformScript(sch.Upgrade());
	   int val = ScanInt(userVersion);
	   val++;
	   sql.Execute(Format("PRAGMA user_version = %i",val));
    }
    if(sch.ScriptChanged(SqlSchema::ATTRIBUTES, schemaDirectory)) {
        SqlPerformScript(sch.Attributes());
    }
    sch.SaveNormal(schemaDirectory);
    
	SQL.ClearError();	
	
	//Icon(MyImages::smallIcon());
	AGAR().Icon(MyImages::smallIcon());
	
	// Entering the main window modal loop
	AGAR().Run();
	
	sql.Execute("VACUUM"); // Deleted data is freed
}

bool AGAR::Key(dword key, int count) {
	if (key == K_TAB) {
		return false;	
	}
	return TopWindow::Key(key,count);
}

void AGAR::About() {
	
	AboutDlg dlg(GetVersion());
	
	dlg.Run();	
}

String AGAR::GetVersion() {
	
	return version_;
}