#include "../agar.h"

class PcbStateDlg : public WithPcbStatesLayout<TopWindow> {
	typedef PcbStateDlg CLASSNAME;

public:
	PcbStateDlg();
	
private:
	EditString ES_label_;
	
	void MyMenu(Bar& bar);
	void Add(){};
	
	void WidgetFactory(int line, One<Ctrl>& x, int cellIndex);
	void CellUpdate(int i);
	
	void Edit();
	void AfterEdit();

};