#include "../agar.h"
#include "../enums.h"

class OriginDlg : public WithOriginLayout<TopWindow> {

	typedef OriginDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;
		
		OriginDlg(const OpeningType type);
};