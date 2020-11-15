#ifndef _AGAR_AGAR_h
#define _AGAR_AGAR_h

#include <CtrlLib/CtrlLib.h>
#include <plugin/sqlite3/Sqlite3.h>
#include <SqlCtrl/SqlCtrl.h>

using namespace Upp;

#define SCHEMADIALECT <plugin/sqlite3/Sqlite3Schema.h>
#define MODEL <agar/agar.sch>
#include "Sql/sch_header.h"

#define LAYOUTFILE <agar/agar.lay>
#include <CtrlCore/lay.h>

#define IMAGECLASS MyImages
#define IMAGEFILE  "agar/img/images.iml"
#include <Draw/iml_header.h>

class MakerDlg;
class GameDlg;
class OriginsDlg;
class LocationsDlg;
class PinoutsDlg;

enum OpeningType {
	OPENING_NEW,
	OPENING_EDIT
};

class AGAR : public WithAGARLayout<TopWindow> {

	typedef AGAR CLASSNAME;

public:

	void DatabaseInit();

	// menu handling functions
	void MainMenu( Bar& bar);

	void SubMenuFile(Bar& bar);
	void SubMenuPcb(Bar& bar);
	void SubMenuOptions(Bar& bar);
	void SubMenuOptionsDefaultvalues(Bar& bar);

	// menu dispatch functions
	void MakerList();
	void GameList();
	void LocationList();
	void OriginList();
	void PinoutList();
	void Exit();
	void PcbList();
	void openPcbFaultDialog();
	void openPcbStateDialog();
	void openPcbTypeDialog();
	void ResetInitialFault();
	void About();
	void openSettingsWindow();
	void initializeConfigurationFile();
	
	String GetVersion();
	
	bool Key(dword key,int);

	AGAR();
	~AGAR();
	
private:
	// Main menu
	MenuBar MB_menu_;
	
	String version_; // software version
	
	// non modal windows
	std::unique_ptr<MakerDlg>       maker_dialog_;
	std::unique_ptr<GameDlg>        game_dialog_;
	std::unique_ptr<OriginsDlg>		origins_dialog_;
	std::unique_ptr<LocationsDlg>	locations_dialog_;
	std::unique_ptr<PinoutsDlg>		pinouts_dialog_;
};

void saveConfiguration(const String& filename, const VectorMap<String, String>& data);
void addConfigurationValue(VectorMap<String, String>& data, const String& key, const String& value);

#endif
