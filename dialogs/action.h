#include "../AGAR.h"

class ActionDlg : public WithActionLayout<TopWindow> {

	typedef ActionDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;
	
		enum ActionType {
			ANALYSIS,
			ACTION
		};		
		
		enum OpeningType {
			CREATION,
			EDIT
		};
		
		ActionDlg(const int& pcbId, const int& actionType, const int& openingType, int parentId = 0);
		ActionDlg(const int& pcbId, const int& actionId);
	
	private:
	
};