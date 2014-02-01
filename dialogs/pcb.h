////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	pcb.h
///
/// \brief	Declares the pcb dialog class. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef _PCB_H
#define _PCB_H

// SYSTEM INCLUDES
#include <plugin/jpg/jpg.h> // JPGRaster
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
	int 	id;
	int     pcbId;
	int 	parentId;
	Time 	date;
	String 	commentary;	
	int		finished;
	int		type;
};

class PreviewCtrl : public StaticText {
	
	typedef PreviewCtrl CLASSNAME;
	
	private:
		Image 	img_;
		int		previewHeight_;
		int		previewWidth_;
	
	public:
		virtual void Paint(Draw& draw);
	
		PreviewCtrl() {
			previewHeight_ = 200;
			previewWidth_ = 250;
		};
		
		void SetImage(const int& id) {
			
		    SQL * Select(DATA).From(PICTURE).Where(ID == id);
		    if (SQL.Fetch()) {
		        //PNGRaster pngr;
		        //img_ = pngr.LoadString(SQL[DATA]);
		        img_ = JPGRaster().LoadString(SQL[DATA]);
		    }
    	}
};

class PcbDlg : public WithPcbLayout<TopWindow> {

	typedef PcbDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;

		WithTabPicturesLayout<ParentCtrl> TabPictures;
		WithTabMiscLayout<ParentCtrl> TabMisc;
		WithTabSignatureLayout<ParentCtrl> TabSignature;
		
		PreviewCtrl			preview_;
		
		// fault data functions
		void GenerateFaultData();
		void LoadFaultData();
		
		// Menu related functions
		void TreeControlMenu(Bar& bar);
		bool DisplayAddActionMenuEntry();
		bool DisplayEditMenuEntry();
		bool DisplayRemoveMenuEntry();
		void SetAddActionMenuEntryVisible(const bool val);
		void SetEditMenuEntryVisible(const bool val);
		void SetRemoveMenuEntryVisible(const bool& val);
		
		// analysis & actions related functions
		void AddAnalysis(const int pcbId);
		void AddAction(const int pcbId);
		void Edit();
		void Remove();
		
		
		int	GetRecordNumber(const int pcbId);
		
		enum TableType {
			TABLE_GAME,
			TABLE_TYPE,
			TABLE_STATE,
			TABLE_PINOUT,
			TABLE_LOCATION,
			TABLE_ORIGIN
		};
		
		// droplists related functions
		void CreateLinkedRecord(const int tableType);
		void LoadDropList(const int tableType);
		
		virtual void ChildGotFocus() {
			//Ctrl *c = GetFocusChild();
			currentCtrl_ = GetFocusChildDeep();
			if (!currentCtrl_ || !currentCtrl_->IsEditable()) return;
			 
			ResetDisplay(currentCtrl_);

		}
		
		virtual void ChildLostFocus() {
			if (!currentCtrl_) return;

			SetupDisplay(currentCtrl_);
			currentCtrl_ = 0;
		}

		static bool GetFaultValue(const int i, const String& faults); // returns the option value for the id selected
		
		PcbDlg(const int openingType, const int pcbId=0);	
		~PcbDlg();
	
	private:
		Array<Label> 				label;
		ArrayMap<int, Option>  		option;
		Array<Label> 				labelOrigin_;
		ArrayMap<int, Option>  		optionOrigin_;
	
		bool 						addActionMenuEntryVisible_;
		bool 						editMenuEntryVisible_;
		bool 						removeMenuEntryVisible_;
		
		EditString::Style 			editStyle_;
		Ctrl*						currentCtrl_;

		std::vector<ActionRecord> 	actionRecords_;
		int 						pcbId_;
		
		void ResetDisplay(Ctrl* ctrl);
		void SetupDisplay(Ctrl* ctrl);
		void TabChanged();
		
		int	pictureWidth_; // max width of saved pictures in the db
		int pictureHeight_; // max heigth of saved picture in the database
		void PictureTabMenu(Bar& bar);
		void RemovePicture();
		void SelectImage();
		void AddImageToDatabase();
		void PopulatePicturesArray();
		void DisplayPicture();
		void DisplayPicturePreview();
		
		void SignatureTabMenu(Bar& bar);
		void AddSignatureRecord();
		void RemoveSignatureRecord();
		void PopulateSignatureArray();
		
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void LoadActionTreeFromDatabase()
		///
		/// \brief	Loads action/analysis treecontrol for current pcb. 
		///
		/// \author	Runik
		/// \date	28/01/2014
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void LoadActionTreeFromDatabase();
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void SaveActionTreeToDatabase() const
		///
		/// \brief	Save action/analysis treecontrol for current pcb to the database. 
		///
		/// \author	Runik
		/// \date	29/01/2014
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void SaveActionTreeToDatabase() const;
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void TreeDrag()
		///
		/// \brief	Function called when drag is initialized in the treecontrol. 
		///
		/// \author	Runik
		/// \date	29/01/2014
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void TreeDrag();
		
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
		void TreeDropInsert(const int parent, const int ii, PasteClip& d);
		
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
		int TreeGetLevel(int id) const;
		
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
		void RemoveActionFromVector(const int id);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	ActionRecord& GetActionFromVector(const int id) const
		///
		/// \brief	Returns a reference to record from vector corresponding to the id.
		///
		/// \author	Runik
		/// \date	30/01/2014
		///
		/// \param  id   id of record to be returned
		///
		/// \return ActionRecord reference to record corresponding to the id
		////////////////////////////////////////////////////////////////////////////////////////////////////		
		ActionRecord& GetActionFromVector(const int id);
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void BuildActionTree()
		///
		/// \brief	Builds the treecontrol using data from the actionRecords_ vector.
		///
		/// \author	Runik
		/// \date	30/01/2014
		///
		////////////////////////////////////////////////////////////////////////////////////////////////////		
		void BuildActionTree();

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void AddActionToVector(const ActionRecord& ar)
		///
		/// \brief	Add record to vector.
		///
		/// \author	Runik
		/// \date	31/01/2014
		///
		/// \param  ar   record to add
		////////////////////////////////////////////////////////////////////////////////////////////////////		
		void AddActionToVector(const ActionRecord& ar);
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void DoOk()
		///
		/// \brief	Callback called when OK button is pressed.
		///
		/// \author	Runik
		/// \date	01/02/2014
		////////////////////////////////////////////////////////////////////////////////////////////////////		
		void DoOk();

		/// ACCESSORS
		void    PcbId(const int id) {pcbId_ = id;}
		int     PcbId() const { return pcbId_;}
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

#endif _PCB_H