#include "..\AGAR.h"

class PcbTypeDlg : public WithPcbTypeLayout<TopWindow> {
	typedef PcbTypeDlg CLASSNAME;

public:
	PcbTypeDlg();
	
private:
	EditString ES_label_;
	
};