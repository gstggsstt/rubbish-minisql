#include "pch.h"
#include "Spliter.h"


Spliter::Spliter(string S)
{
	str = S;
	cur = 0;
}


Spliter::~Spliter()
{
}

string Spliter::getWord()
{
	string temp;
	int cnt = 0;
	while (cur < str.length() &&
		!(str[cur] >= 'A' && str[cur] <= 'Z' ||
			str[cur] >= 'a' && str[cur] <= 'z' ||
			str[cur] >= '0' && str[cur] <= '9' ||
			str[cur] == '_' || str[cur] == '.' ||
			str[cur] == '-' || str[cur] == '\'' ||
			str[cur] == '*' || str[cur] == ';' ||
			str[cur] == '#')) cur++;
	while (cur < str.length() &&
		((str[cur] >= 'A' && str[cur] <= 'Z' ||
			str[cur] >= 'a' && str[cur] <= 'z' ||
			str[cur] >= '0' && str[cur] <= '9' ||
			str[cur] == '_' || str[cur] == '.' ||
			str[cur] == '-' || str[cur] == '\'' ||
			str[cur] == '*' || str[cur] == '#') || cnt))
	{
		if (str[cur] == '\'') cnt ^= 1;
		temp += str[cur++];
	}
	return temp;
}

string trim(string s)
{
	int i = 0; while (s[i] == ' ') i++;
	int j = s.length() - 1; while (s[j] == ' ') j--;
	return s.substr(i, j - i + 1);
}

string Spliter::getBrackets()
{
	string temp;
	int st = 0;
	int cnt = 0;
	while (cur < str.length() && str[cur] != '(') cur++;
	do
	{
		if (str[cur] == '(' && !cnt) st++;
		if (str[cur] == ')' && !cnt) st--;
		if (str[cur] == '\'') cnt ^= 1;
		temp += str[cur++];
	} while (cur < str.length() && st);
	return trim(temp.substr(1, temp.length() - 2));
}

vector<string> splitComma(string S)
{
	vector<string> vec;
	int t = 0;
	while (t != string::npos)
	{
		int temp = S.find_first_of(',', t);
		vec.push_back(S.substr(t, temp-t));
		if(temp!=string::npos) t = temp + 1;
		else t = string::npos;
	}
	return vec;
}

bool Spliter::getSemicolon()
{
	while (cur < str.length() && str[cur] != ';') cur++;
	if (cur < str.length()) { cur++; return true; }
	return false;
}

string Spliter::getOp()
{
	string temp;
	while (cur < str.length() && str[cur] != '<' && str[cur] != '>' &&
		str[cur] != '=') cur++;
	while (cur < str.length() && !(str[cur] != '<' && str[cur] != '>' &&
		str[cur] != '=')) temp += str[cur++];
	return temp;
}

string Spliter::getUntilSemicolon()
{
	string temp;
	while (cur < str.length() && str[cur] != ';') temp += str[cur++];
	cur--;
	return temp;
}

string Spliter::getUntilWord()
{
	string temp;
	while (cur < str.length() &&
		(str[cur] >= 'A' && str[cur] <= 'Z' ||
			str[cur] >= 'a' && str[cur] <= 'z' ||
			str[cur] >= '0' && str[cur] <= '9' ||
			str[cur] == '_' || str[cur] == '.' ||
			str[cur] == '-' || str[cur] == '\'' ||
			str[cur] == '*' || str[cur] == '#')) temp += str[cur++];
	return temp;
}

bool Spliter::end()
{
	return cur == str.length();
}
