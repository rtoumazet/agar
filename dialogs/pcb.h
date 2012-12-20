#include "../AGAR.h"

class PcbDlg : public WithPcbsLayout<TopWindow> {

	typedef PcbDlg CLASSNAME;
	
	public:
		PcbDlg();	
	

	private:
		void MyMenu(Bar& bar);

	
};