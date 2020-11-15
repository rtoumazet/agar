#include "../agar.h"

class OriginsDlg : public WithOriginsLayout<TopWindow> {
	typedef OriginsDlg CLASSNAME;

	public:
		OriginsDlg();

	private:
		void MenuAdd();
		void MenuEdit();
		void MenuRemove();
		void OwnMenu(Bar& bar);
		void DoClose();
		
		DocEdit name_;
		DocEdit origin_;
		DocEdit comment_;
};