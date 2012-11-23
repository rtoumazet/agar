#ifndef _AGAR_AGAR_h
#define _AGAR_AGAR_h

#include <CtrlLib/CtrlLib.h>
#include <plugin/sqlite3/Sqlite3.h>
#include <SqlCtrl/SqlCtrl.h>


using namespace Upp;

#define SCHEMADIALECT <plugin/sqlite3/Sqlite3Schema.h>
#define MODEL <AGAR/AGAR.sch>
#include "Sql/sch_header.h"

#define LAYOUTFILE <AGAR/AGAR.lay>
#include <CtrlCore/lay.h>

class AGAR : public WithAGARLayout<TopWindow> {

	typedef AGAR CLASSNAME;

public:

	void DatabaseInit();
	void MainMenu( Bar& bar);
	void SubMenuMain(Bar& bar);
	void MakerList();
	void RemoveMaker();
	
	void GameList();
	void Exit();

	void SubMenuOptions(Bar& bar);
	void PcbType();

	AGAR();
	
private:
	// Main menu
	MenuBar MB_menu_;
	
};

#endif
