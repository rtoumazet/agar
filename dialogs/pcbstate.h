#include "..\AGAR.h"

class PcbStateDlg : public WithPcbStatesLayout<TopWindow> {
	typedef PcbStateDlg CLASSNAME;

public:
	PcbStateDlg();
	
private:
	EditString ES_label_;
	
	void MyMenu(Bar& bar);
	void Add(){};

	
};