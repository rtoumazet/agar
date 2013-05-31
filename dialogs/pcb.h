#include "../AGAR.h"

#define IMAGECLASS MyImages
#define IMAGEFILE  "../images.iml"
#include <Draw/iml_header.h>

class PcbDlg : public WithPcbLayout<TopWindow> {

	typedef PcbDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;

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
		
		void AddAnalysis(const int& pcbId);
		void AddAction(const int& pcbId);
		void Edit();
		void Remove();
		
		void BuildActionTree(const int& pcbId);
		
		

		int	GetRecordNumber(const int& pcbId);
		
		PcbDlg();	
	
	private:
		Array<Label> 			label;
		ArrayMap<int, Option>  	option;
	
		bool GetFaultValue(const int& i); // returns the option value for the id selected
		
		bool addActionMenuEntryVisible_;
		bool editMenuEntryVisible_;
		bool removeMenuEntryVisible_;
		
		
};