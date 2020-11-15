#include "agar/agar.h"

class LocationsDlg : public WithLocationsLayout<TopWindow> {
	typedef LocationsDlg CLASSNAME;

	public:
		LocationsDlg();
		
	private:
		void MenuAdd();
		void MenuEdit();
		void MenuRemove();
		
		void OwnMenu(Bar& bar);
		
		void DoClose();
		
		EditString location_;


};