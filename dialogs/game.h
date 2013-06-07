#include "../AGAR.h"

class GameDlg : public WithGamesLayout<TopWindow> {

	typedef GameDlg CLASSNAME;
	
	public:
		GameDlg();	
	
	private:
		DropList DL_maker_;
		EditString ES_gameName_;
		
		void OwnMenu(Bar& bar);
		void MenuAdd();
		void MenuEdit();
		void MenuRemove();		

		void InsertCheck();
		
		void SortTable(const int& i);
	
};