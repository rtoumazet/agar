#include "../agar.h"
#include "../enums.h"

class PinoutDlg : public WithPinoutLayout<TopWindow> {
	typedef PinoutDlg CLASSNAME;
	
	public:
		PinoutDlg(const OpeningType type);
		
		SqlCtrls ctrls;
};
