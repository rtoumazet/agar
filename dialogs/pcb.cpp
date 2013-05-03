#include "pcb.h"

#include "../utilities/converts.h"
#include "../utilities/lookups.h"

PcbDlg::PcbDlg() {

	CtrlLayoutOKCancel(*this, t_("Pcb"));

	// Filling droplists data
	Sql sql;
	// Game droplist
	sql.Execute("select GAME.ID, MAKER_NAME, GAME_NAME from MAKER,GAME where GAME.MAKER_ID = MAKER.ID order by MAKER_NAME,GAME_NAME");
	while(sql.Fetch()) {
		String temp = sql[1].ToString() + ' - ' + sql[2].ToString();
		DL_Game.Add(
			sql[0],
			temp
		);
	}
	
	// type droplist
	sql.Execute("select ID,LABEL from PCB_TYPE");
	while(sql.Fetch()) {
		DL_Type.Add(
			sql[0],
			sql[1].ToString()
		);
	}	
	
	// state droplist
	sql.Execute("select ID,LABEL,PAPER,INK from PCB_STATE");
	while(sql.Fetch()) {
		DL_State.Add(
		    sql[0],
			AttrText(sql[1].ToString())
				.Paper(Color::FromRaw(static_cast<dword>(sql[2].To<int64>())))
				.Ink(Color::FromRaw(static_cast<dword>(sql[3].To<int64>())))
		);
	}	

	// location droplist
	sql.Execute("select ID,LABEL from LOCATION");
	while(sql.Fetch()) {
		DL_Location.Add(
			sql[0],
			sql[1].ToString()
		);
	}	

	// origin droplist
	sql.Execute("select ID,ORIGIN from ORIGIN");
	while(sql.Fetch()) {
		DL_Origin.Add(
			sql[0],
			sql[1].ToString()
		);
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

