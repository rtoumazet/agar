#include "pcb.h"

#include "../utilities/converts.h"
#include "../utilities/lookups.h"

PcbDlg::PcbDlg() {

	CtrlLayout(*this, t_("Pcbs list"));
	BTN_Close <<= Breaker(999);
	
	TAB_pcb.SetTable(PCB);
	TAB_pcb.AddIndex(ID);
	TAB_pcb.AddColumn(GAME_ID,t_("Game"));
	TAB_pcb.AddColumn(PCB_TYPE_ID,t_("Type"));
	TAB_pcb.AddColumn(PCB_STATE_ID, t_("State"));
	TAB_pcb.AddColumn(TAG,t_("Tag"));
	TAB_pcb.AddColumn(LOCATION_ID,t_("Location"));
	TAB_pcb.Appending().Removing();
	//TAB_pcb.SetOrderBy(LABEL);
	
	TAB_pcb.Query();
}

void PcbDlg::MyMenu(Bar& bar) {
	//bar.Add("Option 1",THISBACK(Add));
	//bar.Add("Option 2",THISBACK(Add));
}