#include "settings.h"

SettingsDlg::SettingsDlg()
{
	CtrlLayout(*this, t_("Settings"));
	
	auto const cfg = LoadIniFile("agar.cfg");
	
	O_NoResize      = StrInt(cfg.Get("NoResize", Null));
	E_ImageWidth    = StrInt(cfg.Get("ImageWidth", Null));
	E_ImageHeight   = StrInt(cfg.Get("ImageHeight", Null));
	
	O_NoResize <<= THISBACK(clickOption);
	B_ResetInitialFault.Disable();
}

SettingsDlg::~SettingsDlg()
{
	auto cfg = LoadIniFile("agar.cfg");
	addConfigurationValue(cfg, "NoResize", IntStr(~O_NoResize));
	addConfigurationValue(cfg, "ImageWidth", IntStr(~E_ImageWidth));
	addConfigurationValue(cfg, "ImageHeight", IntStr(~E_ImageHeight));
	
	saveConfiguration("agar.cfg", cfg);	
}

void SettingsDlg::clickOption()
{
	if (O_NoResize.Get() == 1) {
		E_ImageWidth.Disable();
		E_ImageHeight.Disable();
	} else {
        E_ImageWidth.Enable();
		E_ImageHeight.Enable();
	}
}