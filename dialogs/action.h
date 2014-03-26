////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	action.h
///
/// \brief	Declares the action dialog class. 
////////////////////////////////////////////////////////////////////////////////////////////////////

// PROJECT INCLUDES
#include "agar/agar.h"
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
	
		/// Action type enum
		enum ActionType {
			ANALYSIS, ///< Analysis
			ACTION ///< Action
		};		
		
		/// Opening type enum
		enum OpeningType {
			CREATION, ///< Creation
			EDIT ///< Edition
		};
		
		ActionDlg(const int pcbId, const int actionType, int parentId = 0);
		ActionDlg(const ActionRecord& ar);
		
		void Record(const ActionRecord& ar){ record_ = ar; }
		ActionRecord Record() const { return record_; }
		
			
	private:
	    ActionRecord record_; ///< record currently created / edited
	    
	    void DoOk();
	    
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn	void InitializeFields(const int type)
	///
	/// \brief	Fields initialization . 
	///
	/// \author	Runik
	/// \date	10/04/2010
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void InitializeFields(const int type);
	
};