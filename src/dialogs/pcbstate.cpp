#include "pcbstate.h"

struct CellDisplay : public Display {

	virtual void Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const
	{
		w.DrawRect(r, q);
	}
};


PcbStateDlg::PcbStateDlg()
{
	CtrlLayout(*this, t_("PCB State"));
	BTN_Close <<= Breaker(999);

	TAB_pcbStateArray.AddIndex();
	TAB_pcbStateArray.AddColumn("Label")
	    .Edit(ES_label_);
	TAB_pcbStateArray.AddIndex(); // Text label in raw form
	TAB_pcbStateArray.AddColumn("Background")
	    .Margin(0)
	    .Ctrls(THISBACK1(WidgetFactory,1));
	TAB_pcbStateArray.AddColumn("Text")
	    .Margin(0).Ctrls(THISBACK1(WidgetFactory,2));
	TAB_pcbStateArray.ColumnWidths("189 80 82");
	TAB_pcbStateArray.Appending();
	TAB_pcbStateArray.WhenLeftDouble = THISBACK(Edit);
	TAB_pcbStateArray.WhenAcceptEdit = THISBACK(AfterEdit);

	Sql sql;
	sql.Execute("select ID,LABEL,INK,PAPER from PCB_STATE");
	while (sql.Fetch()) {
		const auto ink   = Color::FromRaw(static_cast<dword>(sql[2].To<int64>()));
		const auto paper = Color::FromRaw(static_cast<dword>(sql[3].To<int64>()));
		
		TAB_pcbStateArray.Add(
			sql[0], // record id
			AttrText(sql[1].ToString()).Ink(ink).Paper(paper), // formatted text
			sql[1].ToString(), // raw text
			paper, // background color
			ink // text color
		);
    }
}

void PcbStateDlg::MyMenu(Bar& bar)
{
	bar.Add("Option 1",THISBACK(Add));
	bar.Add("Option 2",THISBACK(Add));
}

void PcbStateDlg::WidgetFactory(int line, One<Ctrl>& x, int cellIndex)
{
	auto& cp = x.Create<ColorPusher>();
    cp <<= THISBACK1(CellUpdate, line);
    TAB_pcbStateArray.SetDisplay(line, cellIndex, Single<CellDisplay>());
}

void PcbStateDlg::CellUpdate(int i)
{

	// getting updated data from the table
	const auto str      = TAB_pcbStateArray.Get(i,2).ToString();
	const auto paper    = Color{TAB_pcbStateArray.Get(i,3)};
	const auto ink      = Color{TAB_pcbStateArray.Get(i,4)};
	const auto id       = int{TAB_pcbStateArray.Get(i,0)};
	
	// updating the display
	TAB_pcbStateArray.Set(i,1,AttrText(str).Ink(ink).Paper(paper)); 
	
	// saving changes to the database
	Sql sql;
	const auto statement = Format("update PCB_STATE set LABEL='%s', INK=%d, PAPER=%d where ID=%i",
		str,
		static_cast<int>(ink.GetRaw()),
		static_cast<int>(paper.GetRaw()),
		id
	);
	sql.Execute(statement);
}

void PcbStateDlg::Edit()
{
	// Cell is initialized with the raw text value to prevent a blank line during edit
	TAB_pcbStateArray.Set(TAB_pcbStateArray.GetCursor(),1,TAB_pcbStateArray.Get(TAB_pcbStateArray.GetCursor(),2).ToString());
	TAB_pcbStateArray.DoEdit();
}

void PcbStateDlg::AfterEdit()
{
	const auto cursor   = TAB_pcbStateArray.GetCursor(); // getting current line id
	const auto str      = TAB_pcbStateArray.Get(cursor,1).ToString(); // getting updated text
	TAB_pcbStateArray.Set(cursor,2,str); // updating raw text cell

    CellUpdate(cursor);
}