#include "pcbtype.h"

PcbTypeDlg::PcbTypeDlg()
{
	CtrlLayout(*this, "PCB Type");
	BTN_Close <<= Breaker(999);

	TAB_pcbType.SetTable(PCB_TYPE);
	TAB_pcbType.AddIndex(ID);
	TAB_pcbType.AddColumn(LABEL, "Label").Edit(ES_label_);
	TAB_pcbType.Appending().Removing();
	TAB_pcbType.SetOrderBy(LABEL);
	
	TAB_pcbType.Query();
}