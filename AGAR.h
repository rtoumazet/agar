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
#define IMAGEFILE  "agar/images.iml"
#include <Draw/iml_header.h>

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
	void PcbFault();
	void PcbState();
	void PcbType();
	void About();
	
	String GetVersion();
	
	bool Key(dword key,int);

	AGAR();
	
private:
	// Main menu
	MenuBar MB_menu_;
	
	String version_; // software version	
};

#endif
