#pragma once
#include <string>
#include <vector>
using namespace std;
class Spliter
{
	string str;
	int cur;
public:
	Spliter(string S);
	~Spliter();
	string getWord();
	string getBrackets();
	bool getSemicolon();
	string getOp();
	string getUntilSemicolon();
	string getUntilWord();
	bool end();
};

vector<string> splitComma(string S);

