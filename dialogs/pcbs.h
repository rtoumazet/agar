#include "../agar.h"

class PcbsDlg : public WithPcbsLayout<TopWindow> {

	typedef PcbsDlg CLASSNAME;
	
	private:
		void Create();
		void Edit(int pcbId=0);
		void Remove();

		void OwnMenu(Bar& bar);

		// Sort functions and variables
		bool isSortedAsc_;
		void ReloadTable(const bool& ascSort);
		void SortTable(const int& i);
		
		// Filter functions and variables
		int 					filterState_;
		//Array<Label> 			label_;
		ArrayMap<int, Option>  	option_;
		
		void 					LoadFaultData();
		void 					ExecuteFilter();

	public:
		PcbsDlg();	
	
};