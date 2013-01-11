#include "pcb.h"

#include "../utilities/converts.h"
#include "../utilities/lookups.h"

PcbDlg::PcbDlg() {

	CtrlLayoutOKCancel(*this, t_("Pcb"));

	// Filling droplists data
	Sql sql;
	// Game droplist
	sql.Execute("select MAKER_NAME, GAME_NAME from MAKER,GAME where GAME.MAKER_ID = MAKER.ID");
	while(sql.Fetch()) {
		String temp = sql[0].ToString() + ' - ' + sql[1].ToString();
		DL_Game.Add(temp);
	}
	
	// type droplist
	sql.Execute("select LABEL from PCB_TYPE");
	while(sql.Fetch()) {
		String temp = sql[0].ToString();
		DL_Type.Add(temp);
	}	
	
	// state droplist
	sql.Execute("select LABEL from PCB_STATE");
	while(sql.Fetch()) {
		DL_State.Add(sql[0],AttrText(sql[0].ToString()).Paper(Red()));
	}	

	// location droplist
	sql.Execute("select LABEL from LOCATION");
	while(sql.Fetch()) {
		String temp = sql[0].ToString();
		DL_Location.Add(temp);
	}	

	// origin droplist
	sql.Execute("select ORIGIN from ORIGIN");
	while(sql.Fetch()) {
		String temp = sql[0].ToString();
		DL_Origin.Add(temp);
	}	
	
	//ctrls(*this, PCB); //	matches widgets to columns based on Layout and schema introspection
	
	ctrls // manual declaration
		(PCB_STATE_ID, DL_State)
		(ORIGIN_ID, DL_Origin)
		(LOCATION_ID, DL_Location)
		(PCB_TYPE_ID, DL_Type)
		(GAME_ID, DL_Game)
		(LAST_TEST_DATE, D_LastTestDate)
		(TAG, ES_Tag)
	;
	
}

