#include "pcbstate.h"

PcbStateDlg::PcbStateDlg()
{
	CtrlLayout(*this, t_("PCB State"));
	BTN_Close <<= Breaker(999);
	//TAB_pcbState.WhenBar = THISBACK(MyMenu);

	TAB_pcbState.SetTable(PCB_STATE);
	TAB_pcbState.AddIndex(ID);
	TAB_pcbState.AddColumn(LABEL, "Label").Edit(ES_label_);
	TAB_pcbState.Appending().Removing();
	TAB_pcbState.SetOrderBy(LABEL);
	
	TAB_pcbState.Query();
}

void PcbStateDlg::MyMenu(Bar& bar) {
	bar.Add("Option 1",THISBACK(Add));
	bar.Add("Option 2",THISBACK(Add));
}