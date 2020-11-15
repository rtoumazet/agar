#include "../agar.h"
#include "../enums.h"

class PinoutDlg : public WithPinoutLayout<TopWindow> {

	typedef PinoutDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;
		
		PinoutDlg(const OpeningType type);
};