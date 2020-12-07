#include "../agar.h"

class PcbTypeDlg : public WithPcbTypesLayout<TopWindow> {
	typedef PcbTypeDlg CLASSNAME;

public:
	PcbTypeDlg();
	
private:
	void MyMenu(Bar& bar);
	void Add(){};

	EditString ES_label_;
};