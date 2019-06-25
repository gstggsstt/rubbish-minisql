#pragma once
#include "selector.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <map>
using namespace std;

class interpreter
{
private:
	selector sr;
	void printRecord(map<string, vector<string>> &res,
		vector<string> & Attrs, vector<int> & width, int x);
	void printLine(vector<int> & width);

public:
	interpreter();
	~interpreter();
	void flush(); // flush all info to disk

	// Supported commands:
	//   select * from tableName [where condition];
	//   select (attr1, attr2, ... , attrN) from tableName [where condition];
	//
	//   create table tableName (attr1 type1 [primary unique notnull], ... , attrN typeN);
	//   create table tableName (attr1 type1 [unique notnull], ... , attrN typeN, primary key(attr));
	//
	//   create index indexName on tableName (attrName);
	//
	//   insert into tableName values(val1, val2, ... , valN);
	//   insert into tableName values(attr1=val1, attr2=val2, ... , attrN=valN);
	//
	//   delete * from tableName [where condition];
	//   delete (attr1, attr2, ... , attrN) from tableName [where condition];
	//
	//   drop table tableName;
	//
	//   drop index indexName;
	//
	// supported where expression:
	//   eg. where attr>val          >,<,>=,<=,<>
	//   eg. where attr1<>attr2
	//   eg. where attr1<val1 and ( attr2>=val2 or attr3<>val3 )
	//   eg. where attr between val1, val2
	//      note: between clause turns into :   attr>=val1 and attr<=val2
	//
	//  NOTE:
	//    UNDEFINED values is allowed in tables, but compasion with UNDEFINED is NOT supported.
	//    So if you insert an all-undefined record, you will not be able to access it.

	map<string,vector<string>> execute(string cmd, int printOrNot=-1);

	void printAll(map<string, vector<string>> res);
};

