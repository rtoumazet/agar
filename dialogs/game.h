#include "../AGAR.h"

class GameDlg : public WithGameLayout<TopWindow> {

	typedef GameDlg CLASSNAME;
	
	public:
		GameDlg();	
	
	private:
		DropList DL_maker_;
		EditString ES_gameName_;
	
};