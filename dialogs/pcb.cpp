// SYSTEM INCLUDES
#include <map>
#include <vector>

// PROJECT INCLUDES
#include "pcb.h"
#include "action.h"
#include "origin.h"
#include "pinout.h"
#include "location.h"
#include "pcbstate.h"
#include "game.h"

using namespace std;

PcbDlg::PcbDlg(const int openingType, const int pcbId) {

	CtrlLayout(*this, t_("Pcb"));
	
    // Setting up buttons callbacks
    ok <<= THISBACK(DoOk);
    cancel <<= Rejector(IDCANCEL);
    	
	currentCtrl_ = 0;
	pcbId_ = pcbId;
	
	editStyle_ = EditString::StyleDefault();
	editStyle_.text = SGray();

	// Tabs setup
	Size sz = TC_Tab.GetSize();
	
	// Analysis & action tab
	TC_Tab.Add(TC_AnalysisAction.LeftPos(0, sz.cx-10).TopPos(0, sz.cy-15), t_("Analysis & Action"));
	CtrlLayout(TabPictures);
	
	// Pictures tab
	TC_Tab.Add(TabPictures, t_("Pictures"));
	TabPictures.TAB_Pictures.AddIndex(ID);
	TabPictures.TAB_Pictures.AddColumn(LABEL,t_("Label"));
	TabPictures.Add(preview_.RightPosZ(0, 250).BottomPosZ(0, 200));
	TabPictures.TAB_Pictures.WhenBar = THISBACK(PictureTabMenu);

	TabPictures.BTN_Select.WhenPush = THISBACK(SelectImage);
	TabPictures.BTN_Add.WhenPush = THISBACK(AddImageToDatabase);
	TabPictures.TAB_Pictures.WhenLeftDouble = THISBACK(DisplayPicture);
	TabPictures.TAB_Pictures.WhenLeftClick = THISBACK(DisplayPicturePreview);
	
	// Signature tab
	CtrlLayout(TabSignature);
	TC_Tab.Add(TabSignature, t_("Signatures"));
	TabSignature.TAB_Signature.WhenBar = THISBACK(SignatureTabMenu);
	TabSignature.TAB_Signature.AddIndex(ID);
	TabSignature.TAB_Signature.AddColumn(ROM_NAME, t_("Rom name"));
	TabSignature.TAB_Signature.AddColumn(SECTION, t_("Section"));
	TabSignature.TAB_Signature.AddColumn(RANGE, t_("Range"));
	TabSignature.TAB_Signature.AddColumn(ORIGIN, t_("Origin"));
	TabSignature.TAB_Signature.AddColumn(CRC_32, t_("CRC32"));
	TabSignature.TAB_Signature.AddColumn(FLUKE_SIG, t_("Sig"));	
	TabSignature.TAB_Signature.ColumnWidths("137 137 108 108 60 30");
	
	TabSignature.BTN_Add.WhenPush = THISBACK(AddSignatureRecord);
		
	TabSignature.ES_SigRomName.MaxChars(20);
	TabSignature.ES_SigSection.MaxChars(20);
	TabSignature.ES_SigCrc32.MaxChars(8);
	TabSignature.ES_SigSig.MaxChars(4); 
	
	// Miscellaneous tab
	CtrlLayout(TabMisc);
	TC_Tab.Add(TabMisc, t_("Miscellaneous"));
	
	
	
	pictureWidth_ = 1024;
	pictureHeight_ = 768;

	Ctrl* child = NULL;
	switch (openingType) {
		case OPENING_NEW:
			Title(t_("New PCB"));
			TC_AnalysisAction.NoRoot(true); // root of treecontrol is hidden as there's no entry in creation
			TC_AnalysisAction.Disable(); // no action allowed on the TC during creation
			
			child = TabPictures.GetFirstChild();
			while (child) {
				SetupDisplay(child);
				child = child->GetNext();	
			}
			break;	
		case OPENING_EDIT:
			Title(t_("Edit PCB"));
			break;
	}		

	child = TabSignature.GetFirstChild();
	while (child) {
		SetupDisplay(child);
		child = child->GetNext();	
	}

	
	// Hiding controls not to be displayed
	E_PcbId.Hide();
	ES_Faults.Hide();
	ES_FaultsOrigin.Hide();
	O_ActionsFixed.Hide();
	
	// displaying images on controls
	BTN_NewGame.SetImage(MyImages::add);
	BTN_NewOrigin.SetImage(MyImages::add);
	BTN_NewLocation.SetImage(MyImages::add);
	BTN_NewPinout.SetImage(MyImages::add);
	TabSignature.BTN_Add.SetImage(MyImages::add);
	
	ActiveFocus(DL_Game); // sets the focus to the first droplist
	
	// Filling droplists data
	LoadDropList(TABLE_GAME);
	LoadDropList(TABLE_TYPE);
	LoadDropList(TABLE_STATE);
	LoadDropList(TABLE_LOCATION);
	LoadDropList(TABLE_ORIGIN);
	LoadDropList(TABLE_PINOUT);
	

	// Tree control
	TC_AnalysisAction.WhenBar = THISBACK(TreeControlMenu);
	TC_AnalysisAction.WhenDrag = THISBACK(TreeDrag);
	TC_AnalysisAction.WhenDropInsert = THISBACK(TreeDropInsert);
	TC_AnalysisAction.WhenLeftDouble = THISBACK(Edit);
    
    ActionRecordsKey(0); // action records key is initialized
    LoadActionTreeFromDatabase();	
	
	// buttons actions
	BTN_NewGame.WhenPush = THISBACK1(CreateLinkedRecord, TABLE_GAME);
	BTN_NewOrigin.WhenPush = THISBACK1(CreateLinkedRecord, TABLE_ORIGIN);
	BTN_NewLocation.WhenPush = THISBACK1(CreateLinkedRecord, TABLE_LOCATION);
	BTN_NewPinout.WhenPush = THISBACK1(CreateLinkedRecord, TABLE_PINOUT);
	
	// Tab action
	TC_Tab.WhenSet = THISBACK(TabChanged);
	
	ctrls // manual declaration
		(ID, E_PcbId)
		(PCB_STATE_ID, DL_State)
		(ORIGIN_ID, DL_Origin)
		(LOCATION_ID, DL_Location)
		(PINOUT_ID, DL_Pinout)
		(PCB_TYPE_ID, DL_Type)
		(GAME_ID, DL_Game)
		(LAST_TEST_DATE, D_LastTestDate)
		(TAG, ES_Tag)
		(PCB_FAULT_OPTION, ES_Faults)
		(PCB_ORIGIN_FAULT_OPTION, ES_FaultsOrigin)
		(PCB_VERSION, ES_Version)
		(ACTIONS_FIXED, O_ActionsFixed)
	;


    
    Size s = TabPictures.TAB_Pictures.GetStdSize();
	//auto p =array.GetPos();
    //TabPictures.Add(array.LeftPos(10, 200).TopPos(20, 200));
    TabPictures.Add(array.RightPos(10, 200).TopPos(20, 200));
    array.AddColumn("You can paste the text here too");
    array.MultiSelect();

    //array.WhenDrop = [=] (PasteClip& d) { DnD(d); };
    array.WhenDrop = [=] (PasteClip& d) { DnD(d, array); };
	
	//TabPictures.TAB_Pictures.WhenDrop = [=] (PasteClip& d) { DnD(d, TabPictures.TAB_Pictures); };
}

PcbDlg::~PcbDlg() {
	
}

void PcbDlg::GenerateFaultData() {
	// builds the string containing checked values
	String faults = "";
	Value key, data;
	// Current faults
	for (int i=0; i<option.GetCount(); i++) {
		key = option.GetKey(i);
		data = option[i].GetData();
		faults += key.ToString() +":";
		faults += data.ToString() +";";
	}
	ES_Faults = faults;
	
	// Original faults
	faults = "";
	for (int i=0; i<optionOrigin_.GetCount(); i++) {
		key = optionOrigin_.GetKey(i);
		data = optionOrigin_[i].GetData();
		faults += key.ToString() +":";
		faults += data.ToString() +";";
	}
	ES_FaultsOrigin = faults;
}

bool PcbDlg::GetFaultValue(const int id, const String& faults) {
	// returns the option state for the fault id parameter

	Sql sql;
	int startPos = 0;
	int endPos = 0;
	bool ret = false;
	String str = faults;
	String subStr = "";
	endPos = str.Find(";",startPos);
	while (endPos != -1) {
		subStr = str.Mid(startPos, endPos - startPos);
		int end = subStr.Find(":");
		int val = StdConvertInt().Scan(subStr.Mid(0,end));
		if (val == id) {
			// id was found, getting the option value
			ret = StdConvertInt().Scan(subStr.Mid(end+1));
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
	Rect r = L_Faults.GetRect();
	Rect rOrigin = TabMisc.L_Faults.GetRect();
	
	//int y = r.top + 20;
	int y = 80 + 20;
	int yOrigin = rOrigin.top + 20;
	int linecy = Draw::GetStdFontCy() + 2;
	int current = 0; 
	sql.Execute("select ID,LABEL from PCB_FAULT order by LABEL");
	while(sql.Fetch()) {
		Ctrl::Add(option.Add(sql[ID]).SetLabel(sql[LABEL].ToString()).TopPosZ(y, linecy).RightPosZ(10, 150));
		TabMisc.Add(optionOrigin_.Add(sql[ID]).SetLabel(sql[LABEL].ToString()).TopPos(yOrigin, linecy).LeftPos(rOrigin.left+10, 150));
		int id = StdConvertInt().Scan(sql[0].ToString());
		option[current].SetData(GetFaultValue(id, ~ES_Faults));
		optionOrigin_[current].SetData(GetFaultValue(id, ~ES_FaultsOrigin));
		y += linecy;
		yOrigin += linecy;
		current++;
	}	
	
	// Labels position override
	//L_Faults.RightPosZ(70, 136).TopPosZ(r.top, y - 100);
	L_Faults.TopPosZ(r.top, y - 80);
	TabMisc.L_Faults.TopPosZ(rOrigin.top, y - 100);
}

void PcbDlg::TreeControlMenu(Bar& bar) {
	// Setting options to display
	int id = TC_AnalysisAction.GetCursor();
	if (id != -1 && id != 0) {
		SetAddActionMenuEntryVisible(true);
		SetEditMenuEntryVisible(true);
		if (TC_AnalysisAction.GetChildCount(id) || TC_AnalysisAction.GetCursor() == 0) {
			// At least one child exists for the selected node, removal isn't allowed
			// Root removal isn't allowed either
			SetRemoveMenuEntryVisible(false);
		} else {
			SetRemoveMenuEntryVisible(true);
		}
	} else {
		// Nothing's selected, menu options are greyed
		SetAddActionMenuEntryVisible(false);
		SetEditMenuEntryVisible(false);
		SetRemoveMenuEntryVisible(false);
	}

	bar.Add(t_("Add analysis"),THISBACK2(AddRecord, ~E_PcbId, ActionDlg::ANALYSIS));
	bar.Add(DisplayAddActionMenuEntry(), t_("Link an action"), THISBACK2(AddRecord, ~E_PcbId, ActionDlg::ACTION));
	bar.Add(DisplayEditMenuEntry(), t_("Edit"), THISBACK(Edit));
	bar.Add(DisplayRemoveMenuEntry(), t_("Remove"), THISBACK(Remove));
}

void PcbDlg::Edit() {
	// Edits currently selected analysis / action in the treecontrol

	// Getting the key of the currently selected line
	int key = TC_AnalysisAction.Get();
	if(IsNull(key))
		return;
	
	// Getting the record corresponding to the selected line from the vector
	// As it's a reference, data in the vector will be modified
	ActionRecord& ar = GetActionFromVector(key);
	
	ActionDlg dlg(ar); // Creating edition dialog

	if(dlg.Execute() != IDOK) // Executing the dialog
		return;
	
	ar = dlg.Record();	// OK button was pressed, updating record
	
	BuildActionTree(); // Treecontrol is reloaded
};

void PcbDlg::Remove() {
	// Removes currectly selected analysis / action from the treecontrol
	
	//int id = TC_AnalysisAction.GetCursor();
	int key = TC_AnalysisAction.Get();
	if(IsNull(key) || !PromptYesNo(t_("Delete entry ?")))
	   return;
	
	// Removal from treecontrol
	int id = TC_AnalysisAction.Find(key);
	if (id != -1)
	{
		TC_AnalysisAction.Remove(id);

		// Removal from vector
		RemoveActionFromVector(key);

	} else {
		PromptOK(t_("Could not remove record"));
	}
	
    
    LogActionVector();
}

void PcbDlg::AddRecord(const int pcbId, const int type) {
	// adds a new record to the treecontrol
	
	ActionDlg *dlg = NULL;
	
	// correct contructor is picked regarding the type
	switch (type) {
		case ActionDlg::ANALYSIS:
		{
			dlg = new ActionDlg (pcbId, ActionDlg::ANALYSIS);
			break;
		}
		case ActionDlg::ACTION:
		{
			dlg = new ActionDlg (pcbId, ActionDlg::ACTION, TC_AnalysisAction.GetCursor());
			break;
		}
	}
	
	// opening the dialog
	if(dlg->Execute() != IDOK)
		return;
    
    // Adding the new record to the vector
    AddActionToVector(dlg->Record());	
	
	delete dlg;
	
	BuildActionTree(); // Treecontrol is rebuilt
}

int PcbDlg::GetRecordNumber(int const pcbId) {
	// returns number of record from action table for the pcb id in parameter
	
	int count = 0;
	
	Sql sql;
	sql.Execute(Format("select count(*) from PCB_ACTION where PCB_ID =%i",pcbId));
	if (sql.Fetch()) {
		count = sql[0];
	}
	
	return count;
}

bool PcbDlg::DisplayAddActionMenuEntry() {
	
	return addActionMenuEntryVisible_;
}

bool PcbDlg::DisplayEditMenuEntry() {
	
	return editMenuEntryVisible_;	
}

bool PcbDlg::DisplayRemoveMenuEntry() {
	
	return removeMenuEntryVisible_;	
}

void PcbDlg::SetAddActionMenuEntryVisible(const bool val) {
	addActionMenuEntryVisible_ = val;
}

void PcbDlg::SetEditMenuEntryVisible(const bool val) {
	editMenuEntryVisible_ = val;
}

void PcbDlg::SetRemoveMenuEntryVisible(const bool& val) {
	removeMenuEntryVisible_ = val;
}

void PcbDlg::BuildActionTree() {
	// Fills the tree control with data from action file

	TC_AnalysisAction.Clear();
		
	// vector is full, it's time to add records to the treecontrol.
	// if PARENT_ID is 0, record is an analysis
	// otherwise record is an action, and PARENT_ID refers to the index position in the treecontrol
	// Root of the tree control has index 0, first analysis has index 1
	vector<ActionRecord> actions; // will hold actions
	vector<ActionRecord> analysis; // will hold analysis
	vector<ActionRecord> orphans; // will hold orphans
	
	// looping through records to fill actions & analysis vectors
    for (vector<ActionRecord>::iterator it = actionRecords_.begin(); it != actionRecords_.end(); ++it)
    {
	    if( it->parentIndex == 0) analysis.push_back(*it);
	    else actions.push_back(*it);
    }
	
	TC_AnalysisAction.Clear(); // treecontrol is emptied
	TC_AnalysisAction.SetRoot(Null,0,t_("Analysis & Actions")); // setting up root
	TC_AnalysisAction.NoRoot(false);

	// Treecontrol is built using data from vector
	for (auto it = actionRecords_.begin(); it != actionRecords_.end(); ++it) {
		// Checking whether current iterator points to an analysis or to an action
		if (it->parentIndex == 0) {
			// It's an analysis, record can be added directly
			TC_AnalysisAction.Add(0, MyImages::analysis(), it->key, it->commentary, false);
		} 
		else {
			// It's an action, we must check if the parent exists before adding it
			// Parent id is checked against analysis vector content
			int id = it->parentIndex; // getting parent id of current record
	        vector<ActionRecord>::iterator ite = std::find_if(analysis.begin(), analysis.end(), [&id](const ActionRecord& ar)
	        {
	            return ar.nodeIndex == id;
	        });					
			
			// If parent doesn't exist, record is added to the orphans vector
	        if (ite == analysis.end()) {
	            // Parent wasn't found in the vector, so the record is added to the orphans
	            // vector
	            //TC_AnalysisAction.Add(0, MyImages::warning(), it->key, it->commentary, false);
	            orphans.push_back(*it);
	        }
	        else {
	            // Parent was found in the vector, record can be added to the treecontrol
	            TC_AnalysisAction.Add(it->parentIndex, 
	            	!it->finished ? MyImages::action() : MyImages::actionDone(), it->key, 
	            	it->commentary, false);
	        }

		}
	}
	
    // Handling orphan actions
	if (!orphans.empty()) 
	{
	    PromptOK(t_("Orphan actions exist for current pcb ! Use drag & drop to link them to an analysis"));

        // orphans actions are added to the treecontrol as analysis with a special icon
		for (vector<ActionRecord>::iterator it = orphans.begin(); it != orphans.end(); ++it) // actions loop
		{
			// adding action to the treecontrol
			TC_AnalysisAction.Add(0, MyImages::warning(), it->key, it->commentary, false);
		}
	}

	TC_AnalysisAction.OpenDeep(0); // All tree nodes are opened

}

void PcbDlg::CreateLinkedRecord(const int tableType) {
	int id = 0;
	switch (tableType) {
		case TABLE_PINOUT:
		{
			PinoutDlg dlg(OPENING_NEW);
			if(dlg.Execute() != IDOK)
				return;
			
			SQL * dlg.ctrls.Insert(PINOUT);
			id = SQL.GetInsertedId();
			
			// Droplist refresh
			LoadDropList(tableType);
			//DL_Pinout.SetIndex(id);
			DL_Pinout.SetIndex(DL_Pinout.FindKey(id));
			break;
		}
		case TABLE_ORIGIN:
		{
			OriginDlg dlg(OPENING_NEW);
			if(dlg.Execute() != IDOK)
				return;
			
			SQL * dlg.ctrls.Insert(ORIGIN);
			id = SQL.GetInsertedId();
			
			// Droplist refresh
			LoadDropList(tableType);
			DL_Origin.SetIndex(DL_Origin.FindKey(id));
			break;
		}
		case TABLE_LOCATION:
		{
			LocationDlg dlg(OPENING_NEW);
			if(dlg.Execute() != IDOK)
				return;
			
			SQL * dlg.ctrls.Insert(LOCATION);
			id = SQL.GetInsertedId();
			
			// Droplist refresh
			LoadDropList(tableType);
			DL_Location.SetIndex(DL_Location.FindKey(id));
			break;
		}

		case TABLE_GAME:
		{
			int idBeforeInsert = SQL.GetInsertedId();
			GameDlg dlg;

			dlg.Run();

			
			//SQL * dlg.ctrls.Insert(GAME);
			id = SQL.GetInsertedId();
			
			// Droplist refresh
			if (idBeforeInsert != id) {
				LoadDropList(tableType);
				DL_Game.SetIndex(DL_Game.FindKey(id));
			}
			break;
		}

/*		case TABLE_STATE:
		{
			PcbStateDlg dlg(OPENING_NEW);
			if(dlg.Execute() != IDOK)
				return;
			
			SQL * dlg.ctrls.Insert(PCB_STATE);
			id = SQL.GetInsertedId();
			
			// Droplist refresh
			LoadDropList(tableType);
			DL_State.SetIndex(id);
			break;
		}*/
	}
}

void PcbDlg::LoadDropList(const int tableType) {
	Sql sql;
	
	switch (tableType) {
		case TABLE_PINOUT:
			DL_Pinout.Clear();
			DL_Pinout.Add(0,t_("Not selected"));
			sql.Execute("select ID,LABEL,PIN_SIZE from PINOUT order by LABEL");
			while(sql.Fetch()) {
				String temp = Format("%s (%s)",sql[LABEL].ToString(),sql[PIN_SIZE].ToString());
				DL_Pinout.Add(
					sql[ID],
					temp
				);
			}
			DL_Pinout.SetIndex(0);
			DL_Pinout.NotNull(true);
			break;
		
		case TABLE_ORIGIN:
			DL_Origin.Clear();
			DL_Origin.Add(0,t_("Not selected"));
			sql.Execute("select ID,ORIGIN from ORIGIN order by ORIGIN");
			while(sql.Fetch()) {
				DL_Origin.Add(
					sql[ID],
					sql[ORIGIN].ToString()
				);
			}			
			DL_Origin.SetIndex(0);
			DL_Origin.NotNull(true);
			break;
		
		case TABLE_LOCATION:
			DL_Location.Clear();
			DL_Location.Add(0,t_("Not selected"));
			sql.Execute("select ID,LABEL from LOCATION order by LABEL");
			while(sql.Fetch()) {
				DL_Location.Add(
					sql[ID],
					sql[LABEL].ToString()
				);
			}
			DL_Location.SetIndex(0);
			DL_Location.NotNull(true);
			break;
			
		case TABLE_STATE:
			DL_State.Clear();
			sql.Execute("select ID,LABEL,PAPER,INK from PCB_STATE");
			while(sql.Fetch()) {
				DL_State.Add(
				    sql[ID],
					AttrText(sql[LABEL].ToString())
						.Paper(Color::FromRaw(static_cast<dword>(sql[PAPER].To<int64>())))
						.Ink(Color::FromRaw(static_cast<dword>(sql[INK].To<int64>())))
				);
			}		
			break;
			
		case TABLE_TYPE:
			sql.Execute("select ID,LABEL from PCB_TYPE");
			while(sql.Fetch()) {
				DL_Type.Add(
					sql[ID],
					sql[LABEL].ToString()
				);
			}
			break;
			
		case TABLE_GAME:
			sql.Execute("select GAME.ID, MAKER_NAME, GAME_NAME from MAKER,GAME where GAME.MAKER_ID = MAKER.ID order by MAKER_NAME,GAME_NAME");
			while(sql.Fetch()) {
				String temp = sql[1].ToString() + " - " + sql[2].ToString();
				DL_Game.Add(
					sql[0],
					temp
				);
			}		
	}	
}

void PcbDlg::SetupDisplay(Ctrl* ctrl) {
	if (ctrl->GetLayoutId() == TabSignature.ES_SigOrigin.GetLayoutId()) {
		if (TabSignature.ES_SigOrigin.GetData() == "") {
			TabSignature.ES_SigOrigin <<= "Origin";
			TabSignature.ES_SigOrigin.SetStyle(editStyle_);
		}
	}
	if (ctrl->GetLayoutId() == TabSignature.ES_SigRange.GetLayoutId()) {
		if (TabSignature.ES_SigRange.GetData() == "") {
			TabSignature.ES_SigRange <<= "Range";
			TabSignature.ES_SigRange.SetStyle(editStyle_);
		}
	}

	if (ctrl->GetLayoutId() == TabSignature.ES_SigRomName.GetLayoutId()) {
		if (TabSignature.ES_SigRomName.GetData() == "") {
			TabSignature.ES_SigRomName <<= "Rom name";
			TabSignature.ES_SigRomName.SetStyle(editStyle_);
		}
	}
	if (ctrl->GetLayoutId() == TabSignature.ES_SigSection.GetLayoutId()) {
		if (TabSignature.ES_SigSection.GetData() == "") {
			TabSignature.ES_SigSection <<= "Section";
			TabSignature.ES_SigSection.SetStyle(editStyle_);
		}
	}
	if (ctrl->GetLayoutId() == TabSignature.ES_SigCrc32.GetLayoutId()) {
		if (TabSignature.ES_SigCrc32.GetData() == "") {
			TabSignature.ES_SigCrc32 <<= "CRC32";
			TabSignature.ES_SigCrc32.SetStyle(editStyle_);
		}
	}
	if (ctrl->GetLayoutId() == TabSignature.ES_SigSig.GetLayoutId()) {
		if (TabSignature.ES_SigSig.GetData() == "") {
			TabSignature.ES_SigSig <<= "Sig";
			TabSignature.ES_SigSig.SetStyle(editStyle_);
		}
	}
	if (ctrl->GetLayoutId() == TabPictures.ES_PictureLabel.GetLayoutId()) {
		if (TabPictures.ES_PictureLabel.GetData() == "") {
			TabPictures.ES_PictureLabel <<= "Label";
			TabPictures.ES_PictureLabel.SetStyle(editStyle_);
		}
	}
	if (ctrl->GetLayoutId() == TabPictures.ES_PicturePath.GetLayoutId()) {
		if (TabPictures.ES_PicturePath.GetData() == "") {
			TabPictures.ES_PicturePath <<= "Path";
			TabPictures.ES_PicturePath.SetStyle(editStyle_);
		}
	}
}

void PcbDlg::ResetDisplay(Ctrl* ctrl) {
	if (ctrl->GetLayoutId() == TabSignature.ES_SigOrigin.GetLayoutId()) {
		TabSignature.ES_SigOrigin.SetStyle(EditString::StyleDefault());
		if (TabSignature.ES_SigOrigin.GetData() == "Origin") {
			TabSignature.ES_SigOrigin.Erase();
		}
	}
	if (ctrl->GetLayoutId() == TabSignature.ES_SigRange.GetLayoutId()) {
		TabSignature.ES_SigRange.SetStyle(EditString::StyleDefault());
		if (TabSignature.ES_SigRange.GetData() == "Range") {
			TabSignature.ES_SigRange.Erase();
		}
	}

	if (ctrl->GetLayoutId() == TabSignature.ES_SigRomName.GetLayoutId()) {
		TabSignature.ES_SigRomName.SetStyle(EditString::StyleDefault());
		if (TabSignature.ES_SigRomName.GetData() == "Rom name") {
			TabSignature.ES_SigRomName.Erase();
		}
	}
	if (ctrl->GetLayoutId() == TabSignature.ES_SigSection.GetLayoutId()) {
		TabSignature.ES_SigSection.SetStyle(EditString::StyleDefault());
		if (TabSignature.ES_SigSection.GetData() == "Section") {
			TabSignature.ES_SigSection.Erase();
		}
	}
	if (ctrl->GetLayoutId() == TabSignature.ES_SigCrc32.GetLayoutId()) {
		TabSignature.ES_SigCrc32.SetStyle(EditString::StyleDefault());
		if (TabSignature.ES_SigCrc32.GetData() == "CRC32") {
			TabSignature.ES_SigCrc32.Erase();
		}
	}
	if (ctrl->GetLayoutId() == TabSignature.ES_SigSig.GetLayoutId()) {
		TabSignature.ES_SigSig.SetStyle(EditString::StyleDefault());
		if (TabSignature.ES_SigSig.GetData() == "Sig") {
			TabSignature.ES_SigSig.Erase();
		}
	}	
	if (ctrl->GetLayoutId() == TabPictures.ES_PictureLabel.GetLayoutId()) {
		 TabPictures.ES_PictureLabel.SetStyle(EditString::StyleDefault());
		if ( TabPictures.ES_PictureLabel.GetData() == "Label") {
			 TabPictures.ES_PictureLabel.Erase();
		}
	}	
	if (ctrl->GetLayoutId() == TabPictures.ES_PicturePath.GetLayoutId()) {
		 TabPictures.ES_PicturePath.SetStyle(EditString::StyleDefault());
		if ( TabPictures.ES_PicturePath.GetData() == "Path") {
			 TabPictures.ES_PicturePath.Erase();
		}
	}	
}

void PcbDlg::TabChanged() {
	Ctrl* child = NULL;
	int i = ~TC_Tab;
	switch (i) {
		case 1: // pictures tab
			child = TabPictures.GetFirstChild();
			while (child) {
				ResetDisplay(child);
				SetupDisplay(child);
				child = child->GetNext();	
			}
			
			PopulatePicturesArray();
			break;
		case 2: // Signature tab
			child = TabSignature.GetFirstChild();
			while (child) {
				ResetDisplay(child);
				SetupDisplay(child);
				child = child->GetNext();	
			}
			
			PopulateSignatureArray();
			break;
	}

}

void PcbDlg::DisplayPicture() {

	Popup p(TabPictures.TAB_Pictures.GetKey());
	p.SetRect(0,0,p.img_.GetWidth(),p.img_.GetHeight());
	p.CenterScreen();
	p.RunAppModal();

}

void PcbDlg::DisplayPicturePreview() {
	preview_.SetImage(TabPictures.TAB_Pictures.GetKey());
	preview_.Refresh();
}

void PcbDlg::SelectImage() {
    FileSel fs;
    String s = "";
    fs.Type("Image file", "*.bmp;*.png;*.jpg;*.jpeg");
    if(fs.ExecuteOpen("Choose the image file to open")) {
        TabPictures.ES_PicturePath = ~fs;
    }
}

void PcbDlg::AddImageToDatabase() {
	Image img = StreamRaster::LoadFileAny(AsString(~TabPictures.ES_PicturePath));
	JPGEncoder jpg;
	
	if ((img.GetWidth() > pictureWidth_) || (img.GetHeight() > pictureHeight_)) {
		// picture needs to be resized
		Size sz;
		sz.cx = pictureWidth_;
		sz.cy = pictureHeight_;
		Image newImg = Rescale(img, GetFitSize(img.GetSize(),sz));
		img = newImg;
	}

	SQL * Insert(PICTURE)(LABEL, ~TabPictures.ES_PictureLabel)(DATA, SqlBinary(jpg.SaveString(img)))(PCB_ID, ~E_PcbId);

	PopulatePicturesArray(); 
}

void PcbDlg::PopulatePicturesArray() {
	// Fills pictures array with data from database
	
	TabPictures.TAB_Pictures.Clear();
    SQL * Select(ID,LABEL).From(PICTURE).Where(PCB_ID == ~E_PcbId);
    while (SQL.Fetch()) {
        TabPictures.TAB_Pictures.Add(SQL[ID],SQL[LABEL]);
    }
}

void PcbDlg::PictureTabMenu(Bar& bar) {
	//bar.Add(t_("Create"),THISBACK(Create));
	//bar.Add(t_("Edit"),THISBACK1(Edit,0));
	bar.Add(t_("Remove"),THISBACK(RemovePicture));
}

void PcbDlg::RemovePicture() {
	// Picture removal from database
	SQL * Delete(PICTURE).Where(ID == TabPictures.TAB_Pictures.GetKey());
	
	// Table is reloaded
	PopulatePicturesArray();
}

void PcbDlg::SignatureTabMenu(Bar& bar) {
	//bar.Add(t_("Create"),THISBACK(Create));
	//bar.Add(t_("Edit"),THISBACK1(Edit,0));
	bar.Add(t_("Remove"),THISBACK(RemoveSignatureRecord));
}

void PcbDlg::RemoveSignatureRecord() {
	// Fluke record removal from database
	SQL * Delete(FLUKE).Where(ID == TabSignature.TAB_Signature.GetKey());
	
	// Table is reloaded
	PopulateSignatureArray();
}

void PcbDlg::AddSignatureRecord() {

	bool bInsert = true;
	if ( (~TabSignature.ES_SigRomName == "Rom name") ||
		(~TabSignature.ES_SigSection == "Section") ||
		(~TabSignature.ES_SigRange == "Range") ||
		(~TabSignature.ES_SigOrigin == "Origin") ||
		(~TabSignature.ES_SigCrc32 == "CRC32") ||
		(~TabSignature.ES_SigSig == "Sig") ) {
		
		if (!PromptYesNo(t_("At least one of the fields isn't filled. Do you want to add the record anyway ?")) ) {
			bInsert = false;
		}
	}
	
	if (bInsert) {
		SQL * Insert(FLUKE)(ROM_NAME, ~TabSignature.ES_SigRomName)
				(SECTION, ~TabSignature.ES_SigSection)
				(CRC_32, ~TabSignature.ES_SigCrc32)
				(FLUKE_SIG, ~TabSignature.ES_SigSig)
				(PCB_ID, ~E_PcbId)
				(RANGE, ~TabSignature.ES_SigRange)
				(ORIGIN, ~TabSignature.ES_SigOrigin);
	
		PopulateSignatureArray(); 
	}
}

void PcbDlg::PopulateSignatureArray() {
	// Fills signature array with data from database
	
	TabSignature.TAB_Signature.Clear();
    SQL * Select(ID,ROM_NAME,SECTION,CRC_32,FLUKE_SIG,RANGE,ORIGIN).From(FLUKE).Where(PCB_ID == ~E_PcbId).OrderBy(ROM_NAME);
    while (SQL.Fetch()) {
        TabSignature.TAB_Signature.Add(SQL[ID],SQL[ROM_NAME],SQL[SECTION],SQL[RANGE],SQL[ORIGIN],SQL[CRC_32],SQL[FLUKE_SIG]);
    }
}

void PcbDlg::LoadActionTreeFromDatabase()
{
	Sql sql;
	
	// key is used to give an unique identifier to each action record
	// it'll be added to the key member of each treecontrol line
	int key = ActionRecordsKey();

	// query is done to fill action vector with current PCB_ACTION data
	sql.Execute(Format("SELECT * FROM PCB_ACTION WHERE PCB_ID = %i ORDER BY PARENT_ID,ACTION_DATE", pcbId_));
	while(sql.Fetch())
	{
		++key; // key is incremented
		
		ActionRecord ar;
		ar.id 		    = sql[ID];
		ar.pcbId        = sql[PCB_ID];
	    ar.parentIndex  = sql[PARENT_ID];
	    ar.nodeIndex	= key;
	    ar.key 			= key;
		ar.date 		= sql[ACTION_DATE];
		ar.commentary 	= sql[COMMENTARY];
		ar.finished 	= static_cast<bool>(StrInt(AsString(sql[FINISHED]))); // defined as text in the database
		ar.type 		= sql[ACTION_TYPE];

		actionRecords_.push_back(ar);
	}
	
	// key is saved back to the object member variable
	ActionRecordsKey(key);
	
	BuildActionTree(); // Treecontrol is reloaded
}

void PcbDlg::SaveActionTreeToDatabase()
{
    TreeCtrl tc; // temporary treecontrol used to get correct ids if deletions occured in the main treecontrol
	tc.SetRoot(Null,0,t_("Analysis & Actions")); // setting up root
	tc.NoRoot(false);

    // parent ids correspondance
    // first member is old parent id, second member is new parent id
    // Will contain correspondance between TC_AnalysisAction ids and tc ids 
    map<int, int> pic;
    
    TC_AnalysisAction.OpenDeep(0); // displays all the lines of the treecontrol to save
    
    int max = TC_AnalysisAction.GetLineCount(); // getting number of lines of the treecontrol
    
    for (int i=1; i<max; ++i) { // going through treecontrol lines to get the items, starting at 1 as 0 is root
    	// putting the old keys in the map
    	int id = TC_AnalysisAction.GetParent(TC_AnalysisAction.GetItemAtLine(i));
    	if (id == 0)
    	{
    		// old key is added to the map, second member will be filled later
    		pic[TC_AnalysisAction.GetItemAtLine(i)] = 0;
    	}
    }
    
    int newId = 1; // value incremented by the child number
    
    // Using iterators to go through the map values (using C++11 syntax)
    // in order to fill missing new values
    for (auto iterator = pic.begin(); iterator != pic.end(); ++iterator) {
    
        iterator->second = newId; // new key is added to the map
        LOG(Format("OLD_KEY=%i,NEW_KEY=%i",iterator->first, iterator->second)); 
        // new key is incremented with current parent child number
        newId += TC_AnalysisAction.GetChildCount(iterator->first);
        ++newId;
    }
    
    // Going through TC_AnalysisAction records to copy them to tc, replacing ids in the process
    for (int i=1; i<max; ++i) {
        // Getting node corresponding to the current line in TC_AnalysisAction
        TreeCtrl::Node n = TC_AnalysisAction.GetNode( TC_AnalysisAction.GetItemAtLine(i) );
		
		// Getting parent id
		int pId = TC_AnalysisAction.GetParent( TC_AnalysisAction.GetItemAtLine(i) );
		        
        // Ids are changed using map correspondance
        tc.Add( pic[pId], n);
    }
    
    // Current records for the pcb are deleted from the database prior adding the new ones
    Sql sql;
    sql.Execute(Format("DELETE FROM PCB_ACTION WHERE PCB_ID = %i",PcbId()));

	tc.OpenDeep(0);
	
    // Going through tc records to add them to the database
    for (int i=1; i<max; ++i) {
        // Getting node corresponding to the current line in tc
        TreeCtrl::Node n = tc.GetNode( tc.GetItemAtLine(i) );
		
		// Getting corresponding record in the vector using the key
		int key = n.key;
        vector<ActionRecord>::iterator it = std::find_if(actionRecords_.begin(), actionRecords_.end(), [&key](const ActionRecord& ar)
        {
            return ar.key == key;
        });		
        
        if (it == actionRecords_.end()) PromptOK(t_("Node not found in vector !"));
        else {
        	// Record is added to the database
	       sql * Insert(PCB_ACTION)
	        		(PCB_ID, it->pcbId)
	        		(PARENT_ID, it->parentIndex)
	        		(ACTION_DATE, Time(it->date))
	        		(COMMENTARY, it->commentary)
	        		(FINISHED, it->finished)
	        		(ACTION_TYPE, it->type);        	
        }
    }
}

void PcbDlg::TreeDrag()
{
    // Drag & drop is initiated
    if (TC_AnalysisAction.DoDragAndDrop(InternalClip(TC_AnalysisAction, "mytreedrag"), TC_AnalysisAction.GetDragSample()) == DND_MOVE)
    {
        TC_AnalysisAction.RemoveSelection();
        
    }
}

void PcbDlg::TreeDropInsert(const int parent, const int ii, PasteClip& d)
{
    // Check type of drag data, and restrict to analysis level
    if (IsAvailableInternal<TreeCtrl>(d, "mytreedrag") && TreeGetLevel(parent) == 1) {
        
        if (parent == TC_AnalysisAction.GetCursor()) // preventing drop to self
        {
            d.Reject();
            return;
        }
        // Data can be accepted
        d.Accept();
        // If we haven't dropped the data yet (we are still dragging) don't do anything
        if (!d.IsPaste()) return;

        LOG("Before drop");
        LogActionVector();

        // User has dropped it! Do the insert
        TC_AnalysisAction.InsertDrop(parent, ii, d);
        TC_AnalysisAction.SetFocus();
        
        // Getting the record corresponding to the selected node
        int key = TC_AnalysisAction.Get();
        vector<ActionRecord>::iterator it = std::find_if(actionRecords_.begin(), actionRecords_.end(), [&key](const ActionRecord& ar)
        {
            return ar.key == key;
        });
        
        if (it != actionRecords_.end()) {
            // Corresponding record was found in the vector, we can update it
            it->parentIndex = TC_AnalysisAction.GetParent(TC_AnalysisAction.GetCursor());
        }

		BuildActionTree();
    
        return;
    }
}

int PcbDlg::TreeGetLevel(int id) const
{
    int i=0;
    while (id != 0){
        id = TC_AnalysisAction.GetParent(id);
        ++i;
    }
    return i;
}

void PcbDlg::RemoveActionFromVector(const int key)
{
    // Record is removed form the vector
    actionRecords_.erase(std::remove_if(actionRecords_.begin(), actionRecords_.end(), [&key](const ActionRecord& ar)
    {
        return ar.key == key;
    }), actionRecords_.end());    
}

ActionRecord& PcbDlg::GetActionFromVector(const int key)
{
    try
    {
        vector<ActionRecord>::iterator it = std::find_if(actionRecords_.begin(), actionRecords_.end(), [&key](const ActionRecord& ar)
        {
            return ar.key == key;
        });
    
        if (it == actionRecords_.end()) throw 0;
        
        return *it;
    }
    catch (int i)
    {
          i;
          PromptOK("Exception : record not found in vector !");
    }
}

void PcbDlg::AddActionToVector(ActionRecord ar)
{
    ar.key = ActionRecordsKey() + 1; // internal key is updated
    ActionRecordsKey(ar.key); // new value is written back
    
    if (ar.parentIndex == 0)
    { 
        // It's an analysis, added to the end of the treecontrol
        ar.nodeIndex = TC_AnalysisAction.GetLineCount();
        actionRecords_.push_back(ar);
    } else 
    {
        ar.nodeIndex = ar.parentIndex+1;
        actionRecords_.push_back(ar);
        SortActionVector();
    }
    
}

void PcbDlg::LogActionVector()
{
    LOG("actionRecord_ content : ");
    for (vector<ActionRecord>::iterator it = actionRecords_.begin(); it != actionRecords_.end(); ++it)
    {
	    LOG(Format("ID=%i,PARENT_ID=%i,KEY=%i, NODE_INDEX=%i",it->id, it->parentIndex, it->key, it->nodeIndex)); 
    }
}

void PcbDlg::DoOk()
{
    SaveActionTreeToDatabase();
    
    Break(IDOK);
}

void PcbDlg::SortActionVector()
{
     // Vector is sorted on nodeIndex member
     std::sort(actionRecords_.begin(), actionRecords_.end(), 
        [](ActionRecord const & a, ActionRecord const &b){return a.nodeIndex < b.nodeIndex;});  

    LogActionVector();  
}

void Popup::Paint(Draw& w)
{
    w.DrawRect(GetSize(), White);
    if(img_)
        w.DrawImage(0, 0, img_);
    else
       w.DrawText(0, 0, "No image loaded!", Arial(30).Italic());
}

void PreviewCtrl::Paint(Draw& w) {
  
	w.DrawRect(GetSize(),White);
	if (img_) {
		w.DrawImage(0, 0, this->img_);
		if ((img_.GetWidth() > previewWidth_) || (img_.GetHeight() > previewHeight_)) {
			// picture needs to be resized
			Size sz;
			sz.cx = previewWidth_;
			sz.cy = previewHeight_;
			Image newImg = Rescale(img_, GetFitSize(img_.GetSize(),sz));
			img_ = newImg;
		}
		w.DrawImage(0, 0, img_);
	}
	else
		w.DrawText(0, 0, "Preview not available!", Arial(12).Italic());

}