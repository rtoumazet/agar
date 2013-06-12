#include "about.h"

AboutDlg::AboutDlg() {
	
	CtrlLayout(*this, t_("About"));
	//BTN_Close <<= Breaker(999);

	String tmp = "[+200 [* AGAR v1.0]]&";
	tmp += "Created by Runik&";
	tmp += "[^https://bitbucket.org/Runik/agar/wiki/Home^ Homepage]";
	//RTC_About = "Test";
	RTC_About = tmp;
}