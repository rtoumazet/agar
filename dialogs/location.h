#include "..\AGAR.h"

class LocationDlg : public WithLocationLayout<TopWindow> {

	typedef LocationDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;
		
		LocationDlg();	
		
		bool Key(dword key,int);
		
	
};