#pragma once

/// Type of item enum
enum class ItemType : int {
	analysis	= 0,
	action		= 1,
	unknown     = -1
};

enum class OpeningType {
	opening_new,
	opening_edit
};