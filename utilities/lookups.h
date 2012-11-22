#ifndef _SqlArray2_lookups_h_
#define _SqlArray2_lookups_h_

enum {
	cachetime = 600
};

//This for use in code
#define Lookup(table, key,  value) \
Lookup##table##key##value

//This for use in header files
#define LookupHdr(table, key,  value) \
struct Lookup##table##key##value : Convert { \
	virtual Value Format(const Value& q) const; \
};

//This for use in class definition file
#define LookupSrc(table, key,  value) \
Value Lookup##table##key##value::Format(const Value& q) const \
{ \
	if(q.IsNull()) return Null; \
	static VectorMap<int, String> cache; \
	static Time lastcleartime; \
	if(GetSysTime()-lastcleartime > cachetime){ \
		cache.Clear(); \
		lastcleartime=GetSysTime(); \
	} \
	if(cache.Find(q) > 0) \
		return cache.Get(q, String()); \
	else { \
		Sql sql; \
		sql * Select(key, value).From(table).Where(key==q); \
		if(sql.Fetch()) { \
			int sid = sql[key]; \
			cache.Add(sid, sql[value]); \
			return cache.Get(q, String()); \
		}else \
			return (String)(""); \
	} \
}



#endif
