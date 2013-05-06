#include "../AGAR.h"

class PcbDlg : public WithPcbLayout<TopWindow> {

	typedef PcbDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;
		
		Array<Label> 			label;
		ArrayMap<int, Option>  	option;
		
		PcbDlg();	
	
};