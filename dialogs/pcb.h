#include "agar/agar.h"
#include <plugin/jpg/jpg.h>

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
		WithTabFlukeLayout<ParentCtrl> TabFluke;
		
		PreviewCtrl			preview_;
		
		// fault data functions
		void GenerateFaultData();
		void LoadFaultData();
		
		// Menu related functions
		void TreeControlMenu(Bar& bar);
		bool DisplayAddActionMenuEntry();
		bool DisplayEditMenuEntry();
		bool DisplayRemoveMenuEntry();
		void SetAddActionMenuEntryVisible(const bool& val);
		void SetEditMenuEntryVisible(const bool& val);
		void SetRemoveMenuEntryVisible(const bool& val);
		
		// analysis & actions related functions
		void AddAnalysis(const int& pcbId);
		void AddAction(const int& pcbId);
		void Edit();
		void Remove();
		
		void BuildActionTree(const int& pcbId);
		
		int	GetRecordNumber(const int& pcbId);
		
		enum TableType {
			TABLE_GAME,
			TABLE_TYPE,
			TABLE_STATE,
			TABLE_PINOUT,
			TABLE_LOCATION,
			TABLE_ORIGIN
		};
		
		// droplists related functions
		void CreateLinkedRecord(const int& tableType);
		void LoadDropList(const int& tableType);
		
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

		static bool GetFaultValue(const int& i, const String& faults); // returns the option value for the id selected
		
		PcbDlg(const int& openingType);	
		~PcbDlg();
	
	private:
		Array<Label> 			label;
		ArrayMap<int, Option>  	option;
		Array<Label> 			labelOrigin_;
		ArrayMap<int, Option>  	optionOrigin_;
	
		bool addActionMenuEntryVisible_;
		bool editMenuEntryVisible_;
		bool removeMenuEntryVisible_;
		
		EditString::Style 		editStyle_;
		Ctrl*					currentCtrl_;
		
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
		
		void FlukeTabMenu(Bar& bar);
		void AddFlukeRecord();
		void RemoveFlukeRecord();
		void PopulateFlukeArray();
		
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
