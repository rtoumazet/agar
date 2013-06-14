#include "../agar.h"

class LocationsDlg : public WithLocationsLayout<TopWindow> {
	typedef LocationsDlg CLASSNAME;

	private:
		void Create();
		void Edit();
		void Remove();
		
		void OwnMenu(Bar& bar);

	public:
		LocationsDlg();
};