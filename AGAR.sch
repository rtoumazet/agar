TABLE_(MAKER)
	INT_ 	(ID) PRIMARY_KEY AUTO_INCREMENT
	STRING_	(MAKER_NAME, 100) INDEX
END_TABLE

TABLE_(GAME)
	INT 	(ID) PRIMARY_KEY AUTO_INCREMENT
	INT_    (MAKER_ID) REFERENCES(MAKER)
	STRING_	(GAME_NAME, 200) INDEX
END_TABLE

TABLE (PCB_TYPE)
	INT		(ID) PRIMARY_KEY AUTO_INCREMENT
	STRING_	(PCB_TYPE_LABEL, 200) INDEX
END_TABLE

TABLE (PCB_STATE)
	INT		(ID) PRIMARY_KEY AUTO_INCREMENT
	STRING_	(PCB_STATE_LABEL, 200) INDEX
END_TABLE

TABLE (PCB_FAULT)
	INT		(ID) PRIMARY_KEY AUTO_INCREMENT
	STRING_	(PCB_FAULT_LABEL, 200) INDEX
END_TABLE