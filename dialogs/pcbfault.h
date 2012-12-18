#include "..\AGAR.h"

class PcbFaultDlg : public WithPcbFaultLayout<TopWindow> {
	typedef PcbFaultDlg CLASSNAME;

public:
	PcbFaultDlg();
	
private:
	EditString ES_label_;
	
	void MyMenu(Bar& bar);
	void Add(){};

	
};