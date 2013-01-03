#include "..\AGAR.h"

class OriginDlg : public WithOriginLayout<TopWindow> {

	typedef OriginDlg CLASSNAME;
	
	private:
		bool Key(dword key, int count);
	
	public:
		SqlCtrls ctrls;
		
		OriginDlg();	
	
};