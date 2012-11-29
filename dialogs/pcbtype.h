#include "..\AGAR.h"

class PcbTypeDlg : public WithPcbTypeLayout<TopWindow> {
	typedef PcbTypeDlg CLASSNAME;

public:
	PcbTypeDlg();
	
private:
	EditString ES_label_;
	
	void MyMenu(Bar& bar);
	void Add(){};

	
};