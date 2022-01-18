#include "origins.h"
#include "origin.h"

OriginsDlg::OriginsDlg()
{
	CtrlLayout(*this, t_("Origins list"));
	BTN_Close <<= THISBACK(DoClose);
	
	TAB_origins.SetTable(ORIGIN);
	TAB_origins.AddIndex(ID);
	TAB_origins.AddColumn(NAME, t_("Name")).Edit(name_);
	TAB_origins.AddColumn(ORIGIN,t_("Origin")).Edit(origin_);
	TAB_origins.AddColumn(COMMENTARY,t_("Comment")).Edit(comment_);
	TAB_origins.SetOrderBy(ORIGIN);

	TAB_origins.WhenLeftDouble = THISBACK(MenuEdit);
	TAB_origins.WhenBar = THISBACK(OwnMenu);

	TAB_origins.Query();
}
                 
void OriginsDlg::OwnMenu(Bar& bar)
{
	bar.Add(t_("Add"),THISBACK(MenuAdd));
	bar.Add(t_("Edit"),THISBACK(MenuEdit));
	bar.Add(t_("Remove"),THISBACK(MenuRemove));
}

void OriginsDlg::MenuAdd()
{
	TAB_origins.StartInsert();
}

void OriginsDlg::MenuEdit()
{
	TAB_origins.DoEdit();
}

void OriginsDlg::MenuRemove()
{
	const auto id = int{TAB_origins.GetKey()};
	if (IsNull(id) || !PromptYesNo(t_("Delete origin ?"))) {
	   return;
	}
	 SQL * SqlDelete(ORIGIN).Where(ID == id);
	 TAB_origins.ReQuery();
}

void OriginsDlg::DoClose()
{
	Close();
}