#include "pcbtype.h"

PcbTypeDlg::PcbTypeDlg()
{
	CtrlLayout(*this, "PCB Type");
	BTN_Close <<= Breaker(999);
}