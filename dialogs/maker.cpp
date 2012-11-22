#include "maker.h"

MakerDlg::MakerDlg() {
	
	CtrlLayout(*this, t_("Makers list"));
	BTN_Close <<= Breaker(999);
	
	
	
	TAB_maker.SetTable(MAKER);
	TAB_maker.AddIndex(ID);
	TAB_maker.AddColumn(MAKER_NAME, "NAME").Edit(ES_name_);
	TAB_maker.Appending().Removing();
	TAB_maker.SetOrderBy(MAKER_NAME);
	
	TAB_maker.Query();

}