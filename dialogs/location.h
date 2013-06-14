#include "agar/agar.h"

class LocationDlg : public WithLocationLayout<TopWindow> {

	typedef LocationDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;
		
		LocationDlg();	

};