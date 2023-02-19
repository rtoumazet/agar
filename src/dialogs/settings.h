#include "../agar.h"

class SettingsDlg : public WithSettingsLayout<TopWindow> {
	typedef SettingsDlg CLASSNAME;

	public:
		SettingsDlg();
		~SettingsDlg();
		
		void clickOption();
};
