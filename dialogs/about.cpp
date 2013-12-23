#include "about.h"

AboutDlg::AboutDlg(const String& version) {
	
	CtrlLayout(*this, t_("About"));

	String tmp = Format("[+200 [* %1",version);
	tmp += "]]&Created by Runik&";
	tmp += "[^https://bitbucket.org/Runik/agar/wiki/Home^ Homepage]";
	tmp += "&Config file path : ";
	tmp += DeQtf(ConfigFile("AGAR.db"));
	RTC_About = tmp;
}