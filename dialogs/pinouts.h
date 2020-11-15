#include "agar/agar.h"

class PinoutsDlg : public WithPinoutsLayout<TopWindow> {
	typedef PinoutsDlg CLASSNAME;

	public:
		PinoutsDlg();

	private:
		void MenuAdd();
		void MenuEdit();
		void MenuRemove();
		
		void OwnMenu(Bar& bar);
		
		void DoClose();
		
		DocEdit label_;
		EditField pin_size_;
};