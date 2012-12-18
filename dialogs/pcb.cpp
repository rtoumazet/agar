#include "pcb.h"

#include "../utilities/converts.h"
#include "../utilities/lookups.h"

PcbDlg::PcbDlg() {

	CtrlLayout(*this, t_("Pcbs list"));
	BTN_Close <<= Breaker(999);
	
	option1.Set(1);
}