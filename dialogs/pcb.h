////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	pcb.h
///
/// \brief	Declares the pcb dialog class. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// SYSTEM INCLUDES
#include <plugin/jpg/jpg.h> // JPGRaster
#include <optional>
#include <vector> // std::vector


// PROJECT INCLUDES
#include "agar/agar.h" 

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	ActionRecord
///
/// \brief	Defines a record from PCB_ACTION table.
///
/// \author	Runik
/// \date	28/01/2014
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ActionRecord 
{
	int 	id; //< PCB_ACTION id
	int     pcb_id; //< PCB id
    int     node_index; //< Treecontrol node index
	int 	parent_index; //< Treecontrol parent index
	//int     parentKey; //< PCB_ACTION id of the parent
	int		key; //< internal key of the record, will be added to the key of the treecontrol record
	Time 	date; //< PCB_ACTION date
	String 	commentary;	//< PCB_ACTION commentary
	int		finished; //< PCB_ACTION finished
	int		type; //< PCB_ACTION type
};

class PreviewCtrl : public StaticText {
	
	typedef PreviewCtrl CLASSNAME;
	
	private:
        Image   img_;
		int     preview_height_;
		int     preview_width_;
	
	public:
		virtual void Paint(Draw& draw);
	
		Vector<String> files;
		
		PreviewCtrl() {
			preview_height_ = 200;
			preview_width_ = 250;
		};
		
		void SetImage(const int& id) {
			
		    SQL * Select(DATA).From(PICTURE).Where(ID == id);
		    if (SQL.Fetch()) {
                Size sz;
                sz.cx = preview_width_;
                sz.cy = preview_height_;
                auto img = JPGRaster().LoadString(SQL[DATA]);
                img_ = Rescale(img, GetFitSize(img.GetSize(),sz));
		    }
    	}
};

class PcbDlg : public WithPcbLayout<TopWindow> {

	typedef PcbDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;

		WithTabPicturesLayout<ParentCtrl>  pictures_tab_;
		WithTabMiscLayout<ParentCtrl>      misc_tab_;
		WithTabSignatureLayout<ParentCtrl> signature_tab_;
		
		PreviewCtrl preview_;
		
		// fault data functions
		void generateFaultData();
		void loadFaultData();
		
		// Menu related functions
		void createAnalysisAndActionsMenu(Bar& bar);
		
		// analysis & actions related functions
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void addRecord(const int pcbId, const int type)
		///
		/// \brief	Adds a record to the treecontrol, depending on the type parameter. 
		///
		/// \author	Runik
		/// \date	14/07/2014
		///
		/// \param  pcbId	 PCB id
		/// \param  type     ANALYSIS or ACTION (enum defined in action.h)
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void addRecord(const int pcb_id, const int type);
		void editRecord();
		void removeRecord();
		
		
		int	getRecordNumber(const int pcb_id);
		
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
		
		static bool getFaultValue(const int i, const String& faults); // returns the option value for the id selected

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void DoOk()
		///
		/// \brief	Callback called when OK button is pressed.
		///
		/// \author	Runik
		/// \date	01/02/2014
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void doOk();

		PcbDlg(const int openingType, const int pcbId=0);
		~PcbDlg();
	

	private:
		void resetDisplay(Ctrl* ctrl);
		void setupDisplay(Ctrl* ctrl);
		void tabChanged();
		
		void editPictureLabel(ArrayCtrl* a, const int id);
		void removePicture(ArrayCtrl* a, const int id);
		void savePictureToDatabase(const int id, const String& label, const Image& img);
		void populatePicturesArray();
		void displayPicture();
		void displayPicturePreview();
		void updatePictureLabel();
		void dragAndDrop(PasteClip& d, ArrayCtrl& a);
	
		void signatureTabMenu(Bar& bar);
		void addSignatureRecord();
		void removeSignatureRecord();
		void populateSignatureArray();
		

		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void LoadActionTreeFromDatabase()
		///
		/// \brief	Loads action/analysis treecontrol for current pcb. 
		///
		/// \author	Runik
		/// \date	28/01/2014
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void loadActionTreeFromDatabase();
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void SaveActionTreeToDatabase()
		///
		/// \brief	Save action/analysis treecontrol for current pcb to the database. 
		///
		/// \author	Runik
		/// \date	29/01/2014
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void saveActionTreeToDatabase();
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void TreeDrag()
		///
		/// \brief	Function called when drag is initialized in the treecontrol. 
		///
		/// \author	Runik
		/// \date	29/01/2014
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void treeDrag();
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void TreeDropInsert()
		///
		/// \brief	Function called when dragged item in the treecontrol is dropped. 
		///
		/// \author	Runik
		/// \date	29/01/2014
		///
		/// \param  parent   Parent id
		/// \param  ii       Record id
		/// \param  d        PasteClip value
		////////////////////////////////////////////////////////////////////////////////////////////////////		
		void treeDropInsert(const int parent, const int ii, PasteClip& d);
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	int TreeGetLevel(int i) const
		///
		/// \brief	Returns level in the tree hierarchy of the given parameter.
		///
		/// \author	Runik
		/// \date	30/01/2014
		///
		/// \param  id   tree index to get level from
		///
		/// \return level in the tree hierarchy (0=root, 1=analysis, 2=action)
		////////////////////////////////////////////////////////////////////////////////////////////////////		
		int treeGetLevel(int id) const;
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void RemoveActionFromVector(const int id)
		///
		/// \brief	Removes record from vector corresponding to the id.
		///
		/// \author	Runik
		/// \date	30/01/2014
		///
		/// \param  id   id of record to be removed
		////////////////////////////////////////////////////////////////////////////////////////////////////		
		void removeActionFromVector(const int id);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void BuildActionTree()
		///
		/// \brief	Builds the treecontrol using data from the actionRecords_ vector.
		///
		/// \author	Runik
		/// \date	30/01/2014
		///
		////////////////////////////////////////////////////////////////////////////////////////////////////		
		void buildActionTree();

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void AddActionToVector(ActionRecord ar)
		///
		/// \brief	Add record to vector.
		///
		/// \author	Runik
		/// \date	31/01/2014
		///
		/// \param  ar   record to add
		////////////////////////////////////////////////////////////////////////////////////////////////////		
		void addActionToVector(ActionRecord ar);
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void LogActionVector()
		///
		/// \brief	Logs the vector content.
		///
		/// \author	Runik
		/// \date	06/02/2014
		///
		////////////////////////////////////////////////////////////////////////////////////////////////////		
		void logActionVector();
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void SortActionVector()
		///
		/// \brief	Sorts the vector using nodeIndex member
		///
		/// \author	Runik
		/// \date	05/02/2014
		///
		////////////////////////////////////////////////////////////////////////////////////////////////////		
		void sortActionVector();
		
		/// ACCESSORS
		void pcbId(const int id) {pcb_id_ = id;}
		auto pcbId() const { return pcb_id_;}
		void actionRecordsKey(const int k) {action_records_key_ = k;}
		auto actionRecordsKey() const { return action_records_key_;}
		
		ArrayMap<int, Option> option_;
		ArrayMap<int, Option> option_origin_;
	
		EditString::Style edit_style_;
		EditString picture_label_;

		std::vector<ActionRecord> 	action_records_;
		int 						pcb_id_;
		int							action_records_key_; //< unique key of action records. Incremented as records are added
};

class Popup : public TopWindow {
	
	typedef Popup CLASSNAME;
	
	private:
		//Image img_;
		
	public:
	    Image img_;
	    virtual void Paint(Draw& draw);

    	Popup(const int& id) {
			
		    SQL * Select(DATA).From(PICTURE).Where(ID == id);
		    if (SQL.Fetch()) {
		        //PNGRaster pngr;
		        //img_ = pngr.LoadString(SQL[DATA]);
		        JPGRaster jpgr;
		        img_ = jpgr.LoadString(SQL[DATA]);
		    }
    	}
};