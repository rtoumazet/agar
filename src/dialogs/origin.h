#include "../agar.h"

class OriginDlg : public WithOriginLayout<TopWindow> {

	typedef OriginDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;
		
		OriginDlg(const int& openingType);	
};