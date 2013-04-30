TABLE_(MAKER)
	INT_ 	(ID) PRIMARY_KEY AUTO_INCREMENT
	STRING_	(MAKER_NAME, 100) INDEX
END_TABLE

TABLE_(GAME)
	INT 	(ID) PRIMARY_KEY AUTO_INCREMENT
	INT_    (MAKER_ID) REFERENCES(MAKER.ID)
	STRING_	(GAME_NAME, 200) INDEX
END_TABLE

TABLE_(PCB_TYPE)
	INT		(ID) PRIMARY_KEY AUTO_INCREMENT
	STRING_	(LABEL, 200) INDEX
END_TABLE

TABLE_(PCB_STATE)
	INT		(ID) PRIMARY_KEY AUTO_INCREMENT
	STRING	(LABEL, 200) INDEX
	INT_	(INK)
	INT_ 	(PAPER)
END_TABLE

TABLE_(PCB_FAULT)
	INT		(ID) PRIMARY_KEY AUTO_INCREMENT
	STRING	(LABEL, 200) INDEX
END_TABLE

TABLE_(PCB)
	INT		(ID) PRIMARY_KEY AUTO_INCREMENT
	INT_	(GAME_ID) REFERENCES(GAME.ID)
	INT_	(PCB_TYPE_ID) REFERENCES(PCB_TYPE.ID)
	INT_	(PCB_STATE_ID) REFERENCES(PCB_STATE.ID)
	STRING_	(PCB_FAULT_OPTION,20)
	STRING_	(TAG, 50) INDEX
	INT_	(LOCATION_ID) REFERENCES(LOCATION.ID)
	INT_	(ORIGIN_ID) REFERENCES(ORIGIN.ID)
	DATE_	(LAST_TEST_DATE) INDEX
END_TABLE

TABLE_(ANALYSIS)
	INT		(ID) PRIMARY_KEY AUTO_INCREMENT
	INT_	(PCB_ID) REFERENCES(PCB.ID)
	DATE_	(ANALYSIS_DATE)
	STRING_	(COMMENTARY, 1000) INDEX
END_TABLE

TABLE_(ACTION)
	INT		(ID) PRIMARY_KEY AUTO_INCREMENT
	INT		(PCB_ID) REFERENCES(PCB.ID)
	DATE_	(ACTION_DATE)
	INT		(PCB_STATE_ID) REFERENCES(PCB_STATE.ID)
	STRING	(COMMENTARY, 1000) INDEX
END_TABLE

TABLE_(LOCATION)
	INT		(ID) PRIMARY_KEY AUTO_INCREMENT
	STRING	(LABEL, 200) INDEX
END_TABLE

TABLE_(ORIGIN)
	INT		(ID) PRIMARY_KEY AUTO_INCREMENT
	STRING_	(NAME, 200) INDEX
	STRING	(ORIGIN, 200) INDEX
	STRING	(COMMENTARY, 1000) INDEX
END_TABLE	