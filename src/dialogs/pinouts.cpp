#include "pinouts.h"
#include "pinout.h"

PinoutsDlg::PinoutsDlg()
{
	CtrlLayout(*this, t_("Pinouts list"));

	BTN_Close <<= THISBACK(DoClose);
	
	TAB_pinouts.SetTable(PINOUT);
	TAB_pinouts.AddIndex(ID);
	TAB_pinouts.AddColumn(LABEL, t_("Label"))
	            .Edit(label_);
	TAB_pinouts.AddColumn(PIN_SIZE,t_("Pin size"))
	            .Edit(pin_size_.MaxChars(5));
	TAB_pinouts.AddColumn(DETAIL,t_("Detail"));
	TAB_pinouts.SetOrderBy(LABEL);
	
	TAB_pinouts.WhenLeftDouble = THISBACK(MenuEdit);
	TAB_pinouts.WhenBar = THISBACK(OwnMenu);
	
	TAB_pinouts.Query();
}
                 
void PinoutsDlg::OwnMenu(Bar& bar)
{
	bar.Add(t_("Add"),THISBACK(MenuAdd));
	bar.Add(t_("Edit"),THISBACK(MenuEdit));
	bar.Add(t_("Remove"),THISBACK(MenuRemove));
}

void PinoutsDlg::MenuAdd()
{
	TAB_pinouts.StartInsert();
}

void PinoutsDlg::MenuEdit()
{
	const auto id = int{TAB_pinouts.GetKey()};
	if (IsNull(id)) {
		return;
	}
	PinoutDlg dlg(OpeningType::opening_edit);

	if (!dlg.ctrls.Load(PINOUT, ID == id)) {
		return;
	}
	if (dlg.Execute() != IDOK) {
		return;
	}
	SQL * dlg.ctrls.Update(PINOUT).Where(ID == id);
	TAB_pinouts.ReQuery();
}

void PinoutsDlg::MenuRemove()
{
	const auto id = int{TAB_pinouts.GetKey()};
	if (IsNull(id) || !PromptYesNo(t_("Delete pinout ?"))) {
	   return;
	}
	SQL * SqlDelete(PINOUT).Where(ID == id);
	TAB_pinouts.ReQuery();
}

void PinoutsDlg::DoClose()
{
	Close();
}