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
	;
	
	LoadFaultData();
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
		//Add(option.Add(sql[0]).SetLabel(sql[1].ToString()).TopPos(y, linecy).LeftPos(r.left+10, 150));
		Add(option.Add(sql[ID]).SetLabel(sql[LABEL].ToString()).TopPosZ(y, linecy).RightPosZ(10, 150));
		TabMisc.Add(optionOrigin_.Add(sql[ID]).SetLabel(sql[LABEL].ToString()).TopPos(yOrigin, linecy).LeftPos(rOrigin.left+10, 150));
		int id = StdConvertInt().Scan(sql[0].ToString());
		option[current].SetData(GetFaultValue(id, ES_Faults));
		optionOrigin_[current].SetData(GetFaultValue(id, ES_FaultsOrigin));
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

	bar.Add(t_("Add analysis"),THISBACK1(AddAnalysis, ~E_PcbId));
	bar.Add(DisplayAddActionMenuEntry(), t_("Link an action"), THISBACK1(AddAction, ~E_PcbId));
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
	
	int key = TC_AnalysisAction.Get();
	if(IsNull(key) || !PromptYesNo(t_("Delete entry ?")))
	   return;
	
	// Removal from treecontrol
	int parent = TC_AnalysisAction.GetParent(TC_AnalysisAction.GetCursor());
	TC_AnalysisAction.Remove(TC_AnalysisAction.GetCursor());
	TC_AnalysisAction.RefreshItem(parent);
	
	// Removal from vector
	RemoveActionFromVector(key);

    // Parent id of each record in the vector is updated regarding treecontrol data
    UpdateParentIdInVector();
	
	BuildActionTree();
}

void PcbDlg::AddAnalysis(const int pcbId) {
	// Adds a new analysis
	ActionDlg dlg(pcbId, ActionDlg::ANALYSIS);
	if(dlg.Execute() != IDOK)
		return;
    
    // Adding the new record to the vector
    AddActionToVector(dlg.Record());

    // Parent id of each record in the vector is updated regarding treecontrol data
    UpdateParentIdInVector();

	BuildActionTree();
};

void PcbDlg::AddAction(const int pcbId) {
	// Adds a new action linked to an analysis
	ActionDlg dlg(pcbId, ActionDlg::ACTION, TC_AnalysisAction.GetCursor());
	
	if(dlg.Execute() != IDOK)
		return;
	
    // Adding the new record to the vector
    AddActionToVector(dlg.Record());
    
    // Parent id of each record in the vector is updated regarding treecontrol data
    UpdateParentIdInVector();

	BuildActionTree();	
};

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
	// if PARENT_ID is 0, record's an analysis
	// otherwise record's an action, and PARENT_ID refers to the index position in the treecontrol
	// Root of the tree control has index 0, first analysis has index 1
	vector<ActionRecord> actions; // will hold actions
	vector<ActionRecord> analysis; // will hold analysis
	
	// looping through records to fill actions & analysis vectors
    for (vector<ActionRecord>::iterator it = actionRecords_.begin(); it != actionRecords_.end(); ++it)
    {
	    if( it->parentId == 0) analysis.push_back(*it);
	    else actions.push_back(*it);
    }
	
	TC_AnalysisAction.Clear(); // treecontrol is emptied
	TC_AnalysisAction.SetRoot(Null,0,t_("Analysis & Actions")); // setting up root
	TC_AnalysisAction.NoRoot(false);

	// actions & analysis vectors are set, time to build treecontrol
	int iParent = 1; // current parent index in the treecontrol. Initialized to 1 as 0 is root

	for (vector<ActionRecord>::iterator it = analysis.begin(); it != analysis.end(); ++it) // analysis loop
	{
		// current analysis is added to the treecontrol
		TC_AnalysisAction.Add(0, MyImages::analysis(), it->id, it->commentary, false);
		int displacement = 1; // number of actions added to the treecontrol for current parent
		for (vector<ActionRecord>::iterator it = actions.begin(); it != actions.end(); ++it) // actions loop
		{
			if (it->parentId != iParent) break; // loop is exited when current record isn't linked to current parent (vector is sorted)
			
			// adding action to the treecontrol
			TC_AnalysisAction.Add(iParent, !it->finished ? MyImages::action() : MyImages::actionDone(), it->id, it->commentary, false);
			displacement++;
		}

        // removing actions linked to current parent_id from vector
        actions.erase(std::remove_if(actions.begin(), actions.end(), [&iParent](const ActionRecord& ar)
        {
            return ar.parentId == iParent;
        }), actions.end());
        
        // updating parent index position
        iParent += displacement;
	}
	
	TC_AnalysisAction.OpenDeep(0,true); // opening all nodes
	
    // Handling orphan actions
	if (!actions.empty()) 
	{
	    PromptOK(t_("Orphan actions exist for current pcb ! Use drag & drop to link them to an analysis"));

        // orphans actions are added to the treecontrol as analysis with a special icon
		for (vector<ActionRecord>::iterator it = actions.begin(); it != actions.end(); ++it) // actions loop
		{
			// adding action to the treecontrol
			TC_AnalysisAction.Add(0, MyImages::warning(), it->id, it->commentary, false);
		}
	}
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
	// query is done to fill action vector with current PCB_ACTION data
	Sql sql;
	sql.Execute(Format("SELECT * FROM PCB_ACTION WHERE PCB_ID = %i ORDER BY PARENT_ID,ACTION_DATE", pcbId_));
	while(sql.Fetch())
	{
		ActionRecord ar;
		ar.id 		    = sql[ID];
		ar.pcbId        = sql[PCB_ID];
		ar.parentId 	= sql[PARENT_ID];
		ar.date 		= sql[ACTION_DATE];
		ar.commentary 	= sql[COMMENTARY];
		ar.finished 	= static_cast<bool>(StrInt(AsString(sql[FINISHED]))); // defined as text in the database
		ar.type 		= sql[ACTION_TYPE];

		actionRecords_.push_back(ar);
	}
	
	BuildActionTree(); // Treecontrol is reloaded
}

void PcbDlg::SaveActionTreeToDatabase()
{
    // Going through the treecontrol lines to update each parentId in the vector
    UpdateParentIdInVector();

    // Current records for the pcb are deleted from the database prior adding the new ones
    Sql sql;
    sql.Execute(Format("DELETE FROM PCB_ACTION WHERE PCB_ID = %i",PcbId()));
    if (sql.WasError()){
		PromptOK(sql.GetErrorCodeString());
	}

    // Going through the vector to add the records
    for ( vector<ActionRecord>::const_iterator it = actionRecords_.begin(); it!=actionRecords_.end(); it++)
    {
        //sql.Execute(s);
        sql * Insert(PCB_ACTION)
        		(PCB_ID, it->pcbId)
        		(PARENT_ID, it->parentId)
        		(ACTION_DATE, Time(it->date))
        		(COMMENTARY, it->commentary)
        		(FINISHED, it->finished)
        		(ACTION_TYPE, it->type);

        
        if (sql.WasError()){
			//Cout() << m_session.GetErrorCodeString() << "\n";
			PromptOK(sql.GetErrorCodeString());
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

        // User has dropped it! Do the insert
        TC_AnalysisAction.InsertDrop(parent, ii, d);
        TC_AnalysisAction.SetFocus();

        // Getting the record corresponding to the selected node
        int id = TC_AnalysisAction.Get();
        vector<ActionRecord>::iterator it = std::find_if(actionRecords_.begin(), actionRecords_.end(), [&id](const ActionRecord& ar)
        {
            return ar.id == id;
        });
        
        // Setting up the right image
        Image img;
        if (it != actionRecords_.end())
        {
            if (!it->finished) img = MyImages::action();
            else img = MyImages::actionDone();
        } else img = MyImages::action();
        
        // Setting up the new parent
        it->parentId = parent;
        
        // Node is updated
        int i = TC_AnalysisAction.GetCursor();
        TreeCtrl::Node n = TC_AnalysisAction.GetNode(i);
        n.SetImage(img);
        TC_AnalysisAction.SetNode(i, n);

        return;
    }
}

int PcbDlg::TreeGetLevel(int id) const
{
    int i=0;
    while (id != 0){
        id = TC_AnalysisAction.GetParent(id);
        i++;
    }
    return i;   
}

void PcbDlg::RemoveActionFromVector(const int id)
{
    actionRecords_.erase(std::remove_if(actionRecords_.begin(), actionRecords_.end(), [&id](const ActionRecord& ar)
    {
        return ar.id == id;
    }), actionRecords_.end());    
}

ActionRecord& PcbDlg::GetActionFromVector(const int id)
{
    try
    {
        vector<ActionRecord>::iterator it = std::find_if(actionRecords_.begin(), actionRecords_.end(), [&id](const ActionRecord& ar)
        {
            return ar.id == id;
        });  
    
        if (it == actionRecords_.end()) throw 0;
        
        return *it;
    }
    catch (int i)
    {
          PromptOK("Exception : record not found in vector !");
    }
    
        
}

void PcbDlg::AddActionToVector(const ActionRecord& ar)
{
    actionRecords_.push_back(ar);
}

void PcbDlg::DoOk()
{
    SaveActionTreeToDatabase();
    
    Break(IDOK);
}

void PcbDlg::UpdateParentIdInVector()
{
    // Going through the treecontrol lines to update each parentId in the vector
    TC_AnalysisAction.OpenDeep(0,true); // opening all nodes
    
    TC_AnalysisAction.SortDeep(0);
    
    for ( vector<ActionRecord>::iterator it = actionRecords_.begin(); it!=actionRecords_.end(); it++)
    {
        // getting the node id in the treecontrol based on the key of the record
        int treeId = TC_AnalysisAction.Find(it->id);
        if (treeId<0) PromptOK(t_("Key not found"));
        else
        {
            it->parentId = TC_AnalysisAction.GetParent(treeId);
        }
    }
   
    /*for (int i=1; i<TC_AnalysisAction.GetLineCount(); i++) // looping through treecontrol elements, leaving root out
    {
        // getting the key (ie id) corresponding to the current line
        int key = TC_AnalysisAction[i];
        
        PromptOK(AsString(TC_AnalysisAction.GetValue(i)));
        
        if (key>=0)
        {
            // let's grab a reference to the corresponding record in the vector
            ActionRecord& ar = GetActionFromVector(key);
            
            // updating the parent id of the record
            ar.parentId = TC_AnalysisAction.GetParent(i);
        }
    } */
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
		//w.DrawImage(0, 0, this->img_);
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