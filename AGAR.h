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
	//WithModifyLayout<ParentCtrl> modify;

public:
	// Main menu
	MenuBar menu;
	void MainMenu( Bar& bar);
	void SubMenu(Bar& bar);
	void MakerList();
	void GameList();
	void Exit();

	typedef AGAR CLASSNAME;
	AGAR();
};

#endif
