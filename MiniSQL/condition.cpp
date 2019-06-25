#include "pch.h"
#include "condition.h"

int conditionChecker::precedence(char op)
{
	if (op == 'O')
		return 1;
	if (op == 'A')
		return 2;
	return 0;
}

int conditionChecker::applyOp(int a, int b, char op)
{
	if (op == 'A')
		return a & b;
	return a | b;
}

int conditionChecker::evaluate(string tokens)
{
	int i;
	stack<int> values;
	stack<char> ops;
	for (i = 0; i < tokens.length(); i++)
	{
		if (tokens[i] == ' ')
			continue;
		else if (tokens[i] == '(')
			ops.push(tokens[i]);
		else if (isdigit(tokens[i]))
		{
			int val = 0;
			while (i < tokens.length() && isdigit(tokens[i]))
			{
				val = (val * 10) + (tokens[i] - '0');
				i++;
			}
			i--;
			values.push(val);
		}

		else if (tokens[i] == ')')
		{
			while (!ops.empty() && ops.top() != '(')
			{
				int val2 = values.top();
				values.pop();
				int val1 = values.top();
				values.pop();
				char op = ops.top();
				ops.pop();
				values.push(applyOp(val1, val2, op));
			}
			ops.pop();
		}
		else
		{
			while (!ops.empty() && precedence(ops.top()) >= precedence(tokens[i]))
			{
				int val2 = values.top();
				values.pop();
				int val1 = values.top();
				values.pop();
				char op = ops.top();
				ops.pop();
				values.push(applyOp(val1, val2, op));
			}
			ops.push(tokens[i]);
		}
	}

	while (!ops.empty())
	{
		int val2 = values.top();
		values.pop();
		int val1 = values.top();
		values.pop();
		char op = ops.top();
		ops.pop();
		values.push(applyOp(val1, val2, op));
	}

	return values.top();
}

int conditionChecker::getType(string str)
{
	if (regex_match(str.begin(), str.end(), regex("\'(\\s|\\S)*\'")))
		return (int)str.length() - 2;
	if (regex_match(str.begin(), str.end(), regex("[+-]?\\d+\\.\\d+")))
		return -2;
	if (regex_match(str.begin(), str.end(), regex("[+-]?\\d+")))
		return 0;

	throw string("Can't determin the datatype of \""+str+"\" (check single quotation arround string).");
}

bool conditionChecker::check(table &T, record res, condition &cond)
{
	vector<bool> vec;
	string v1, v2;
	int t1, t2;
	for (auto it : cond.first)
	{
		if (res.count(it.val1))
			v1 = res[it.val1], t1 = T[it.val1].type;
		else
			v1 = it.val1, t1 = getType(v1);
		if (res.count(it.val2))
			v2 = res[it.val2], t2 = T[it.val2].type;
		else
			v2 = it.val2, t2 = getType(v2);
		if ((t1 <= 0) != (t2 <= 0))
			throw string("Comparison between different datatype.");
		if (t1 > 0 && t2 > 0)
			vec.push_back(comp(v1, v2, it.op));
		else if (t1 < 0 || t2 < 0)
			vec.push_back(comp(atof(v1.c_str()), atof(v2.c_str()), it.op));
		else
			vec.push_back(comp(atoi(v1.c_str()), atoi(v2.c_str()), it.op));
	}
	string expr;
	for (int i = 0; i < (int)vec.size(); ++i)
	{
		expr += cond.second[i].op;
		expr += (vec[i] ? "1" : "0");
	}
	expr += cond.second.back().op;
	return evaluate(expr);
}
