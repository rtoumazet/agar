#include "pcb.h"
#include "action.h"
#include "origin.h"
#include "pinout.h"
#include "location.h"
#include "pcbstate.h"

#include "agar/utilities/converts.h"
#include "agar/utilities/lookups.h"

PcbDlg::PcbDlg(const int& openingType) {

	CtrlLayoutOKCancel(*this, t_("Pcb"));
	
	currentCtrl_ = 0;
	
	editStyle_ = EditString::StyleDefault();
	editStyle_.text = SGray();

	// tab
	Size sz = TC_Tab.GetSize();
	TC_Tab.Add(TC_AnalysisAction.LeftPos(0, sz.cx).TopPos(0, sz.cy), t_("Analysis & Action"));
	CtrlLayout(TabPictures);
	TC_Tab.Add(TabPictures, t_("Pictures"));
	CtrlLayout(TabMisc);
	TC_Tab.Add(TabMisc, t_("Miscellaneous"));


	Ctrl* child = NULL;
	switch (openingType) {
		case OPENING_NEW:
			Title(t_("New PCB"));
			TC_AnalysisAction.NoRoot(true); // root of treecontrol is hidden as there's no entry in creation
			TC_AnalysisAction.Disable(); // no action allowed on the TC during creation
			
			child = TabMisc.GetFirstChild();
			while (child) {
				SetupDisplay(child);
				child = child->GetNext();	
			}
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
	
	// Hiding controls not to be displayed
	E_PcbId.Hide();
	ES_Faults.Hide();
	
	// displaying images on controls
	BTN_NewOrigin.SetImage(MyImages::add);
	BTN_NewLocation.SetImage(MyImages::add);
	BTN_NewPinout.SetImage(MyImages::add);
	//TabPictures.BTN_Select.SetImage(MyImages::analysis);
	
	ActiveFocus(DL_Game); // sets the focus to the first droplist
	
	// Filling droplists data
	LoadDropList(TABLE_GAME);
	LoadDropList(TABLE_TYPE);
	LoadDropList(TABLE_STATE);
	LoadDropList(TABLE_LOCATION);
	LoadDropList(TABLE_ORIGIN);
	LoadDropList(TABLE_PINOUT);
	
	TabPictures.TAB_Pictures.AddIndex(ID);
	TabPictures.TAB_Pictures.AddColumn(LABEL,t_("Label"));

	// Tree control
	TC_AnalysisAction.WhenBar = THISBACK(TreeControlMenu);
	
	// buttons actions
	BTN_NewOrigin.WhenPush = THISBACK1(CreateLinkedRecord, TABLE_ORIGIN);
	BTN_NewLocation.WhenPush = THISBACK1(CreateLinkedRecord, TABLE_LOCATION);
	BTN_NewPinout.WhenPush = THISBACK1(CreateLinkedRecord, TABLE_PINOUT);
	
	//TabPictures.BTN_Popup.WhenPush = THISBACK(DisplayPicture);
	TabPictures.BTN_Select.WhenPush = THISBACK(SelectImage);
	TabPictures.BTN_Add.WhenPush = THISBACK(AddImageToDatabase);
	TabPictures.TAB_Pictures.WhenLeftDouble = THISBACK(DisplayPicture);
	
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
		(ROM_NAME, TabMisc.ES_FlukeRomName)
		(SECTION, TabMisc.ES_FlukeSection)
		(CRC_32, TabMisc.ES_FlukeCrc32)
		(FLUKE_SIG, TabMisc.ES_FlukeSig)
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
	int y = r.top + 20;
	int linecy = Draw::GetStdFontCy() + 4;
	int current = 0; 
	sql.Execute("select ID,LABEL from PCB_FAULT order by LABEL");
	while(sql.Fetch()) {
		//Add(option.Add(sql[0]).SetLabel(sql[1].ToString()).TopPos(y, linecy).RightPos(10, 150));
		Add(option.Add(sql[0]).SetLabel(sql[1].ToString()).TopPos(y, linecy).LeftPos(r.left+10, 150));
		int id = StdConvertInt().Scan(sql[0].ToString());
		option[current].SetData(GetFaultValue(id));
		y += linecy;
		current++;
	}	
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

	int key = TC_AnalysisAction.Get();
	if(IsNull(key))
		return;
	
	ActionDlg dlg(~E_PcbId, key);
	
	if(dlg.Execute() != IDOK)
		return;
	
	SQL * dlg.ctrls.Update(PCB_ACTION).Where(ID == key);
	
	BuildActionTree(~E_PcbId);	
};

void PcbDlg::Remove() {
	// Removes currectly selected analysis / action in the treecontrol
	
	int key = TC_AnalysisAction.Get();
	if(IsNull(key) || !PromptYesNo(t_("Delete entry ?")))
	   return;
	SQL * SqlDelete(PCB_ACTION).Where(ID == key);	
	
	// Tree is rebuilt
	BuildActionTree(~E_PcbId);
}

void PcbDlg::AddAnalysis(const int& pcbId) {
	// Adds a new analysis
	ActionDlg dlg(pcbId, ActionDlg::ANALYSIS, ActionDlg::CREATION);
	if(dlg.Execute() != IDOK)
		return;

	SQL * dlg.ctrls.Insert(PCB_ACTION);
	if(SQL.WasError()){
    	PromptOK(SQL.GetLastError());
	}
	int id = SQL.GetInsertedId();
	
	BuildActionTree(pcbId);
};

void PcbDlg::AddAction(const int& pcbId) {
	// Adds a new action linked to an analysis
	ActionDlg dlg(pcbId, ActionDlg::ACTION, ActionDlg::CREATION, TC_AnalysisAction.GetCursor());
	
	if(dlg.Execute() != IDOK)
		return;

	SQL * dlg.ctrls.Insert(PCB_ACTION);
	if(SQL.WasError()){
    	PromptOK(SQL.GetLastError());
    	//PromptOK(SQL.GetErrorStatement());
	}
	int id = SQL.GetInsertedId();
	
	BuildActionTree(pcbId);	
};

int PcbDlg::GetRecordNumber(int const& pcbId) {
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

void PcbDlg::SetAddActionMenuEntryVisible(const bool& val) {
	addActionMenuEntryVisible_ = val;
}

void PcbDlg::SetEditMenuEntryVisible(const bool& val) {
	editMenuEntryVisible_ = val;
}

void PcbDlg::SetRemoveMenuEntryVisible(const bool& val) {
	removeMenuEntryVisible_ = val;
}

void PcbDlg::BuildActionTree(const int& pcbId) {
	// Fills the tree control with data from action file

	TC_AnalysisAction.Clear();
		
	Sql sql;
	sql.Execute(Format("select ID,PARENT_ID,COMMENTARY,FINISHED from PCB_ACTION where PCB_ID = %i order by ACTION_DATE",pcbId));
	
	TC_AnalysisAction.SetRoot(Null,0,t_("Analysis & Actions"));
	TC_AnalysisAction.NoRoot(false);

	while(sql.Fetch()) {
		Image img;
		if (sql[PARENT_ID]==0) {
			// Analysis
			img = MyImages::analysis();
		} else {
			if (sql[FINISHED]=="0") {
				img = MyImages::action();
			} else {
				 img = MyImages::actionDone();
			}
		}
		
		TC_AnalysisAction.Add(sql[PARENT_ID], img, sql[ID], sql[COMMENTARY]);
	}	
	
	TC_AnalysisAction.OpenDeep(0,true);
	
}

void PcbDlg::CreateLinkedRecord(const int& tableType) {
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
			DL_Pinout.SetIndex(id);
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
			DL_Origin.SetIndex(id);
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
			DL_Location.SetIndex(id);
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

void PcbDlg::LoadDropList(const int& tableType) {
	Sql sql;
	
	switch (tableType) {
		case TABLE_PINOUT:
			DL_Pinout.Clear();
			DL_Pinout.Add(0,t_("Not selected"));
			sql.Execute("select ID,LABEL,PIN_SIZE from PINOUT");
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
			sql.Execute("select ID,ORIGIN from ORIGIN");
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
			sql.Execute("select ID,LABEL from LOCATION");
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
				String temp = sql[1].ToString() + ' - ' + sql[2].ToString();
				DL_Game.Add(
					sql[0],
					temp
				);
			}		
	}	
}

void PcbDlg::SetupDisplay(Ctrl* ctrl) {
	if (ctrl->GetLayoutId() == TabMisc.ES_FlukeRomName.GetLayoutId()) {
		if (TabMisc.ES_FlukeRomName.GetData() == "") {
			TabMisc.ES_FlukeRomName <<= "Rom name";
			TabMisc.ES_FlukeRomName.SetStyle(editStyle_);
		}
	}
	if (ctrl->GetLayoutId() == TabMisc.ES_FlukeSection.GetLayoutId()) {
		if (TabMisc.ES_FlukeSection.GetData() == "") {
			TabMisc.ES_FlukeSection <<= "Section";
			TabMisc.ES_FlukeSection.SetStyle(editStyle_);
		}
	}
	if (ctrl->GetLayoutId() == TabMisc.ES_FlukeCrc32.GetLayoutId()) {
		if (TabMisc.ES_FlukeCrc32.GetData() == "") {
			TabMisc.ES_FlukeCrc32 <<= "CRC32";
			TabMisc.ES_FlukeCrc32.SetStyle(editStyle_);
		}
	}
	if (ctrl->GetLayoutId() == TabMisc.ES_FlukeSig.GetLayoutId()) {
		if (TabMisc.ES_FlukeSig.GetData() == "") {
			TabMisc.ES_FlukeSig <<= "Sig";
			TabMisc.ES_FlukeSig.SetStyle(editStyle_);
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
	if (ctrl->GetLayoutId() == TabMisc.ES_FlukeRomName.GetLayoutId()) {
		TabMisc.ES_FlukeRomName.SetStyle(EditString::StyleDefault());
		if (TabMisc.ES_FlukeRomName.GetData() == "Rom name") {
			TabMisc.ES_FlukeRomName.Erase();
		}
	}
	if (ctrl->GetLayoutId() == TabMisc.ES_FlukeSection.GetLayoutId()) {
		TabMisc.ES_FlukeSection.SetStyle(EditString::StyleDefault());
		if (TabMisc.ES_FlukeSection.GetData() == "Section") {
			TabMisc.ES_FlukeSection.Erase();
		}
	}
	if (ctrl->GetLayoutId() == TabMisc.ES_FlukeCrc32.GetLayoutId()) {
		TabMisc.ES_FlukeCrc32.SetStyle(EditString::StyleDefault());
		if (TabMisc.ES_FlukeCrc32.GetData() == "CRC32") {
			TabMisc.ES_FlukeCrc32.Erase();
		}
	}
	if (ctrl->GetLayoutId() == TabMisc.ES_FlukeSig.GetLayoutId()) {
		TabMisc.ES_FlukeSig.SetStyle(EditString::StyleDefault());
		if (TabMisc.ES_FlukeSig.GetData() == "Sig") {
			TabMisc.ES_FlukeSig.Erase();
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
		case 2: // Misc tab
			child = TabMisc.GetFirstChild();
			while (child) {
				ResetDisplay(child);
				SetupDisplay(child);
				child = child->GetNext();	
			}
			break;
	}

}

void PcbDlg::DisplayPicture() {

	Popup p(TabPictures.TAB_Pictures.GetKey());
	p.SetRect(0,0,800,600);
	p.CenterScreen();
	p.RunAppModal();

}

void PcbDlg::SelectImage() {
    FileSel fs;
    String s = "";
    fs.Type("Image file", "*.bmp;*.png;*.jpg;*.jpeg");
    if(fs.ExecuteOpen("Choose the image file to open")) {
        TabPictures.ES_PicturePath = ~fs;
        //img_ = StreamRaster::LoadFileAny(~fs);
        //PNGEncoder png;
        
        //SQL * Insert(PICTURE)(LABEL, "TEST")(DATA, SqlBinary(png.SaveString(img_)));
		//Refresh();
    }
	
    //Sizeable();
    //return s;
}

void PcbDlg::AddImageToDatabase() {
	Image img = StreamRaster::LoadFileAny(AsString(~TabPictures.ES_PicturePath));
	PNGEncoder png;

	SQL * Insert(PICTURE)(LABEL, ~TabPictures.ES_PictureLabel)(DATA, SqlBinary(png.SaveString(img)))(PCB_ID, ~E_PcbId);

	PopulatePicturesArray(); 
}

void PcbDlg::PopulatePicturesArray() {
	// Fills pictures array with data from database
	
	TabPictures.TAB_Pictures.Clear();
    SQL * Select(ID,LABEL).From(PICTURE).Where(PCB_ID == ~E_PcbId);
    if (SQL.Fetch()) {
        TabPictures.TAB_Pictures.Add(SQL[ID],SQL[LABEL]);
    }
}

void Popup::Paint(Draw& w)
{
    w.DrawRect(GetSize(), White);
    if(img_)
        w.DrawImage(0, 0, img_);
    else
       w.DrawText(0, 0, "No image loaded!", Arial(30).Italic());
}