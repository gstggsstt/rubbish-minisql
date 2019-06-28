#pragma once
#include "table.h"
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <cstdlib>
#include <stack>

using namespace std;

class valueComparison
{
public:
	string val1, val2;
	string op;
	valueComparison(string v1,string v2,string o):val1(v1),val2(v2),op(o){}
};
class logicalComparison
{
public:
	string op;
	logicalComparison(string o):op(o){}
};

// example1:  select * from tb where attr between 1 and 10
//   =>  ( {  { attr,1,> } , { attr,10,< }  } , { {"",false}, { "A",true }, {"",false} } )
// example2:  select * from tb where A>1 || B<2 && C==3
//   =>  ( {  { A,1,> } , { B,2,< } , { C,3,== }  } , {  { "",false } , { "O",false } , { "A",false } , { "",false }  } )

typedef pair<vector<valueComparison>,vector<logicalComparison> > condition;
typedef map<string,string> record;

const double EPS = 1e-6;

class conditionChecker
{
	int precedence(char op);
	int applyOp(int a, int b, char op);

	public:

	// evaluate a bool expression like "0A(1O0)A0O1"
	int evaluate(string tokens);

	// get the type of str, result could be ATTR_TYPE_INT ect.(defined in attribute.h)
	// if a string is not a number neither surrounded with single quotations, then throw a string 
	//   which contains error info
	int getType(string str);

	// check whether a cond is true or not
	bool check(table & T, record res, condition & cond);

	template<typename _tp1,typename _tp2>
	bool compWithoutEps(_tp1 v1, _tp2 v2, string op)
	{
		if(op=="=" || op=="==") return v1==v2;
		if(op=="<") return v1<v2;
		if(op==">") return v1>v2;
		if(op=="<=") return v1<=v2;
		if(op==">=") return v1>=v2;
		if(op=="<>" || op=="!=") return v1!=v2;
		throw string("Unknown operator \""+op+"\".");
	}
	template<typename _tp1,typename _tp2>
	bool compWithEps(_tp1 v1, _tp2 v2, string op)
	{
		if(op=="=" || op=="==") return fabs(v1-v2)<(v1+v2)*EPS;
		if(op=="<") return v1<v2-v2*EPS;
		if(op==">") return v1>v2+v2*EPS;
		if(op=="<=") return v1<=v2+v2*EPS;
		if(op==">=") return v1>=v2-v2*EPS;
		if(op=="<>" || op=="!=") return fabs(v1-v2)>(v1+v2)*EPS;
		throw string("Unknown operator \""+op+"\".");
	}
};
