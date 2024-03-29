#include "agar.h"

#include <Sql/sch_schema.h>
#include <Sql/sch_source.h>

#define IMAGECLASS MyImages
#define IMAGEFILE  "agar/img/images.iml"
#include <Draw/iml_source.h>

#include <vector>
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
#include "dialogs/settings.h"

constexpr auto default_image_width = int{1024};
constexpr auto default_image_height = int{768};

enum class ParentIdUpdated{
	not_updated = 0,
	updated = 1
};

SqlId count("count(*)");

AGAR::AGAR()
{
	version_ = "AGAR 1.2.2";
	
	SetDateFormat("%3:02d/%2:02d/%1:4d"); // set dd/mm/yyyy format
	SetDateScan("dmy"); // same but for input
	
	CtrlLayout(*this, version_);
	
	AddFrame(MB_menu_);
    MB_menu_.Set(THISBACK(MainMenu));
    
    DatabaseInit();
    
	initializeConfigurationFile();
}

AGAR::~AGAR() {}

void AGAR::DatabaseInit()
{
	// Various database init
	Sql sql;
	
    // PCB_TYPE default values
	sql.Execute("select * from PCB_TYPE;");
	if (!sql.Fetch()) {
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
	if (!sql.Fetch()) {
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
	if (!sql.Fetch()) {
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
	
	updateParentId();
}

void AGAR::Exit()
{
    if (PromptOKCancel(t_("Exit AGAR ?"))) {
        Break();
    }
}

void AGAR::MainMenu(Bar& bar)
{
	bar.Add(t_("File"), THISBACK(SubMenuFile));
	bar.Add(t_("PCB"), THISBACK(SubMenuPcb));
	bar.Add(t_("Options"), THISBACK(SubMenuOptions));
	bar.Add(t_("?"), THISBACK(openAboutDialog));
}

void AGAR::SubMenuFile(Bar& bar)
{
    bar.Add(t_("Manufacturers"), THISBACK(openMakerList));
    bar.Add(t_("Games"), THISBACK(openGameList));
    bar.Add(t_("Locations"), THISBACK(openLocationList));
    bar.Add(t_("Origins"), THISBACK(openOriginList));
    bar.Add(t_("Pinouts"), THISBACK(openPinoutList));
    bar.Separator();
    bar.Add(t_("Exit"), THISBACK(Exit));
}

void AGAR::SubMenuPcb(Bar& bar)
{
    bar.Add(t_("PCB list"), THISBACK(openPcbList));
}

void AGAR::SubMenuOptions(Bar& bar)
{
	bar.Add(t_("Settings"), THISBACK(openSettingsWindow));
	bar.Add(t_("Default values"), THISBACK(SubMenuOptionsDefaultvalues));
}

void AGAR::SubMenuOptionsDefaultvalues(Bar& bar) {
    bar.Add(t_("PCB Faults"), THISBACK(openPcbFaultDialog));
    bar.Add(t_("PCB States"), THISBACK(openPcbStateDialog));
    bar.Add(t_("PCB Types"), THISBACK(openPcbTypeDialog));
}

void AGAR::openMakerList()
{
    if (!maker_dialog_) {
        maker_dialog_ = std::make_unique<MakerDlg>();
    }
	maker_dialog_->IsOpen() ? maker_dialog_->Close() : maker_dialog_->Open(this);
}

void AGAR::openGameList()
{
	if (!game_dialog_) {
        game_dialog_ = std::make_unique<GameDlg>();
    }
	game_dialog_->IsOpen() ? game_dialog_->Close() : game_dialog_->Open(this);
}

void AGAR::openOriginList()
{
	if (!origins_dialog_) {
        origins_dialog_ = std::make_unique<OriginsDlg>();
    }
	origins_dialog_->IsOpen() ? origins_dialog_->Close() : origins_dialog_->Open(this);
}

void AGAR::openLocationList()
{
	if (!locations_dialog_) {
        locations_dialog_ = std::make_unique<LocationsDlg>();
    }
	locations_dialog_->IsOpen() ? locations_dialog_->Close() : locations_dialog_->Open(this);
}

void AGAR::openPinoutList()
{
	if (!pinouts_dialog_) {
        pinouts_dialog_ = std::make_unique<PinoutsDlg>();
    }
	pinouts_dialog_->IsOpen() ? pinouts_dialog_->Close() : pinouts_dialog_->Open(this);
}

void AGAR::openPcbList()
{
	PcbsDlg().Sizeable().Run();
}

void AGAR::openPcbFaultDialog()
{
	PcbFaultDlg().Run();
}

void AGAR::openPcbStateDialog()
{
	PcbStateDlg().Run();
}

void AGAR::openPcbTypeDialog()
{
	PcbTypeDlg().Run();
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
	
	const auto path_config = ConfigFile("AGAR.db");
	const auto path_current= AppendFileName(GetCurrentDirectory(), "AGAR.db");
	if (path_config != path_current){
		if (GetFileLength(path_current) == 0) {
			if (!FileDelete(path_current)) {
				Exclamation("Could not delete empty AGAR.db");
			}
		}
		
		// On previous linux version the database was saved in the config directory.
		// If that's the case, the file is copied to the current directory.
		if (FileExists(path_config)){
			if (!FileExists(path_current)) {
				if (!FileCopy(path_config, path_current)) {
					auto msg = Format("Can't copy AGAR.db from \001%s\001 &to %s", path_config, path_current);
					msg += "&Reason could be you don't have the rights to copy the file.";
					msg += "&Please do it manually to get your database back at the right place.";
					Exclamation(msg);
				}
			}
		}
	}
	//String databaseFullPath = ConfigFile("AGAR.db");
	const auto database_full_path = AppendFileName(GetCurrentDirectory(), "AGAR.db");
	if (!sqlite3.Open(database_full_path)) {
		Cout() << t_("Can't create or open database file\n");
		return;
	}


	#ifdef _DEBUG
	sqlite3.SetTrace();
	#endif
	
	SQL = sqlite3;

	Sql sql;
	auto user_version = String{""};
	sql.Execute("PRAGMA user_version;");
	if (sql.Fetch()) {
		//PromptOK(sql[0].ToString());
		user_version = sql[0].ToString();
	}

	// Following PRAGMA tells the database to use memory for temp files. (used by VACUUM)
	sql.Execute("PRAGMA temp_store = 2;");

	// strings initializations
	auto database_directory = GetFileDirectory(database_full_path);

	auto backup_directory = NativePath(database_directory);
	backup_directory += "backup";
	RealizeDirectory(backup_directory);

	auto schema_directory = NativePath(database_directory);
	schema_directory += "schema";
	RealizeDirectory(schema_directory);

	SqlSchema sch(SQLITE3);
	All_Tables(sch);
    if (sch.ScriptChanged(SqlSchema::UPGRADE, schema_directory)) {
	   // schema was updated, database is saved
	   auto backup_filename = String{"AGAR.db.bck"};
	   backup_filename += user_version;
	   FileCopy(database_full_path, AppendFileName(backup_directory, backup_filename));
	   SqlPerformScript(sch.Upgrade());
	   auto val = ScanInt(user_version);
	   ++val;
	   sql.Execute(Format("PRAGMA user_version = %i",val));
    }
    if (sch.ScriptChanged(SqlSchema::ATTRIBUTES, schema_directory)) {
        SqlPerformScript(sch.Attributes());
    }
	
    sch.SaveNormal(schema_directory);
    
	SQL.ClearError();
	
	AGAR agar;
	agar.Icon(MyImages::smallIcon());
	
	// Entering the main window modal loop
	agar.Run();
	
	SQL.Execute("VACUUM;"); // Deleted data is freed
}

auto AGAR::Key(dword key, int count) -> bool
{
	if (key == K_TAB) {
		return false;
	}
	return TopWindow::Key(key,count);
}

void AGAR::openAboutDialog()
{
	AboutDlg(GetVersion()).Run();
}

String AGAR::GetVersion()
{
	return version_;
}

void AGAR::openSettingsWindow()
{
	SettingsDlg().Run();
}

void AGAR::initializeConfigurationFile()
{
	auto cfg = LoadIniFile("agar.cfg");

	if (cfg.Find("ImageWidth") == -1)	cfg.Add("ImageWidth", IntStr(default_image_width));
	if (cfg.Find("ImageHeight") == -1)	cfg.Add("ImageHeight", IntStr(default_image_height));
	
	saveConfiguration("agar.cfg", cfg);
}

void AGAR::updateParentId()
{
    auto cfg     = LoadIniFile("agar.cfg");
	auto updated = static_cast<ParentIdUpdated>(StrInt(cfg.Get("ParentIdUpdated", "0")));
    
    if (updated == ParentIdUpdated::not_updated) {
        std::vector<ParentIdCorrespondance> pics;
        Sql sql;
        sql.Execute("select count(*) from pcb_action");
        if (sql.Fetch()) {
            pics.reserve(static_cast<int>(sql[0]));
            
            auto index = int{0};
            auto saved_pcb_id = int{0};
            sql.Execute("select ID, PCB_ID, PARENT_ID from pcb_action order by pcb_id, action_date;");
            while (sql.Fetch()) {
                const auto current_pcb_id = int{sql[1]};
                if(saved_pcb_id != current_pcb_id){
                    saved_pcb_id = current_pcb_id;
                    index = 1;
                }
                ParentIdCorrespondance pic = {sql[0], sql[1], sql[2], index};
                pics.emplace_back(pic);
                ++index;
            }
            
            for (auto& item: pics) {
                if(item.parent_id != 0){
                    const auto it = std::find_if(pics.begin(), pics.end(),[&item](const ParentIdCorrespondance& p) {
                                                                    return ((p.pcb_id == item.pcb_id) && (p.index == item.parent_id));}
                                            );
                    if (it != pics.end()){
                        item.parent_id = it->action_id;
                    }
                    sql.Execute("update pcb_action set PARENT_ID = ? where ID = ?", item.parent_id, item.action_id);
                }
            }
        }
        
        addConfigurationValue(cfg, "ParentIdUpdated", "1");
        saveConfiguration("agar.cfg", cfg);
    }
}

void saveConfiguration(const String& filename, const VectorMap<String, String>& data)
{
	String dataToSave;
	for(int i = 0; i < data.GetCount(); i++){
		dataToSave << data.GetKey(i) << "=" << data[i] << "\n";
	}
	
	if(!SaveFile("agar.cfg", dataToSave)) Exclamation("Error saving configuration!");
};

void addConfigurationValue(VectorMap<String, String>& data, const String& key, const String& value)
{
	const auto ndx = data.FindAdd(key, value);
	data[ndx] = value;
}
