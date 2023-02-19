#include "viewer.h"
#include "../agar.h"

void ViewerDlg::callbackViewMenu(const int i)
{
	switch(i)
	{
		case -1 :
			raster_.ZoomWidth();
			break;
		case -2 :
			raster_.ZoomPage();
			break;
		default :
			raster_.Zoom(i);
			break;
	}
}

void ViewerDlg::openViewMenu(Bar& bar)
{
    bar.Add( "Page width", THISBACK1( callbackViewMenu,  -1 ) );
    bar.Add( "Full page",  THISBACK1( callbackViewMenu,  -2 ) );
    bar.Add( "Zoom 10%",   THISBACK1( callbackViewMenu,  10 ) );
    bar.Add( "Zoom 20%",   THISBACK1( callbackViewMenu,  20 ) );
    bar.Add( "Zoom 50%",   THISBACK1( callbackViewMenu,  50 ) );
    bar.Add( "Zoom 100%",  THISBACK1( callbackViewMenu, 100 ) );
    bar.Add( "Zoom 150%",  THISBACK1( callbackViewMenu, 150 ) );
}

void ViewerDlg::openMainMenu(Bar& bar)
{
    bar.Add( "View", THISBACK(openViewMenu) );
}

ViewerDlg::ViewerDlg(const int id)
{
    AddFrame(menu_);
    AddFrame(TopSeparatorFrame());
    AddFrame(status_);
    AddFrame(InsetFrame());

    menu_.Set(THISBACK(openMainMenu));

	Add(raster_.HSizePos().VSizePos());

    Sizeable().Zoomable();
	BackPaint();

	Refresh();

	SQL * Select(DATA).From(PICTURE).Where(ID == id);
    if (SQL.Fetch()) {
		StringStream ss(SQL[DATA]);
		raster_.Open(ss);
    }
}
