////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	viewer.h
///
/// \brief	Declares the viewer dialog class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <CtrlLib/CtrlLib.h>
#include <RasterCtrl/RasterCtrl.h>

using namespace Upp;

class ViewerDlg : public TopWindow
{
	public:
		typedef ViewerDlg CLASSNAME;
		
		ViewerDlg(const int i);
		
	private:
		void callbackViewMenu(const int i);
		void openViewMenu(Bar& bar);
		void openMainMenu(Bar& bar);

		MenuBar menu_;
		StatusBar status_;
		RasterCtrl raster_;
};
