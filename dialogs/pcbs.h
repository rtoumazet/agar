#include "../AGAR.h"

class PcbsDlg : public WithPcbsLayout<TopWindow> {

	typedef PcbsDlg CLASSNAME;
	
	private:
		void Create();
		void Edit();
		void Remove();
		
		void MyMenu(Bar& bar);

	public:
		PcbsDlg();	
	
};