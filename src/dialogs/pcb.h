////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	pcb.h
///
/// \brief	Declares the pcb dialog class. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// SYSTEM INCLUDES
#include <plugin/jpg/jpg.h> // JPGRaster
#include <RasterCtrl/RasterCtrl.h>
#include <optional>
#include <vector> // std::vector


// PROJECT INCLUDES
#include "../agar.h"
#include "../enums.h"

constexpr unsigned int preview_height = 200;
constexpr unsigned int preview_width = 250;

// Defines a record from PCB_ACTION table.
struct ActionRecord {
	int         id; //< PCB_ACTION id
	int         pcb_id; //< PCB id
    int         node_index; //< Treecontrol node index
	int         parent_id; //< Treecontrol parent id
	Time        date; //< PCB_ACTION date
	String      commentary;	//< PCB_ACTION commentary
	int			finished; //< PCB_ACTION finished
	ItemType	type; //< PCB_ACTION type
};

class PreviewCtrl : public StaticText {
	
	typedef PreviewCtrl CLASSNAME;
	
	public:
		// Constructor.
		PreviewCtrl() {};
		
		// Initializes internal widget image from database.
		void SetImage(const int id);
        
        // Returns the address of the internal image.
        auto GetImage() -> Image& { return img_; }

		// Widget paint function.
		virtual void Paint(Draw& w);

	private:
        Image   img_;           // Internal fullsize image.
        Image	preview_img_;   // Internal preview image.
};

class PcbDlg : public WithPcbLayout<TopWindow> {

	typedef PcbDlg CLASSNAME;
	
	public:
		PcbDlg(const OpeningType type, const int pcbId=0);
		~PcbDlg();

		// fault data functions
		void generateFaultData();
		void loadFaultData();
		
		// Menu related functions
		void createAnalysisAndActionsMenu(Bar& bar);
		
		// analysis & actions related functions
		
        // Adds a record to the treecontrol, depending on the ItemType parameter.
		void addRecord(const int pcb_id, const ItemType type);
		
        // Edits currently selected record in the treecontrol
		void editRecord();

        // Removes the selected record from the TreeControl
		void removeRecord();
		
		// returns number of record from action table for the pcb id in parameter
		auto getRecordNumber(const int pcb_id) -> int;
		
		enum class TableType {
			game,
			type,
			state,
			pinout,
			location,
			origin
		};
		
		// droplists related functions
		void createLinkedRecord(const TableType tableType);
		void loadDropList(const TableType tableType);
		
		// returns the option value for the id selected
		static auto getFaultValue(const int i, const String& faults) -> bool;

		// Callback called when OK button is pressed.
		void doOk();

        SqlCtrls ctrls; // Controls of the dialog.

        // Various tabs of the dialog.
        WithTabPicturesLayout<ParentCtrl>  pictures_tab_;
        WithTabMiscLayout<ParentCtrl>      misc_tab_;
        WithTabSignatureLayout<ParentCtrl> signature_tab_;
		
        PreviewCtrl preview_; // Widget for image preview.

	private:
		// Resets up signatures tab display.
		void resetDisplay(Ctrl* ctrl);
		
		// Sets up signatures tab display.
		void setupDisplay(Ctrl* ctrl);
		
		// Called when a different tab is selected.
		void tabChanged();
		
		// Edits the picture label.
		void editPictureLabel(ArrayCtrl* a, const int id);
		
		// Removes picture from database.
		void removePicture(ArrayCtrl* a, const int id);
		
		// Save pictures to database.
		void savePictureToDatabase(const int id, const String& label, const Image& img);
		
        // Loads pictures from database.
		void populatePicturesArray();
		
		// Opens a window to display the picture fullsize.
		void displayPicture();
		
		// Display a preview of the currently selected picture.
		void displayPicturePreview();
		
		// Updates the picture label.
		void updatePictureLabel();
		
		// Loads pictures from the file explorer using drag & drop.
		void dragAndDrop(PasteClip& d, ArrayCtrl& a);
	
		// Generates the mouse menu of the signature tab.
		void generateSignatureTabMenu(Bar& bar);
		
		// Adds signature to database.
		void addSignatureRecord();
		
		// Remove selected signature from database.
		void removeSignatureRecord();
		
		// Loads signatures from database.
		void populateSignatureArray();
		
		// Loads treecontrol content from database.
		void loadActionTreeFromDatabase();
		
		// Saves treecontrol content to database.
		void saveActionTreeToDatabase();
		
        // Function called when drag is initialized in the treecontrol.
		void treeDrag();
		
        // Function called when dragged item in the treecontrol is dropped.
		void treeDropInsert(const int new_parent_index, const int index_to_parent, PasteClip& d);
		
        // Returns level in the tree hierarchy of the given parameter. (0=root, 1=analysis,
        // 2=action).
		auto treeGetLevel(int id) const -> int;
		
		// Removes the item with parameter as id from the vector.
		void removeActionFromVector(const int id);

		// Builds the treecontrol using data from the main vector.
		void buildItemTree();

		// Adds an item to the main vector
		void addActionToVector(ActionRecord& ar, const int node_index);
		
		// Logs the main vector content.
		void logActionVector();
		
		// Updates the treecontrol id of an item in the main vector
		void updateNodeIndexInMainVector(const ActionRecord& current_record, const int& new_index);
		
		// Returns a pointer to the record corresponding to the treecontrol index in the main
		// vector, nullptr otherwise.
		auto getRecordFromIndex(const int index) -> ActionRecord*;
		
		// Returns a pointer to the record corresponding to the database id in the main vector,
		// nullptr otherwise
		auto getRecordFromId(const int id) -> ActionRecord*;
		
		/// ACCESSORS
		void pcbId(const int id) {pcb_id_ = id;}
		auto pcbId() const { return pcb_id_;}
		
		// Options management.
		ArrayMap<int, Option> option_;
		ArrayMap<int, Option> option_origin_;
	
		// Controls for direct edition.
		EditString::Style edit_style_;
		EditString picture_label_;

        std::vector<ActionRecord> action_records_; // Main vector, linked to the treecontrol.
		int pcb_id_; // Id of the current PCB.
};

class Popup : public TopWindow {
	
	typedef Popup CLASSNAME;
	
	public:
        Popup(const int& id);

	private:
		StatusBar   status;
		MenuBar     menu;
		RasterCtrl  raster_;
	
};