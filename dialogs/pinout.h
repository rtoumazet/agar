#include "agar/agar.h"

class PinoutDlg : public WithPinoutLayout<TopWindow> {

	typedef PinoutDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;
		
		PinoutDlg();	
};