#include "../AGAR.h"

class PcbDlg : public WithPcbLayout<TopWindow> {

	typedef PcbDlg CLASSNAME;
	
	public:
		SqlCtrls ctrls;

		void GenerateFaultData();
		void LoadFaultData();
		
		PcbDlg();	
	
	private:
		Array<Label> 			label;
		ArrayMap<int, Option>  	option;
	
		bool GetFaultValue(const int& i); // returns the option value for the id selected
};