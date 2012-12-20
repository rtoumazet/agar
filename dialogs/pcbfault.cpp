#include "pcbfault.h"

PcbFaultDlg::PcbFaultDlg()
{
	CtrlLayout(*this, t_("PCB Fault"));
	BTN_Close <<= Breaker(999);
	//TAB_pcbFault.WhenBar = THISBACK(MyMenu);

	TAB_pcbFault.SetTable(PCB_FAULT);
	TAB_pcbFault.AddIndex(ID);
	TAB_pcbFault.AddColumn(LABEL, "Label").Edit(ES_label_);
	TAB_pcbFault.Appending().Removing();
	TAB_pcbFault.SetOrderBy(LABEL);
	
	TAB_pcbFault.Query();
}

void PcbFaultDlg::MyMenu(Bar& bar) {
	//bar.Add("Option 1",THISBACK(Add));
	//bar.Add("Option 2",THISBACK(Add));
}