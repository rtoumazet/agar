#include "../agar.h"

class MakerDlg : public WithMakersLayout<TopWindow> {
	typedef MakerDlg CLASSNAME;

	public:
		MakerDlg();
	
	private:
		void OwnMenu(Bar& bar);
		void MenuAdd();
		void MenuEdit();
		void MenuRemove();
		
		void InsertCheck();
	
		void DoClose();
		
        EditString ES_name_;
};
