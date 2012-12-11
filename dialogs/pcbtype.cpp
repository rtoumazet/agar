#include "pcbtype.h"

PcbTypeDlg::PcbTypeDlg()
{
	CtrlLayout(*this, t_("PCB Type"));
	BTN_Close <<= Breaker(999);
	TAB_pcbType.WhenBar = THISBACK(MyMenu);

	TAB_pcbType.SetTable(PCB_TYPE);
	TAB_pcbType.AddIndex(ID);
	TAB_pcbType.AddColumn(LABEL, "Label").Edit(ES_label_);
	TAB_pcbType.Appending().Removing();
	TAB_pcbType.SetOrderBy(LABEL);
	
	TAB_pcbType.Query();
}

void PcbTypeDlg::MyMenu(Bar& bar) {
	bar.Add("Option 1",THISBACK(Add));
	bar.Add("Option 2",THISBACK(Add));
}