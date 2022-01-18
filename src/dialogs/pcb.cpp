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
#include "viewer.h"
#include "../utilities/converts.h" // toUnderlying

using namespace std;

enum class ResizePicture {
	do_resize = 0,
	do_not_resize = 1
};

void PreviewCtrl::SetImage(const int id) {
	if( id < 0) return;
	Sql sql;
	sql * Select(PREVIEW_DATA, DATA).From(PICTURE).Where(ID == id);
	if (sql.Fetch()) {
		if (sql[PREVIEW_DATA].IsNull()) {
            const auto sz = Size{preview_width, preview_height};
            img_ = JPGRaster().LoadString(sql[DATA]);
            preview_img_ = Rescale(img_, GetFitSize(img_.GetSize(),sz));

			JPGEncoder jpg;
			Sql sql_update;
			sql_update * ::Update(PICTURE)
			(PREVIEW_DATA, SqlBinary(jpg.SaveString(preview_img_)))
			.Where(ID == id);

			if (sql_update.WasError()) {
			    PromptOK(sql_update.GetErrorCodeString());
			}
		} else {
			JPGRaster jpgr;
			preview_img_ = jpgr.LoadString(sql[PREVIEW_DATA]);
		}
	}
}

void PreviewCtrl::Paint(Draw& w) {
	w.DrawRect(GetSize(),White);
	if (preview_img_) { w.DrawImage(0, 0, this->preview_img_); }
	else { w.DrawText(0, 0, "Preview not available!", Arial(12).Italic()); }
}
		
PcbDlg::PcbDlg(const OpeningType type, const int pcb_id)
{

	CtrlLayout(*this, t_("Pcb"));
	
    // Setting up buttons callbacks
    ok <<= THISBACK(doOk);
    cancel <<= Rejector(IDCANCEL);
	
	pcb_id_ = pcb_id;
	
	edit_style_ = EditString::StyleDefault();
	edit_style_.text = SGray();

	// Tabs setup
	auto sz = Size{TC_Tab.GetSize()};
	
	// Analysis & action tab
	TC_Tab.Add(TC_AnalysisAction.LeftPos(0, sz.cx-10).TopPos(0, sz.cy-15), t_("Analysis & Action"));
	CtrlLayout(pictures_tab_);
	
	// Pictures tab
	TC_Tab.Add(pictures_tab_, t_("Pictures"));
	pictures_tab_.pictures.NoVertGrid();
	pictures_tab_.pictures.AddIndex(ID);
	pictures_tab_.pictures.AddColumn(LABEL,t_("Label")).Edit(picture_label_);
	pictures_tab_.pictures.AddColumn("").Margin(0);
	pictures_tab_.pictures.AddColumn("").Margin(0);
	pictures_tab_.pictures.AddColumn("").Margin(0);
	pictures_tab_.pictures.WhenLeftDouble = THISBACK(displayPicture);
	pictures_tab_.pictures.WhenLeftClick = THISBACK(displayPicturePreview);
	pictures_tab_.pictures.WhenAcceptEdit = THISBACK(updatePictureLabel);
    pictures_tab_.pictures.ColumnWidths("195 16 16 16");
	pictures_tab_.Add(preview_.RightPosZ(-15, 250).TopPosZ(20, 200));
    
	// Signature tab
	CtrlLayout(signature_tab_);
	TC_Tab.Add(signature_tab_, t_("Signatures"));
	signature_tab_.signatures.WhenBar = THISBACK(generateSignatureTabMenu);
	signature_tab_.signatures.AddIndex(ID);
	signature_tab_.signatures.AddColumn(ROM_NAME, t_("Rom name"));
	signature_tab_.signatures.AddColumn(SECTION, t_("Section"));
	signature_tab_.signatures.AddColumn(RANGE, t_("Range"));
	signature_tab_.signatures.AddColumn(ORIGIN, t_("Origin"));
	signature_tab_.signatures.AddColumn(CRC_32, t_("CRC32"));
	signature_tab_.signatures.AddColumn(FLUKE_SIG, t_("Sig"));
	signature_tab_.signatures.ColumnWidths("137 137 108 108 60 30");
	
	signature_tab_.BTN_Add.WhenPush = THISBACK(addSignatureRecord);
		
	signature_tab_.ES_SigRomName.MaxChars(20);
	signature_tab_.ES_SigSection.MaxChars(20);
	signature_tab_.ES_SigCrc32.MaxChars(8);
	signature_tab_.ES_SigSig.MaxChars(4);
	
	// Miscellaneous tab
	CtrlLayout(misc_tab_);
	TC_Tab.Add(misc_tab_, t_("Miscellaneous"));
	
	//Ctrl* child = NULL;
	switch (type) {
		case OpeningType::opening_new: {
			Title(t_("New PCB"));
			TC_AnalysisAction.NoRoot(true); // root of treecontrol is hidden as there's no entry in creation
			TC_AnalysisAction.Disable(); // no action allowed on the TC during creation
			
			auto child = pictures_tab_.GetFirstChild();
			while (child) {
				setupDisplay(child);
				child = child->GetNext();
			}
			break;
		}
		case OpeningType::opening_edit:
			Title(t_("Edit PCB"));
			break;
	}

	auto signature_child = signature_tab_.GetFirstChild();
	while (signature_child) {
		setupDisplay(signature_child);
		signature_child = signature_child->GetNext();
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
	signature_tab_.BTN_Add.SetImage(MyImages::add);
	
	ActiveFocus(DL_Game); // sets the focus to the first droplist
	
	// Filling droplists data
	loadDropList(TableType::game);
	loadDropList(TableType::type);
	loadDropList(TableType::state);
	loadDropList(TableType::location);
	loadDropList(TableType::origin);
	loadDropList(TableType::pinout);
	

	// Tree control
	TC_AnalysisAction.WhenBar = THISBACK(createAnalysisAndActionsMenu);
	TC_AnalysisAction.WhenDrag = THISBACK(treeDrag);
	TC_AnalysisAction.WhenDropInsert = THISBACK(treeDropInsert);
	TC_AnalysisAction.WhenLeftDouble = THISBACK(editRecord);
    
    loadActionTreeFromDatabase();
	
	// buttons actions
	BTN_NewGame.WhenPush = THISBACK1(createLinkedRecord, TableType::game);
	BTN_NewOrigin.WhenPush = THISBACK1(createLinkedRecord, TableType::origin);
	BTN_NewLocation.WhenPush = THISBACK1(createLinkedRecord, TableType::location);
	BTN_NewPinout.WhenPush = THISBACK1(createLinkedRecord, TableType::pinout);
	
	// Tab action
	TC_Tab.WhenSet = THISBACK(tabChanged);
	
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

	pictures_tab_.pictures.WhenDrop = [=] (PasteClip& d) { dragAndDrop(d, pictures_tab_.pictures); };
}

PcbDlg::~PcbDlg() {
	
}

void PcbDlg::generateFaultData()
{
	// builds the string containing checked values
	auto faults = String{""};
	auto key    = Value{};
	auto data   = Value{};
	// Current faults
	for (int i=0; i<option_.GetCount(); ++i) {
		key    = option_.GetKey(i);
		data   = option_[i].GetData();
		faults += key.ToString() +":";
		faults += data.ToString() +";";
	}
	ES_Faults = faults;
	
	// Original faults
	faults = "";
	for (int i=0; i<option_origin_.GetCount(); ++i) {
		key     = option_origin_.GetKey(i);
		data    = option_origin_[i].GetData();
		faults += key.ToString() +":";
		faults += data.ToString() +";";
	}
	ES_FaultsOrigin = faults;
}

auto PcbDlg::getFaultValue(const int id, const String& faults) -> bool {
	// returns the option state for the fault id parameter

	auto start_pos = int{0};
	auto end_pos = int{0};
	auto ret = bool{false};
	auto str = String{faults};
	auto sub_str = String{""};
	end_pos = str.Find(";",start_pos);
	while (end_pos != -1) {
		sub_str = str.Mid(start_pos, end_pos - start_pos);
		auto end = int{sub_str.Find(":")};
		auto val = int{StdConvertInt().Scan(sub_str.Mid(0, end))};
		if (val == id) {
			// id was found, getting the option value
			ret = StdConvertInt().Scan(sub_str.Mid(end+1));
			break;
		}

		// moving to the next chunk
		start_pos = end_pos + 1;
		end_pos = str.Find(";",start_pos);
	}
	
	return ret;
}

void PcbDlg::loadFaultData()
{
	// fault option list
	
	auto r = Rect{L_Faults.GetRect()};
	auto r_origin = Rect{misc_tab_.L_Faults.GetRect()};
	
	//int y = r.top + 20;
	auto y = int{80 + 20};
	auto y_origin = int{r_origin.top + 20};
	auto linecy = int{Draw::GetStdFontCy() + 2};
	auto current = int{0};
	Sql sql;
	sql.Execute("select ID,LABEL from PCB_FAULT order by LABEL");
	while(sql.Fetch()) {
		Ctrl::Add(option_.Add(sql[ID]).SetLabel(sql[LABEL].ToString()).TopPosZ(y, linecy).RightPosZ(10, 150));
		misc_tab_.Add(option_origin_.Add(sql[ID]).SetLabel(sql[LABEL].ToString()).TopPos(y_origin, linecy).LeftPos(r_origin.left+10, 150));
		int id = StdConvertInt().Scan(sql[0].ToString());
		option_[current].SetData(getFaultValue(id, ~ES_Faults));
		option_origin_[current].SetData(getFaultValue(id, ~ES_FaultsOrigin));
		y += linecy;
		y_origin += linecy;
		++current;
	}
	
	// Labels position override
	//L_Faults.RightPosZ(70, 136).TopPosZ(r.top, y - 100);
	L_Faults.TopPosZ(r.top, y - 80);
	misc_tab_.L_Faults.TopPosZ(r_origin.top, y - 100);
}

void PcbDlg::createAnalysisAndActionsMenu(Bar& bar)
{
	auto isAddActionEntryVisible = bool{false};
	auto isEditEntryVisible      = bool{false};
	auto isRemoveEntryVisible    = bool{false};

	// Setting options to display
	const auto id = int{TC_AnalysisAction.GetCursor()};
	if (id != -1 && id != 0) {
		isAddActionEntryVisible = true;
		isEditEntryVisible = true;
		auto isRootSelected = bool{TC_AnalysisAction.GetCursor() == 0};
		auto doesChildExist = bool{TC_AnalysisAction.GetChildCount(id) > 0};
		if(!doesChildExist && !isRootSelected) isRemoveEntryVisible = true;
	}

	bar.Add(t_("Add analysis"),THISBACK2(addRecord, ~E_PcbId, ItemType::analysis));
	bar.Add(isAddActionEntryVisible, t_("Link an action"), THISBACK2(addRecord, ~E_PcbId, ItemType::action));
	bar.Add(isEditEntryVisible, t_("Edit"), THISBACK(editRecord));
	bar.Add(isRemoveEntryVisible, t_("Remove"), THISBACK(removeRecord));
}

void PcbDlg::editRecord()
{
	const auto selected_index = TC_AnalysisAction.GetCursor();
	if(selected_index < 0) return;
	
	auto record = getRecordFromIndex(selected_index);
	if (record != nullptr) {
        ActionDlg dlg(*record);
        if(dlg.Execute() != IDOK) return;

        *record = dlg.Record();
        TC_AnalysisAction.SetValue(record->commentary);
	}
};

void PcbDlg::removeRecord()
{
	const auto selected_index = TC_AnalysisAction.GetCursor();
	if((selected_index < 0) || !PromptYesNo(t_("Delete entry ?")))
	   return;
	
	TC_AnalysisAction.Remove(selected_index);
	removeActionFromVector(selected_index);
}

void PcbDlg::addRecord(const int pcb_id, const ItemType type)
{
	// adds a new record to the treecontrol
	auto dlg = std::unique_ptr<ActionDlg>{};
	
	// correct contructor is picked regarding the type
	switch (type) {
		case ItemType::analysis: {
			dlg = std::make_unique<ActionDlg>(pcb_id, ItemType::analysis);
			break;
		}
		case ItemType::action: {
			const auto parent_index = int{TC_AnalysisAction.GetCursor()};
			if (parent_index != -1){
			    const auto parent_id = int{TC_AnalysisAction[parent_index]};
			    dlg = std::make_unique<ActionDlg>(pcb_id, ItemType::action, parent_id);
			}
			break;
		}
		default:
		    return;
	}
	
	if(dlg->Execute() != IDOK) return;
    
    auto new_record  = dlg->Record();
    const auto index = TC_AnalysisAction.Add((new_record.parent_id == 0) ? 0 : TC_AnalysisAction.GetCursor(),
                                        (new_record.parent_id == 0) ? MyImages::analysis() : MyImages::action(),
                                        new_record.id,
                                        new_record.commentary,
                                        false);
    //updateNodeIndexInMainVector(new_record, index);
    
    addActionToVector(new_record, index);
	
    TC_AnalysisAction.OpenDeep(0);
}

auto PcbDlg::getRecordNumber(const int pcb_id) -> int
{
	auto count = int{0};
	
	Sql sql;
	sql.Execute(Format("select count(*) from PCB_ACTION where PCB_ID =%i",pcb_id));
	if (sql.Fetch()) {
		count = sql[0];
	}
	
	return count;
}

void PcbDlg::removeActionFromVector(const int index)
{
    action_records_.erase(std::remove_if(action_records_.begin(), action_records_.end(), [&](const ActionRecord& ar) {
        return ar.node_index == index;
    }), action_records_.end());
}

void PcbDlg::addActionToVector(ActionRecord& ar, const int node_index)
{
    ar.id = -1; // new record, no id yet
    ar.node_index = node_index;
    action_records_.push_back(ar);
}

void PcbDlg::buildItemTree()
{
	// Fills the tree control with data from action file

	TC_AnalysisAction.Clear();
		
	// vector is full, it's time to add records to the treecontrol.
	// if PARENT_ID is 0, record is an analysis
	// otherwise record is an action, and PARENT_ID refers to the key of the parent in the treecontrol
	// Root of the tree control has index 0, first analysis has index 1
	auto actions    = vector<ActionRecord>{};
	auto analysis   = vector<ActionRecord>{};;
	auto orphans    = vector<ActionRecord>{};
	
	// looping through records to fill actions & analysis vectors
    for(const auto& ar : action_records_){
        if(ar.parent_id == 0) analysis.push_back(ar);
        else actions.push_back(ar);
    }
	
	TC_AnalysisAction.Clear(); // treecontrol is emptied
	TC_AnalysisAction.SetRoot(Null,0,t_("Analysis & Actions")); // setting up root
	TC_AnalysisAction.NoRoot(false);

    // First pass to create analysis and update linked node index in the vector.
    for(const auto& a : analysis) {
        const auto index = TC_AnalysisAction.Add(0,
                                                MyImages::analysis(),
                                                a.id,
                                                a.commentary,
                                                false);
        updateNodeIndexInMainVector(a, index);
    }

    for(const auto& a : actions) {
		const auto parent_id = int{a.parent_id}; // getting parent id of current record
        const auto& it = std::find_if(analysis.begin(), analysis.end(), [&](const ActionRecord& r) {
            return r.id == parent_id;
        });
		
        if (it == analysis.end()) {
            orphans.push_back(a);
        } else {
            const auto id = TC_AnalysisAction.Find(parent_id);
            if (id > 0){
                const auto index = TC_AnalysisAction.Add(id,
                                                          !a.finished ? MyImages::action() : MyImages::actionDone(),
                                                          a.id,
                                                          a.commentary,
                                                          false);
                updateNodeIndexInMainVector(a, index);
            } else {
                orphans.push_back(a);
            }
        }
    }

    if (!orphans.empty()) {
	    PromptOK(t_("Orphan actions exist for current pcb ! Use drag & drop to link them to an analysis"));

        // orphans actions are added to the treecontrol as analysis with a special icon
		for (const auto& o : orphans) {
            const auto index = TC_AnalysisAction.Add(0, 
                                                    MyImages::warning(),
                                                    o.id,
                                                    o.commentary,
                                                    false);
			updateNodeIndexInMainVector(o, index);
		}
	}

	TC_AnalysisAction.OpenDeep(0); // All tree nodes are opened

}

void PcbDlg::updateNodeIndexInMainVector(const ActionRecord& current_record, const int& new_index)
{
    auto record = getRecordFromId(current_record.id);
    if (record != nullptr) {
        record->node_index = new_index;
    }
}

void PcbDlg::createLinkedRecord(const TableType tableType)
{
	auto id = int{0};
	switch (tableType) {
		case TableType::pinout:
		{
			PinoutDlg dlg(OpeningType::opening_new);
			if(dlg.Execute() != IDOK)
				return;
			
			SQL * dlg.ctrls.Insert(PINOUT);
			id = SQL.GetInsertedId();
			
			// Droplist refresh
			loadDropList(tableType);
			DL_Pinout.SetIndex(DL_Pinout.FindKey(id));
			break;
		}
		case TableType::origin:
		{
			OriginDlg dlg(OpeningType::opening_new);
			if(dlg.Execute() != IDOK)
				return;
			
			SQL * dlg.ctrls.Insert(ORIGIN);
			id = SQL.GetInsertedId();
			
			// Droplist refresh
			loadDropList(tableType);
			DL_Origin.SetIndex(DL_Origin.FindKey(id));
			break;
		}
		case TableType::location:
		{
			LocationDlg dlg(OpeningType::opening_new);
			if(dlg.Execute() != IDOK)
				return;
			
			SQL * dlg.ctrls.Insert(LOCATION);
			id = SQL.GetInsertedId();
			
			// Droplist refresh
			loadDropList(tableType);
			DL_Location.SetIndex(DL_Location.FindKey(id));
			break;
		}

		case TableType::game:
		{
			int idBeforeInsert = SQL.GetInsertedId();
			GameDlg dlg;

			dlg.Run();

			id = SQL.GetInsertedId();
			
			// Droplist refresh
			if (idBeforeInsert != id) {
				loadDropList(tableType);
				DL_Game.SetIndex(DL_Game.FindKey(id));
			}
			break;
		}

		// Following cases aren't dynamically handled
		default: break;
			
			
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

void PcbDlg::loadDropList(const TableType table_type)
{
	Sql sql;
	switch (table_type) {
		case TableType::pinout:
			DL_Pinout.Clear();
			DL_Pinout.Add(0,t_("Not selected"));
			sql.Execute("select ID,LABEL,PIN_SIZE from PINOUT order by LABEL");
			while(sql.Fetch()) {
				auto temp = String{Format("%s (%s)",sql[LABEL].ToString(),sql[PIN_SIZE].ToString())};
				DL_Pinout.Add(
					sql[ID],
					temp
				);
			}
			DL_Pinout.SetIndex(0);
			DL_Pinout.NotNull(true);
			break;
		
		case TableType::origin:
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
		
		case TableType::location:
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
			
		case TableType::state:
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
			
		case TableType::type:
			sql.Execute("select ID,LABEL from PCB_TYPE");
			while(sql.Fetch()) {
				DL_Type.Add(
					sql[ID],
					sql[LABEL].ToString()
				);
			}
			break;
			
		case TableType::game:
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

void PcbDlg::setupDisplay(Ctrl* ctrl)
{
	if (ctrl->GetLayoutId() == signature_tab_.ES_SigOrigin.GetLayoutId()) {
		if (signature_tab_.ES_SigOrigin.GetData() == "") {
			signature_tab_.ES_SigOrigin <<= "Origin";
			signature_tab_.ES_SigOrigin.SetStyle(edit_style_);
		}
	}
	if (ctrl->GetLayoutId() == signature_tab_.ES_SigRange.GetLayoutId()) {
		if (signature_tab_.ES_SigRange.GetData() == "") {
			signature_tab_.ES_SigRange <<= "Range";
			signature_tab_.ES_SigRange.SetStyle(edit_style_);
		}
	}

	if (ctrl->GetLayoutId() == signature_tab_.ES_SigRomName.GetLayoutId()) {
		if (signature_tab_.ES_SigRomName.GetData() == "") {
			signature_tab_.ES_SigRomName <<= "Rom name";
			signature_tab_.ES_SigRomName.SetStyle(edit_style_);
		}
	}
	if (ctrl->GetLayoutId() == signature_tab_.ES_SigSection.GetLayoutId()) {
		if (signature_tab_.ES_SigSection.GetData() == "") {
			signature_tab_.ES_SigSection <<= "Section";
			signature_tab_.ES_SigSection.SetStyle(edit_style_);
		}
	}
	if (ctrl->GetLayoutId() == signature_tab_.ES_SigCrc32.GetLayoutId()) {
		if (signature_tab_.ES_SigCrc32.GetData() == "") {
			signature_tab_.ES_SigCrc32 <<= "CRC32";
			signature_tab_.ES_SigCrc32.SetStyle(edit_style_);
		}
	}
	if (ctrl->GetLayoutId() == signature_tab_.ES_SigSig.GetLayoutId()) {
		if (signature_tab_.ES_SigSig.GetData() == "") {
			signature_tab_.ES_SigSig <<= "Sig";
			signature_tab_.ES_SigSig.SetStyle(edit_style_);
		}
	}
}

void PcbDlg::resetDisplay(Ctrl* ctrl)
{
	if (ctrl->GetLayoutId() == signature_tab_.ES_SigOrigin.GetLayoutId()) {
		signature_tab_.ES_SigOrigin.SetStyle(EditString::StyleDefault());
		if (signature_tab_.ES_SigOrigin.GetData() == "Origin") {
			signature_tab_.ES_SigOrigin.Erase();
		}
	}
	if (ctrl->GetLayoutId() == signature_tab_.ES_SigRange.GetLayoutId()) {
		signature_tab_.ES_SigRange.SetStyle(EditString::StyleDefault());
		if (signature_tab_.ES_SigRange.GetData() == "Range") {
			signature_tab_.ES_SigRange.Erase();
		}
	}

	if (ctrl->GetLayoutId() == signature_tab_.ES_SigRomName.GetLayoutId()) {
		signature_tab_.ES_SigRomName.SetStyle(EditString::StyleDefault());
		if (signature_tab_.ES_SigRomName.GetData() == "Rom name") {
			signature_tab_.ES_SigRomName.Erase();
		}
	}
	if (ctrl->GetLayoutId() == signature_tab_.ES_SigSection.GetLayoutId()) {
		signature_tab_.ES_SigSection.SetStyle(EditString::StyleDefault());
		if (signature_tab_.ES_SigSection.GetData() == "Section") {
			signature_tab_.ES_SigSection.Erase();
		}
	}
	if (ctrl->GetLayoutId() == signature_tab_.ES_SigCrc32.GetLayoutId()) {
		signature_tab_.ES_SigCrc32.SetStyle(EditString::StyleDefault());
		if (signature_tab_.ES_SigCrc32.GetData() == "CRC32") {
			signature_tab_.ES_SigCrc32.Erase();
		}
	}
	if (ctrl->GetLayoutId() == signature_tab_.ES_SigSig.GetLayoutId()) {
		signature_tab_.ES_SigSig.SetStyle(EditString::StyleDefault());
		if (signature_tab_.ES_SigSig.GetData() == "Sig") {
			signature_tab_.ES_SigSig.Erase();
		}
	}
}

void PcbDlg::tabChanged()
{
	constexpr int8 pictures_tab{1};
	constexpr int8 signatures_tab{2};
	auto i = int{~TC_Tab};
	switch (i) {
		case pictures_tab: {
			auto child = pictures_tab_.GetFirstChild();
			while (child) {
				resetDisplay(child);
				setupDisplay(child);
				child = child->GetNext();
			}
			
			populatePicturesArray();
			break;
		}
		case signatures_tab: {
			auto child = signature_tab_.GetFirstChild();
			while (child) {
				resetDisplay(child);
				setupDisplay(child);
				child = child->GetNext();
			}
			
			populateSignatureArray();
			break;
		}
	}
}

void PcbDlg::displayPicture()
{
    ViewerDlg(pictures_tab_.pictures.GetKey()).RunAppModal();
}

void PcbDlg::displayPicturePreview()
{
	preview_.SetImage(pictures_tab_.pictures.GetKey());
	preview_.Refresh();
}

void PcbDlg::updatePictureLabel()
{
    constexpr int label_id{1};
    const int row_id = pictures_tab_.pictures.GetCursor();
    if (row_id == -1) return;
    auto label = pictures_tab_.pictures.Get(row_id, label_id);

    const auto key = pictures_tab_.pictures.GetKey();
    SQL * ::Update(PICTURE)(LABEL, label).Where(ID == key);
    
    populatePicturesArray();
}

void PcbDlg::savePictureToDatabase(const int pcb_id, const String& label, const Image& img)
{
	auto cfg            = LoadIniFile("agar.cfg");
	auto resize_picture = static_cast<ResizePicture>(StrInt(cfg.Get("NoResize", Null)));
	auto width          = StrInt(cfg.Get("ImageWidth", Null));
	auto height         = StrInt(cfg.Get("ImageHeight", Null));
	
	//Size img_size(img.GetSize());
	auto img_size = Size{img.GetSize()};
	if (resize_picture == ResizePicture::do_resize){
		if ((img.GetWidth() > width) || (img.GetHeight() > height)) {
			// picture needs to be resized
			img_size = Size(width, height);
		}
	}
	
	const auto img_to_save = Rescale(img, GetFitSize(img.GetSize(), img_size));
	
	const auto preview_size = Size{preview_width, preview_height};
	const auto preview_to_save = Rescale(img, GetFitSize(img.GetSize(), preview_size));

	JPGEncoder jpg;
	SQL * Insert(PICTURE)
	(LABEL, label)
	(DATA, SqlBinary(jpg.SaveString(img_to_save)))
	(PREVIEW_DATA, SqlBinary(jpg.SaveString(preview_to_save)))
	(PCB_ID, pcb_id);
	
	if(SQL.WasError()){
	    PromptOK(SQL.GetErrorCodeString());
	}

}

void PcbDlg::populatePicturesArray()
{
	// Fills pictures array with data from database
	
	pictures_tab_.pictures.Clear();
    SQL * Select(ID,LABEL).From(PICTURE).Where(PCB_ID == ~E_PcbId).OrderBy(LABEL);
    auto i = uint32{0};
    while (SQL.Fetch()) {
        pictures_tab_.pictures.Add(SQL[ID],SQL[LABEL]);
        pictures_tab_.pictures.CreateCtrl<Button>(i, 1).SetImage(MyImages::edit) <<= THISBACK2(editPictureLabel, &pictures_tab_.pictures, i);
        pictures_tab_.pictures.CreateCtrl<Button>(i, 2).SetImage(MyImages::remove)<<= THISBACK2(removePicture, &pictures_tab_.pictures, i);
        pictures_tab_.pictures.CreateCtrl<Button>(i, 3).SetImage(MyImages::save)<<= THISBACK2(savePicture, &pictures_tab_.pictures, i);
        ++i;
    }
}

void PcbDlg::editPictureLabel(ArrayCtrl* a, const int id)
{
    a->SetCursor(id);
    a->DoEdit();
}

void PcbDlg::removePicture(ArrayCtrl* a, const int id)
{
	if(PromptOKCancel(t_("The picture will be permanently deleted. Continue ?")) == 1){
        a->SetCursor(id);
        SQL * Delete(PICTURE).Where(ID == a->GetKey());
        populatePicturesArray();
	}
}

void PcbDlg::savePicture(ArrayCtrl* a, const int id){
    
    const String fileName = SelectFileSaveAs("*.jpg");
    if (!fileName.IsVoid()){
        a->SetCursor(id);
        SQL * Select(DATA).From(PICTURE).Where(ID == a->GetKey());
        if (SQL.Fetch()) {
            JPGRaster jpgr;
            auto img = jpgr.LoadString(SQL[DATA]);
            JPGEncoder jpg;
            jpg.SaveFile(fileName, img);
        }
    }
}

void PcbDlg::dragAndDrop(PasteClip& d, ArrayCtrl& a)
{
   if(AcceptFiles(d)) {
       auto files = GetFiles(d);
       if(files.GetCount()){
           Progress pi("Loading files");
           pi.SetTotal(files.GetCount());
           for(int i = 0; i < files.GetCount(); i++){
               auto img = StreamRaster::LoadFileAny(files[i]);
               if(!img.IsNullInstance()){
					savePictureToDatabase(~E_PcbId, GetFileName(files[i]), img);
               }
               ProcessEvents();
               pi.Step();
               
           }
           populatePicturesArray();
       }
       Refresh();
   }
}

void PcbDlg::generateSignatureTabMenu(Bar& bar)
{
	bar.Add(t_("Remove"),THISBACK(removeSignatureRecord));
}

void PcbDlg::removeSignatureRecord()
{
	// Fluke record removal from database
	SQL * Delete(FLUKE).Where(ID == signature_tab_.signatures.GetKey());
	
	// Table is reloaded
	populateSignatureArray();
}

void PcbDlg::addSignatureRecord()
{

	auto insert_record = bool{true};
	if ( (~signature_tab_.ES_SigRomName == "Rom name") ||
		(~signature_tab_.ES_SigSection == "Section") ||
		(~signature_tab_.ES_SigRange == "Range") ||
		(~signature_tab_.ES_SigOrigin == "Origin") ||
		(~signature_tab_.ES_SigCrc32 == "CRC32") ||
		(~signature_tab_.ES_SigSig == "Sig") ) {
		
		if (!PromptYesNo(t_("At least one of the fields isn't filled. Do you want to add the record anyway ?")) ) {
			insert_record = false;
		}
	}
	
	if (insert_record) {
		SQL * Insert(FLUKE)(ROM_NAME, ~signature_tab_.ES_SigRomName)
				(SECTION, ~signature_tab_.ES_SigSection)
				(CRC_32, ~signature_tab_.ES_SigCrc32)
				(FLUKE_SIG, ~signature_tab_.ES_SigSig)
				(PCB_ID, ~E_PcbId)
				(RANGE, ~signature_tab_.ES_SigRange)
				(ORIGIN, ~signature_tab_.ES_SigOrigin);
	
		populateSignatureArray();
	}
}

void PcbDlg::populateSignatureArray() {
	// Fills signature array with data from database
	
	signature_tab_.signatures.Clear();
    SQL * Select(ID,ROM_NAME,SECTION,CRC_32,FLUKE_SIG,RANGE,ORIGIN).From(FLUKE).Where(PCB_ID == ~E_PcbId).OrderBy(ROM_NAME);
    while (SQL.Fetch()) {
        signature_tab_.signatures.Add(SQL[ID],SQL[ROM_NAME],SQL[SECTION],SQL[RANGE],SQL[ORIGIN],SQL[CRC_32],SQL[FLUKE_SIG]);
    }
}

void PcbDlg::loadActionTreeFromDatabase()
{
	// query is done to fill action vector with current PCB_ACTION data
	Sql sql;
	sql.Execute(Format("SELECT * FROM PCB_ACTION WHERE PCB_ID = %i ORDER BY PARENT_ID,ACTION_DATE", pcb_id_));
	while(sql.Fetch())
	{
		//++key; // key is incremented
		
		ActionRecord ar;
        ar.id           = sql[ID];
        ar.pcb_id       = sql[PCB_ID];
        ar.parent_id    = sql[PARENT_ID];
        //ar.node_index	= key;
        //ar.key          = key;
        ar.date         = sql[ACTION_DATE];
        ar.commentary   = sql[COMMENTARY];
        ar.finished     = static_cast<bool>(StrInt(AsString(sql[FINISHED]))); // defined as text in the database
        ar.type         = ItemType(static_cast<int>(sql[ACTION_TYPE]));

		action_records_.push_back(ar);
	}
	
	buildItemTree(); // Treecontrol is reloaded
}

void PcbDlg::saveActionTreeToDatabase()
{
    // Current records for the pcb are deleted from the database prior adding the new ones
    Sql sql;
    sql.Execute(Format("DELETE FROM PCB_ACTION WHERE PCB_ID = %i",pcbId()));

	auto actions    = vector<ActionRecord>{};
	auto analysis   = vector<ActionRecord>{};;
	auto orphans    = vector<ActionRecord>{};
	
	// looping through records to fill actions & analysis vectors
    for(const auto& ar : action_records_){
        if(ar.parent_id == 0) analysis.push_back(ar);
        else actions.push_back(ar);
    }

    // Creating analysis
    for (const auto& an : analysis) {
	       sql * Insert(PCB_ACTION)
                    (PCB_ID,        an.pcb_id)
                    (PARENT_ID,     an.parent_id)
                    (ACTION_DATE,   Time(an.date))
                    (COMMENTARY,    an.commentary)
                    (FINISHED,      an.finished)
                    (ACTION_TYPE,   toUnderlying(an.type));
                    
            auto inserted_id = int{sql.GetInsertedId()};
            
            // Updating the original record with the new database id
            auto record = getRecordFromIndex(an.node_index);
            if (record != nullptr) {
                record->id = inserted_id;
            }
    }
    
    // Creating actions
    for (const auto& ac : actions) {
        const auto parent_index = TC_AnalysisAction.GetParent(ac.node_index);
        auto parent_id = int{-1};
        auto record = getRecordFromIndex(parent_index);
        if (record != nullptr) {
            parent_id = record->id;
        }
        
        sql * Insert(PCB_ACTION)
                (PCB_ID,        ac.pcb_id)
                (PARENT_ID,     parent_id)
                (ACTION_DATE,   Time(ac.date))
                (COMMENTARY,    ac.commentary)
                (FINISHED,      ac.finished)
                (ACTION_TYPE,   toUnderlying(ac.type));
    }
}

void PcbDlg::treeDrag() {
    const auto selected_index   = TC_AnalysisAction.GetCursor();
    if (selected_index == 0) return; // can't drag root

    auto record = getRecordFromIndex(selected_index);
    if (record != nullptr) {
        if (record->type == ItemType::analysis) return;
        record->node_index = -1;
    }

    // Drag & drop is initiated
    if (TC_AnalysisAction.DoDragAndDrop(InternalClip(TC_AnalysisAction, "mytreedrag"), TC_AnalysisAction.GetDragSample()) == DND_MOVE) {
        TC_AnalysisAction.RemoveSelection();
    }
}

void PcbDlg::treeDropInsert(const int parent, const int ii, PasteClip& d)
{
    // Check type of drag data, and restrict to analysis level
    if (IsAvailableInternal<TreeCtrl>(d, "mytreedrag") && treeGetLevel(parent) == 1) {
        
        if (parent == TC_AnalysisAction.GetCursor()){ // preventing drop to self
            d.Reject();
            return;
        }
        
        // Data can be accepted
        d.Accept();
        // If we haven't dropped the data yet (we are still dragging) don't do anything
        if (!d.IsPaste()) return;

        // User has dropped it! Do the insert
        auto inserted_indexes = TC_AnalysisAction.InsertDrop(parent, ii, d);
        
        // Updating the parent_id of the dropped action
        if (!inserted_indexes.empty()) {
            // only one action can be selected at a time
            const auto before_drop_index = int{-1};
            auto record = getRecordFromIndex(before_drop_index);
            if (record != nullptr){
                const auto parent_id = TC_AnalysisAction.Get(parent);
                record->parent_id = parent_id;
                record->node_index = inserted_indexes[0];
            }
        }
        return;
    }
}

auto PcbDlg::treeGetLevel(int id) const -> int
{
    int i=0;
    while (id != 0){
        id = TC_AnalysisAction.GetParent(id);
        ++i;
    }
    return i;
}

void PcbDlg::logActionVector()
{
    LOG("actionRecord_ content : ");
    for (vector<ActionRecord>::iterator it = action_records_.begin(); it != action_records_.end(); ++it)
    {
	    LOG(Format("ID=%i,PARENT_ID=%i, NODE_INDEX=%i",it->id, it->parent_id, it->node_index));
    }
}

void PcbDlg::doOk()
{
    saveActionTreeToDatabase();
    
    Break(IDOK);
}

auto PcbDlg::getRecordFromIndex(const int index) -> ActionRecord*
{
    auto it = std::find_if(action_records_.begin(), action_records_.end(), [&](const ActionRecord& ar) {
        return ar.node_index == index;
    });
    
    if (it != action_records_.end()){
        return &(*it);
    }
    return nullptr;
}

auto PcbDlg::getRecordFromId(const int id) -> ActionRecord*
{
    auto it = std::find_if(action_records_.begin(), action_records_.end(), [&](const ActionRecord& ar) {
        return ar.id == id;
    });
    
    if (it != action_records_.end()){
        return &(*it);
    }
    return nullptr;
}

Popup::Popup(const int& id) {

	SQL * Select(DATA).From(PICTURE).Where(ID == id);
    if (SQL.Fetch()) {
        JPGRaster jpgr;
        auto img = jpgr.LoadString(SQL[DATA]);
        auto sz = jpgr.GetSize();
        
	    AddFrame( menu );
	    AddFrame( TopSeparatorFrame() );
	    AddFrame( status );
	    AddFrame( InsetFrame() );

		raster_.Open("Y:\\repair_logs\\cyvern.jpg");

		// adds raster control
		Add(raster_.HSizePos().VSizePos());
	    Sizeable().Zoomable();
		BackPaint();
		Refresh();
    }
}