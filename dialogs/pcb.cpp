#include "pcb.h"
#include "action.h"
#include "origin.h"
#include "pinout.h"
#include "location.h"
#include "pcbstate.h"
#include "game.h"

#include "agar/utilities/converts.h"
#include "agar/utilities/lookups.h"

PcbDlg::PcbDlg(const int& openingType) {

	CtrlLayoutOKCancel(*this, t_("Pcb"));
	
	currentCtrl_ = 0;
	
	editStyle_ = EditString::StyleDefault();
	editStyle_.text = SGray();

	// Tabs setup
	Size sz = TC_Tab.GetSize();
	
	// Analysis & action tab
	TC_Tab.Add(TC_AnalysisAction.LeftPos(0, sz.cx).TopPos(0, sz.cy), t_("Analysis & Action"));
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
	
	// Fluke tab
	CtrlLayout(TabFluke);
	TC_Tab.Add(TabFluke, t_("Fluke"));
	TabFluke.TAB_Fluke.WhenBar = THISBACK(FlukeTabMenu);
	TabFluke.TAB_Fluke.AddIndex(ID);
	TabFluke.TAB_Fluke.AddColumn(ROM_NAME, t_("Rom name"));
	TabFluke.TAB_Fluke.AddColumn(SECTION, t_("Section"));
	TabFluke.TAB_Fluke.AddColumn(CRC_32, t_("CRC32"));
	TabFluke.TAB_Fluke.AddColumn(FLUKE_SIG, t_("Sig"));	
	TabFluke.TAB_Fluke.ColumnWidths("137 137 108 108");
	
	TabFluke.BTN_Add.WhenPush = THISBACK(AddFlukeRecord);
		
	TabFluke.ES_FlukeRomName.MaxChars(20);
	TabFluke.ES_FlukeSection.MaxChars(20);
	TabFluke.ES_FlukeCrc32.MaxChars(8);
	TabFluke.ES_FlukeSig.MaxChars(4); 
	
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

	child = TabFluke.GetFirstChild();
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
	TabFluke.BTN_Add.SetImage(MyImages::add);
	
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
	;
	
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

bool PcbDlg::GetFaultValue(const int& id, const String& faults) {
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

void PcbDlg::LoadDropList(const int& tableType) {
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
	if (ctrl->GetLayoutId() == TabFluke.ES_FlukeRomName.GetLayoutId()) {
		if (TabFluke.ES_FlukeRomName.GetData() == "") {
			TabFluke.ES_FlukeRomName <<= "Rom name";
			TabFluke.ES_FlukeRomName.SetStyle(editStyle_);
		}
	}
	if (ctrl->GetLayoutId() == TabFluke.ES_FlukeSection.GetLayoutId()) {
		if (TabFluke.ES_FlukeSection.GetData() == "") {
			TabFluke.ES_FlukeSection <<= "Section";
			TabFluke.ES_FlukeSection.SetStyle(editStyle_);
		}
	}
	if (ctrl->GetLayoutId() == TabFluke.ES_FlukeCrc32.GetLayoutId()) {
		if (TabFluke.ES_FlukeCrc32.GetData() == "") {
			TabFluke.ES_FlukeCrc32 <<= "CRC32";
			TabFluke.ES_FlukeCrc32.SetStyle(editStyle_);
		}
	}
	if (ctrl->GetLayoutId() == TabFluke.ES_FlukeSig.GetLayoutId()) {
		if (TabFluke.ES_FlukeSig.GetData() == "") {
			TabFluke.ES_FlukeSig <<= "Sig";
			TabFluke.ES_FlukeSig.SetStyle(editStyle_);
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
	if (ctrl->GetLayoutId() == TabFluke.ES_FlukeRomName.GetLayoutId()) {
		TabFluke.ES_FlukeRomName.SetStyle(EditString::StyleDefault());
		if (TabFluke.ES_FlukeRomName.GetData() == "Rom name") {
			TabFluke.ES_FlukeRomName.Erase();
		}
	}
	if (ctrl->GetLayoutId() == TabFluke.ES_FlukeSection.GetLayoutId()) {
		TabFluke.ES_FlukeSection.SetStyle(EditString::StyleDefault());
		if (TabFluke.ES_FlukeSection.GetData() == "Section") {
			TabFluke.ES_FlukeSection.Erase();
		}
	}
	if (ctrl->GetLayoutId() == TabFluke.ES_FlukeCrc32.GetLayoutId()) {
		TabFluke.ES_FlukeCrc32.SetStyle(EditString::StyleDefault());
		if (TabFluke.ES_FlukeCrc32.GetData() == "CRC32") {
			TabFluke.ES_FlukeCrc32.Erase();
		}
	}
	if (ctrl->GetLayoutId() == TabFluke.ES_FlukeSig.GetLayoutId()) {
		TabFluke.ES_FlukeSig.SetStyle(EditString::StyleDefault());
		if (TabFluke.ES_FlukeSig.GetData() == "Sig") {
			TabFluke.ES_FlukeSig.Erase();
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
		case 2: // Fluke tab
			child = TabFluke.GetFirstChild();
			while (child) {
				ResetDisplay(child);
				SetupDisplay(child);
				child = child->GetNext();	
			}
			
			PopulateFlukeArray();
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

void PcbDlg::FlukeTabMenu(Bar& bar) {
	//bar.Add(t_("Create"),THISBACK(Create));
	//bar.Add(t_("Edit"),THISBACK1(Edit,0));
	bar.Add(t_("Remove"),THISBACK(RemoveFlukeRecord));
}

void PcbDlg::RemoveFlukeRecord() {
	// Fluke record removal from database
	SQL * Delete(FLUKE).Where(ID == TabFluke.TAB_Fluke.GetKey());
	
	// Table is reloaded
	PopulateFlukeArray();
}

void PcbDlg::AddFlukeRecord() {

	bool bInsert = true;
	if ( (~TabFluke.ES_FlukeRomName == "Rom name") ||
		(~TabFluke.ES_FlukeSection == "Section") ||
		(~TabFluke.ES_FlukeCrc32 == "CRC32") ||
		(~TabFluke.ES_FlukeSig == "Sig") ) {
		
		if (!PromptYesNo(t_("At least one of the fields isn't filled. Do you want to add the record anyway ?")) ) {
			bInsert = false;
		}
	}
	
	if (bInsert) {
		SQL * Insert(FLUKE)(ROM_NAME, ~TabFluke.ES_FlukeRomName)
				(SECTION, ~TabFluke.ES_FlukeSection)
				(CRC_32, ~TabFluke.ES_FlukeCrc32)
				(FLUKE_SIG, ~TabFluke.ES_FlukeSig)
				(PCB_ID, ~E_PcbId);
	
		PopulateFlukeArray(); 
	}
}

void PcbDlg::PopulateFlukeArray() {
	// Fills Fluke array with data from database
	
	TabFluke.TAB_Fluke.Clear();
    SQL * Select(ID,ROM_NAME,SECTION,CRC_32,FLUKE_SIG).From(FLUKE).Where(PCB_ID == ~E_PcbId).OrderBy(ROM_NAME);
    while (SQL.Fetch()) {
        TabFluke.TAB_Fluke.Add(SQL[ID],SQL[ROM_NAME],SQL[SECTION],SQL[CRC_32],SQL[FLUKE_SIG]);
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