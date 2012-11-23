#include "game.h"

#include "../utilities/converts.h"
#include "../utilities/lookups.h"

GameDlg::GameDlg() {

	CtrlLayout(*this, t_("Games list"));
	BTN_Close <<= Breaker(999);
	
	
	Sql sql;
	sql * Select(SqlAll()).From(MAKER).OrderBy(MAKER_NAME);
	while(sql.Fetch()) {
		DL_maker_.Add(sql[0],sql[1]);
	}
	
	TAB_game.SetTable(GAME);
	TAB_game.AddIndex(ID);
	TAB_game.AddColumn(MAKER_ID,"Maker").SetConvert(Single<Lookup(MAKER,ID,MAKER_NAME)>()).Edit(DL_maker_);
	TAB_game.AddColumn(GAME_NAME,"Game name").Edit(ES_gameName_);
	TAB_game.Appending().Removing();
	TAB_game.SetOrderBy(GAME_NAME);
	
	TAB_game.Query();	
}