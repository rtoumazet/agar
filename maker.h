#include "AGAR.h"

class MakerDlg : public WithMakerLayout<TopWindow> {

	public:
		
		MakerDlg();	
		
	private:
		void Test(){PromptOK("You have clicked the button!");}
	
	typedef MakerDlg CLASSNAME;
};