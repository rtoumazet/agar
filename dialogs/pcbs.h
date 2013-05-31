#include "../AGAR.h"

class PcbsDlg : public WithPcbsLayout<TopWindow> {

	typedef PcbsDlg CLASSNAME;
	
	private:
		void Create();
		void Edit(int pcbId=0);
		void Remove();

		void ReloadTable();
		
		void OwnMenu(Bar& bar);

	public:
		PcbsDlg();	
	
};