#include "about.h"

AboutDlg::AboutDlg(const String& version) {
	
	CtrlLayout(*this, t_("About"));

	String tmp = Format("[+200 [* %1",version);
	tmp += "]]&Created by Runik&";
	tmp += "[^https://github.com/rtoumazet/agar^ Homepage]";
	tmp += "&Database file path : ";
	//tmp += DeQtf(ConfigFile("AGAR.db"));
	tmp += AppendFileName(GetCurrentDirectory(), "AGAR.db");
	RTC_About = tmp;
}