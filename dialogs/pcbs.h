#include "../AGAR.h"

class PcbsDlg : public WithPcbsLayout<TopWindow> {

	typedef PcbsDlg CLASSNAME;
	
	private:
		void Create();
		void Edit(int pcbId=0);
		void Remove();

		void ReloadTable(const bool& ascSort);
		void SortTable(const int& i);
		
		void OwnMenu(Bar& bar);
		
		bool isSortedAsc_;

	public:
		PcbsDlg();	
	
};