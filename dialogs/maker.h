#include "..\AGAR.h"

class MakerDlg : public WithMakerLayout<TopWindow> {

	public:
		MakerDlg();	
	
	private:
		EditString ES_name_;
	
	typedef MakerDlg CLASSNAME;
};