#include "pcbs.h"

#include "../utilities/converts.h"
#include "../utilities/lookups.h"

PcbsDlg::PcbsDlg() {

	CtrlLayout(*this, t_("Pcbs list"));
	BTN_Close <<= Breaker(999);
	
	TAB_pcbs.SetTable(PCB);
	TAB_pcbs.AddIndex(ID);
	TAB_pcbs.AddColumn(GAME_ID,t_("Game"));
	TAB_pcbs.AddColumn(PCB_TYPE_ID,t_("Type"));
	TAB_pcbs.AddColumn(PCB_STATE_ID, t_("State"));
	TAB_pcbs.AddColumn(TAG,t_("Tag"));
	TAB_pcbs.AddColumn(LOCATION_ID,t_("Location"));
	TAB_pcbs.Appending().Removing();
	//TAB_pcbs.SetOrderBy(LABEL);
	
	TAB_pcbs.Query();
}

void PcbsDlg::MyMenu(Bar& bar) {
	//bar.Add("Option 1",THISBACK(Add));
	//bar.Add("Option 2",THISBACK(Add));
}