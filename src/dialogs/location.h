#include "../agar.h"
#include "../enums.h"

class LocationDlg : public WithLocationLayout<TopWindow> {

	typedef LocationDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;
		
		LocationDlg(const OpeningType type);

};