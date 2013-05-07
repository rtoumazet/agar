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
	

	ctrls // manual declaration
		(PCB_STATE_ID, DL_State)
		(ORIGIN_ID, DL_Origin)
		(LOCATION_ID, DL_Location)
		(PCB_TYPE_ID, DL_Type)
		(GAME_ID, DL_Game)
		(LAST_TEST_DATE, D_LastTestDate)
		(TAG, ES_Tag)
		(PCB_FAULT_OPTION, ES_Faults)
	;
}

void PcbDlg::GenerateFaultData() {
	// builds the string containing checked values
	String faults = "";
	Value key, data;
	for (int i=0; i<option.GetCount(); i++) {
		key = option.GetKey(i);
		data = option[i].GetData();
		faults += key.ToString() +":";
		faults += data.ToString() +";";
	}
	ES_Faults = faults;
	//PromptOK(dlg.ES_Faults.GetData().ToString());	
}

bool PcbDlg::GetFaultValue(const int& id) {
	// returns the option state for the fault id parameter


	Sql sql;
	int startPos = 0;
	int endPos = 0;
	bool ret = false;
	String str = ES_Faults;
	String subStr = "";
	endPos = str.Find(";",startPos);
	while (endPos != -1) {
		subStr = str.Mid(startPos, endPos - startPos);
		int end = subStr.Find(":");
		//Value val = subStr.Mid(0,end);
		int val = StdConvertInt().Scan(subStr.Mid(0,end));
		if (val == id) {
			// id was found, getting the option value
			//val = subStr.Mid(end+1);
			ret = StdConvertInt().Scan(subStr.Mid(end+1));
			//ret = true;
			break;	
		}

		// moving to the next chunk
		startPos = endPos + 1;
		endPos = str.Find(";",startPos);
	}	
	
	return ret;
}

void PcbDlg::LoadFaultData() {
	// fault option list
	Sql sql;
	int y = 100;
	int linecy = Draw::GetStdFontCy() + 4;
	int current = 0; 
	sql.Execute("select ID,LABEL from PCB_FAULT order by LABEL");
	while(sql.Fetch()) {
		Add(option.Add(sql[0]).SetLabel(sql[1].ToString()).TopPos(y, linecy).LeftPos(340, 100));
		int id = StdConvertInt().Scan(sql[0].ToString());
		option[current].SetData(GetFaultValue(id));
		y += linecy;
		current++;
	}	
}