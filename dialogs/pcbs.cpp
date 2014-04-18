#include "pcbs.h"
#include "pcb.h"

PcbsDlg::PcbsDlg() {

	CtrlLayout(*this, t_("Pcbs list"));
	BTN_Close <<= Breaker(999);
	BTN_Extract.WhenPush = THISBACK(ExtractListing);
	TAB_pcbs.WhenBar = THISBACK(OwnMenu); // own menu
	
	SqlId GAME_RAW("GAME_RAW");
	TAB_pcbs.AddIndex(ID);
	TAB_pcbs.AddColumn(GAME,t_("Game")).HeaderTab().WhenAction = THISBACK1(SortTable, 0);
	TAB_pcbs.AddColumn(PCB_TYPE,t_("Type")).HeaderTab().WhenAction = THISBACK1(SortTable, 1);
	TAB_pcbs.AddColumn(TAG,t_("Tag")).HeaderTab().WhenAction = THISBACK1(SortTable, 2);
	TAB_pcbs.AddColumn(LOCATION,t_("Location")).HeaderTab().WhenAction = THISBACK1(SortTable, 3);
	TAB_pcbs.AddColumn(PINOUT,t_("Pinout")).HeaderTab().WhenAction = THISBACK1(SortTable, 4);
	TAB_pcbs.AddColumn(ORIGIN,t_("Origin")).HeaderTab().WhenAction = THISBACK1(SortTable, 5);
	TAB_pcbs.AddIndex(LABEL);
	TAB_pcbs.AddIndex(GAME_RAW);
	TAB_pcbs.WhenLeftDouble = THISBACK1(Edit,0);
	TAB_pcbs.ColumnWidths("346 82 81 82 82 82");
	
	// Filters
	// state droplist
	filterState_ = 0; // state selected is reset
	DL_State.Add(0, t_("All")); // First line of the dropbox

	Sql sql;
	sql * Select(ID,LABEL,PAPER,INK).From(PCB_STATE);
	while(sql.Fetch()) {
		DL_State.Add(
		    sql[ID],
			AttrText(sql[LABEL].ToString())
				.Paper(Color::FromRaw(static_cast<dword>(sql[PAPER].To<int64>())))
				.Ink(Color::FromRaw(static_cast<dword>(sql[INK].To<int64>())))
		);
	}		
	DL_State.WhenAction = THISBACK(ExecuteFilter);
	DL_State.SetIndex(0); // first line is selected
	
	LoadFaultData();
	for (int i=0; i<option_.GetCount(); i++) {
		option_[i].WhenAction = THISBACK(ExecuteFilter);
	}
	
	// Table is filled and sorted
	/*isSortedAsc_ = true;
	ReloadTable(isSortedAsc_);
	sortedColumnIndex_ = 0;
	sortedColumnDirection_ = SORT_UP;*/
	
	// First switch option is selected
	S_ExtractType = 0; // Text
	
	// Getting column widths from the ini file
	VectorMap<String, String> cfg = LoadIniFile("agar.cfg");
	String columnWidths = cfg.Get("PcbsListColumnWidths", Null);
	if (!IsNull(columnWidths)) TAB_pcbs.ColumnWidths(columnWidths);
	
	int l = ScanInt(cfg.Get("PcbsListWindowPosLeft", Null));
	if (!IsNull(l)) {
		int t = ScanInt(cfg.Get("PcbsListWindowPosTop", Null));
		int r = ScanInt(cfg.Get("PcbsListWindowPosRight", Null));
		int b = ScanInt(cfg.Get("PcbsListWindowPosBottom", Null));
		
		Rect rect(l, t, r, b);
		this->SetRect(rect);
	}
	
	/*for(int i = 0; i < TAB_pcbs.HeaderObject().GetCount(); i++)
		TAB_pcbs.HeaderObject().Tab(i).SetRightImage(TAB_pcbs.HeaderObject().GetTabIndex(i) == sortcolumn ?
		                                 sortcolumndescending ? CtrlImg::SortUp()
		                                                      : CtrlImg::SortDown()
		                            : Image());*/

	sortedColumnIndex_ = ScanInt(cfg.Get("PcbsListSortColumnIndex", Null));
	if (IsNull(sortedColumnIndex_)) {
		// value isn't available in the config file, default values are set
		sortedColumnIndex_ = 0;
		sortedColumnDirection_ = SORT_UP;
		isSortedAsc_ = false;
		ReloadTable(isSortedAsc_);		
	} else {
		// getting sort direction
		sortedColumnDirection_ = ScanInt(cfg.Get("PcbsListSortColumnDirection", Null));
		if (IsNull(sortedColumnDirection_)) sortedColumnDirection_ = SORT_NONE;
		
		if (sortedColumnIndex_ == 0) {
			// first column is sorted through sql
			if ((sortedColumnDirection_ == SORT_UP) || (sortedColumnDirection_ == SORT_NONE)) {
				// arrow going up
				isSortedAsc_ = true;
				TAB_pcbs.HeaderObject().Tab(sortedColumnIndex_).SetRightImage(CtrlImg::SortUp());
			} else {
				// arrow going down
				isSortedAsc_ = false;
				TAB_pcbs.HeaderObject().Tab(sortedColumnIndex_).SetRightImage(CtrlImg::SortDown());
			}
			ReloadTable(isSortedAsc_);
				
		} else {
			ReloadTable(isSortedAsc_);
			if ((sortedColumnDirection_ == SORT_UP) || (sortedColumnDirection_ == SORT_NONE)) {
				// arrow going up
				TAB_pcbs.SetSortColumn(sortedColumnIndex_,false);
				TAB_pcbs.HeaderObject().Tab(sortedColumnIndex_).SetRightImage(CtrlImg::SortUp());
			} else {
				// arrow going down
				TAB_pcbs.SetSortColumn(sortedColumnIndex_,true);
				TAB_pcbs.HeaderObject().Tab(sortedColumnIndex_).SetRightImage(CtrlImg::SortDown());
			}
		}
	}		

}

PcbsDlg::~PcbsDlg() {
	// Saving column data to the ini file
	String cfg;
	Rect r = this->GetRect();
//	String windowSize = Format("%d %d %d %d", r.left, r.top, r.right, r.bottom);
	cfg << "PcbsListColumnWidths=" << TAB_pcbs.GetColumnWidths() << "\n"
		"PcbsListWindowPosLeft=" << r.left << "\n"
		"PcbsListWindowPosTop=" << r.top << "\n"
		"PcbsListWindowPosRight=" << r.right << "\n"
		"PcbsListWindowPosBottom=" << r.bottom << "\n"
		"PcbsListSortColumnIndex=" << sortedColumnIndex_ << "\n"
		"PcbsListSortColumnDirection=" << sortedColumnDirection_ << "\n";
	
	if(!SaveFile("agar.cfg", cfg))
	    Exclamation("Error saving configuration!");
}

void PcbsDlg::OwnMenu(Bar& bar) {
	bar.Add(t_("Create"),THISBACK(Create));
	bar.Add(t_("Edit"),THISBACK1(Edit,0));
	bar.Add(t_("Remove"),THISBACK(Remove));
	bar.Separator();
	bar.Add(t_("Generate report"),THISBACK(GenerateReport));
}

void PcbsDlg::Create() {
	PcbDlg dlg(OPENING_NEW);
	
	if(dlg.Execute() != IDOK)
		return;
	dlg.GenerateFaultData();
	
	dlg.ES_FaultsOrigin.SetData(~dlg.ES_Faults);
		
	SQL * dlg.ctrls.Insert(PCB); // record is inserted in the database
	int id = SQL.GetInsertedId();
	// Do we want an initial analysis to be created ?
	if (PromptYesNo(t_("Do you want to create an initial analysis ?"))) {
		// Creation of the initial analysis
		// TODO
		dlg.AddAnalysis(id);
		Edit(id);
	}
	
	ReloadTable(true);
	TAB_pcbs.FindSetCursor(id);
}

void PcbsDlg::Edit(int pcbId) {
	int id = pcbId;
	if (!id) id = TAB_pcbs.GetKey();
	if(IsNull(id))
		return;
	PcbDlg dlg(OPENING_EDIT, id);

	if(!dlg.ctrls.Load(PCB, ID == id))
		return;
	
	//dlg.BuildActionTree(id);
	if (!dlg.GetRecordNumber(id)) {
		// no record for this pcb
		dlg.TC_AnalysisAction.NoRoot(true); // root of treecontrol is hidden as there's nothing to display
		dlg.SetAddActionMenuEntryVisible(false);
		dlg.SetEditMenuEntryVisible(false);
	}
	//dlg.SizePos();
	if(dlg.Execute() != IDOK)
		return;
	dlg.GenerateFaultData();
	
	SQL * dlg.ctrls.Update(PCB).Where(ID == id);

	int index = TAB_pcbs.GetCursor();
	ReloadTable(true);
	
	// edited pcb is selected in the table
	TAB_pcbs.SetCursor(index);
	TAB_pcbs.CenterCursor();
}

void PcbsDlg::Remove() {
	//TAB_pcbs.DoRemove();
	int id = TAB_pcbs.GetKey();
	if(IsNull(id) || !PromptYesNo(t_("Delete PCB ?")))
	   return;
	SQL * SqlDelete(PCB_ACTION).Where(PCB_ID == id);
	SQL * SqlDelete(PICTURE).Where(PCB_ID == id);
	SQL * SqlDelete(FLUKE).Where(PCB_ID == id);
	SQL * SqlDelete(PCB).Where(ID == id);

	ReloadTable(true);
}

void PcbsDlg::GenerateReport() {
	// Generates a text report extracted from the pcb data
	String report;
	String filename;
	String nl = "\r\n";
	
	int id = TAB_pcbs.GetKey(); //getting the pcb id
	
	String statement = "select MAKER.MAKER_NAME, GAME.GAME_NAME, PCB.PCB_ORIGIN_FAULT_OPTION ";
	statement += "from PCB, GAME, MAKER ";
	statement += "where PCB.GAME_ID = GAME.ID ";
	statement += "and GAME.MAKER_ID = MAKER.ID ";
	statement += Format("and PCB.ID = %i",id);
	
	Sql sql;
	sql.Execute(statement);
	if (sql.Fetch()) {
		report = AsString(sql[0]) + " " + AsString(sql[1]);
		filename = report + ".txt";
		
	}
	report += nl;
	
	// Original faults listing
	report += "Original fault(s): ";
	String faults = AsString(sql[2]);
	sql.Execute("select ID,LABEL from PCB_FAULT order by LABEL");
	while (sql.Fetch()) {
		int id = StdConvertInt().Scan(sql[ID].ToString());
		// faults at 1 will be added to the report
		if (PcbDlg::GetFaultValue(id, faults)) {
			report += AsString(sql[LABEL]) + " ";
		}
	}
	report += nl;
	
	// Analysis & actions listing
	report += "Analysis `& actions :";
	report += nl;
	sql.Execute(Format("select ID,PARENT_ID,COMMENTARY,FINISHED from PCB_ACTION where PCB_ID = %i order by ACTION_DATE",id));
	while (sql.Fetch()) {
		if (sql[PARENT_ID] == 0) report += "- Analysis : ";
		else report += "     - Action : ";
		
		report += sql[COMMENTARY].ToString();
		report += nl;
	}
	

	//PromptOK(report);
	if (SaveFile(filename, report)) PromptOK("'" + filename + "' was generated.");
	else PromptOK("File '" + filename + "' couldn't be generated !");
}

void PcbsDlg::SortTable(const int& i) {
	// checking if the clicked column is already sorted
	if (i == sortedColumnIndex_) {
		// column was previously sorted, checking sorting direction
		switch (sortedColumnDirection_) {
			case SORT_UP:
				// previous sorting was up
				sortedColumnDirection_ = SORT_DOWN;
				TAB_pcbs.SetSortColumn(sortedColumnIndex_,true);
				TAB_pcbs.HeaderObject().Tab(sortedColumnIndex_).SetRightImage(CtrlImg::SortDown());
				break;
			case SORT_DOWN:
				// previous sorting was down
				sortedColumnDirection_ = SORT_UP;
				TAB_pcbs.SetSortColumn(sortedColumnIndex_,false);
				TAB_pcbs.HeaderObject().Tab(sortedColumnIndex_).SetRightImage(CtrlImg::SortUp());
				break;	
		}
	
	} else {
		// column wasn't sorted previously, sorting up
		sortedColumnDirection_ = SORT_UP;
		TAB_pcbs.SetSortColumn(i, false);
		TAB_pcbs.HeaderObject().Tab(i).SetRightImage(CtrlImg::SortUp());
		 
		// reseting arrow for the previous column
		TAB_pcbs.HeaderObject().Tab(sortedColumnIndex_).SetRightImage(Image());
	}
	
	sortedColumnIndex_ = i;

	if (!i) {
		// Sorting is manually done through table reload for the first column as it's not possible to sort directly
		// AttrText data from a column directly.
		if ((sortedColumnDirection_ == SORT_UP) || (sortedColumnDirection_ == SORT_NONE)) {
			// arrow going up
			isSortedAsc_ = true;
			TAB_pcbs.HeaderObject().Tab(sortedColumnIndex_).SetRightImage(CtrlImg::SortUp());
		} else {
			// arrow going down
			isSortedAsc_ = false;
			TAB_pcbs.HeaderObject().Tab(sortedColumnIndex_).SetRightImage(CtrlImg::SortDown());
		}
		ReloadTable(isSortedAsc_);

	}
}

void PcbsDlg::ReloadTable(const bool& ascSort) {
	// reloads table content from the database
	TAB_pcbs.Clear();
	
	Sql sql;
	String statement = "select PCB.ID, MAKER.MAKER_NAME, GAME.GAME_NAME, PCB_STATE.INK, PCB_STATE.PAPER, PCB_TYPE.LABEL, TAG, LOCATION.LABEL, ";
	statement += "PINOUT.LABEL, ORIGIN.ORIGIN, PCB_STATE.LABEL ";
	statement += "from PCB,	GAME, MAKER, PCB_TYPE, PCB_STATE ";
	statement += "left outer join LOCATION on pcb.location_id = location.id ";
	statement += "left outer join PINOUT on pcb.pinout_id = pinout.id ";
	statement += "left outer join ORIGIN on pcb.origin_id = origin.id ";
	statement += "where pcb.game_id = game.id ";
	statement += "and game.maker_id = maker.id ";
	statement += "and pcb.pcb_type_id = pcb_type.id ";
	statement += "and pcb.pcb_state_id = pcb_state.id ";
	// State filter
	if (filterState_) statement += Format("and pcb.pcb_state_id = %1 ",filterState_);
	// Faults filter
	Value key;
	String str = "";
	for (int i=0; i<option_.GetCount(); i++) {
		if (option_[i]){
			// option is checked, statement is modified
			key = option_.GetKey(i);
			str = "\"%";
			str += key.ToString();
			str += ":1%\"";

			statement += Format(" and pcb.pcb_fault_option like % ",str);
		}
	}

	
	if (ascSort) statement += " order by MAKER_NAME asc,GAME_NAME asc";
	else statement += " order by MAKER_NAME desc,GAME_NAME desc";
	sql.Execute(statement);
	while (sql.Fetch()) {
		String game = sql[1].ToString()+" - "+sql[2].ToString();
		Color ink = Color::FromRaw(static_cast<dword>(sql[3].To<int64>()));
		Color paper = Color::FromRaw(static_cast<dword>(sql[4].To<int64>()));
		
		AttrText atGame = AttrText(game).Ink(ink).Paper(paper);
		TAB_pcbs.Add(
			sql[0], // pcb id.
			atGame, // maker + game formatted
			sql[5], // type
			sql[6], // tag
			sql[7], // location
			sql[8], // pinout
			sql[9], // origin
			sql[10],// state
			game
		);
	}	
}

void PcbsDlg::ExecuteFilter() {
	// State filter
	filterState_ = ~DL_State;
	
	// Faults filter
	
	
	// Table is reloaded
	ReloadTable(isSortedAsc_);
}

void PcbsDlg::LoadFaultData() {
	// fault option list
	Sql sql;
	Rect r = L_Faults.GetRect();
	//int y = r.top + 20;
	int y = 116;
	int linecy = Draw::GetStdFontCy() + 2;
	int current = 0; 
	sql * Select(ID,LABEL).From(PCB_FAULT).OrderBy(LABEL);
	while(sql.Fetch()) {
		
		Add(option_.Add(sql[ID]).SetLabel(sql[LABEL].ToString()).TopPosZ(y, linecy).RightPosZ(10,200));
		int id = StdConvertInt().Scan(sql[ID].ToString());
		y += linecy;
		current++;
	}	
	
	// Labels position override
	L_Faults.RightPosZ(80, 136).TopPosZ(96, y - 96);
	L_Filters.RightPosZ(8, 228).TopPosZ(44, y - 44 +2);
}

void PcbsDlg::ExtractListing() {
	String listing = "";
	String nl = "\r\n";
	//String nl = "&";
	JsonArray ja;
	
	
	for (int i=0; i<TAB_pcbs.GetCount(); i++) {
		// data filling
		Data x;
		x.game 		= TAB_pcbs.Get(i,8).ToString().ToString(); // game name
		x.type 		= TAB_pcbs.Get(i,2).ToString(); // type
		x.tag 		= TAB_pcbs.Get(i,3).ToString(); // tag
		x.location 	= TAB_pcbs.Get(i,4).ToString(); // location
		x.state		= TAB_pcbs.Get(i,7).ToString(); // state
		
		switch(S_ExtractType) {
		case 0 : // text
			// text export
			listing += x.game+"|"+ x.type+"|"+ x.tag+"|"+ x.location+"|"+ x.state+ nl;
			break;	
		case 1:
		case 2:  // JSON & XML
			ja << StoreAsJsonValue(x);
			break;
		}
					
		// JSON export
		/*ja << Json("game", TAB_pcbs.Get(i,8).ToString().ToString()) 
				("type", TAB_pcbs.Get(i,2).ToString())
				("tag", TAB_pcbs.Get(i,3).ToString())
				("location", TAB_pcbs.Get(i,4).ToString())
				("state", TAB_pcbs.Get(i,7).ToString());*/
			
	}

	String filename = "";
	switch(S_ExtractType) {
	case 0 : // text
		filename = "listing.txt";
		SaveFile(filename, listing);
		break;	
	case 1: // JSON
		filename = "listing.json";
		StoreAsJsonFile(ParseJSON(ja.ToString()), filename, false);
		break;
	case 2:  // XML
		filename = "listing.xml";
		Value xml = ParseJSON(ja.ToString());
		StoreAsXMLFile(xml, NULL, filename);
		break;
	}
	
	PromptOK("Data extraction done to '"+filename+"'");
}

void Data::Jsonize(JsonIO& json) {

    json
        ("game", game)
        ("type", type)
        ("tag", tag)
        ("location", location)
        ("state", state)
    ;

}

void PcbsDlg::TableSort(const int& columnIndex, const int& direction) {


}