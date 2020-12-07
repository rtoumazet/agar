#include "../agar.h"

class PcbFaultDlg : public WithPcbFaultsLayout<TopWindow> {
	typedef PcbFaultDlg CLASSNAME;

public:
	PcbFaultDlg();
	
private:
	void MyMenu(Bar& bar);

	EditString ES_label_;
};