#include "../agar.h"

enum SortDirection {
	SORT_UP		= 1,
	SORT_DOWN	= 2,
	SORT_NONE	= 0
};

class PcbsDlg : public WithPcbsLayout<TopWindow> {
	typedef PcbsDlg CLASSNAME;
	
	public:
		PcbsDlg();
		~PcbsDlg();

	private:
		void Create();
		void Edit(const int pcb_id=0);
		void Remove();
		void GenerateReport();

		void OwnMenu(Bar& bar);

		// Sort functions and variables
		void ReloadTable(const bool asc_sort);
		void SortTable(const int i);
		
		void LoadFaultData();
		void ExecuteFilter();
		
		// listing extraction functions
		void ExtractListing();
		
		bool                    is_sorted_asc_;
		int                     sorted_column_index_;
		int                     sorted_column_direction_; // based on SortDirection enum
        int                     filter_state_;
        ArrayMap<int, Option>   option_;
};

struct Data {
    void Jsonize(JsonIO& json);
    void Xmlize(XmlIO& xio) { XmlizeByJsonize(xio, *this); }
    
    String game;
    String type;
    String tag;
    String location;
    String state;
};
