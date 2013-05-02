#include "..\AGAR.h"

class MakerDlg : public WithMakersLayout<TopWindow> {

	typedef MakerDlg CLASSNAME;

	public:
		MakerDlg();	
	
	private:
		EditString ES_name_;
		
		void OwnMenu(Bar& bar);
		void MenuAdd();
		void MenuEdit();
		void MenuRemove();
		
		void InsertCheck();
	
};