#include "agar/agar.h"

class OriginsDlg : public WithOriginsLayout<TopWindow> {
	typedef OriginsDlg CLASSNAME;

	private:
		void Create();
		void Edit();
		void Remove();
		
		void OwnMenu(Bar& bar);

	public:
		OriginsDlg();
};