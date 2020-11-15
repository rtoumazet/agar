#include "agar/agar.h"

class GameDlg : public WithGamesLayout<TopWindow> {

	typedef GameDlg CLASSNAME;
	
	public:
		GameDlg();	
	
	private:
		void OwnMenu(Bar& bar);
		void MenuAdd();
		void MenuEdit();
		void MenuRemove();

		void InsertCheck();
		
		void SortTable(const int& i);
		
		void DoClose();
	
		DropList manufacturer_;
		EditString game_name_;

};