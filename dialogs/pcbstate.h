#include "..\AGAR.h"

class PcbStateDlg : public WithPcbStateLayout<TopWindow> {
	typedef PcbStateDlg CLASSNAME;

public:
	PcbStateDlg();
	
private:
	EditString ES_label_;
	
	void MyMenu(Bar& bar);
	void Add(){};

	
};