////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	action.h
///
/// \brief	Declares the action dialog class. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// PROJECT INCLUDES
#include "../agar.h"
#include "../enums.h"
#include "pcb.h" // ActionRecord


////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	ActionDlg
///
/// \brief	Action dialog class. 
///
/// \author	Runik
/// \date	30/01/2014
////////////////////////////////////////////////////////////////////////////////////////////////////
class ActionDlg : public WithActionLayout<TopWindow> {

	typedef ActionDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;
	
		/// Opening type enum
		enum OpeningType {
			CREATION, ///< Creation
			EDIT ///< Edition
		};
		
		ActionDlg(const int pcb_id, const ItemType type, int parent_id = 0);
		ActionDlg(const ActionRecord& ar);
		
		void Record(const ActionRecord& ar){ record_ = ar; }
		ActionRecord Record() const { return record_; }
			
	private:
	    ActionRecord record_; ///< record currently created / edited

	    void DoOk();
	    
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn	void InitializeFields(const ItemType type)
	///
	/// \brief	Fields initialization . 
	///
	/// \author	Runik
	/// \date	10/04/2010
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void InitializeFields(const ItemType type);
	
};