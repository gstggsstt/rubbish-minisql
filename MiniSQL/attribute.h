#pragma once
#include "minisql.h"
#include <string>

using namespace std;

#define ATTR_TYPE_INT 0
#define ATTR_TYPE_FLOAT -1
#define ATTR_TYPE_DOUBLE -2
#define ATTR_TYPE_UNDEF -3
// positive type refers to the length of char array.

class attribute
{
	public:
	string name;
	int type;
	bool notNull;
	bool primaryKey;
	bool unique;
	string index;

	attribute(string nm="", int tp=ATTR_TYPE_INT, bool nN=false, bool pr=false, bool unq=false,string idx="*NULL*");
};
