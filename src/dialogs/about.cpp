#include "about.h"

AboutDlg::AboutDlg(const String& version)
{
	CtrlLayout(*this, t_("About"));

	auto tmp = Format("[+200 [* %1",version);
	tmp += "]]&Created by Runik&";
	tmp += "[^https://rtoumazet.github.io/agar/^ Homepage]";
	tmp += "&Database file path : ";
	tmp += DeQtf(AppendFileName(GetCurrentDirectory(), "AGAR.db"));
	RTC_About = tmp;
}