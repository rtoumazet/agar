#include "agar/agar.h"

class PinoutsDlg : public WithPinoutsLayout<TopWindow> {
	typedef PinoutsDlg CLASSNAME;

	private:
		void Create();
		void Edit();
		void Remove();
		
		void OwnMenu(Bar& bar);
		
		void DoClose();

	public:
		PinoutsDlg();
};