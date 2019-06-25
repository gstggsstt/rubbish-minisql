#include "pch.h"
#include "interpreter.h"
#include "Spliter.h"
#include <iostream>
#include <iomanip>


interpreter::interpreter() { }
interpreter::~interpreter() { }

void interpreter::flush() { sr.flush(); }

map<string, vector<string>> interpreter::execute(string cmd, int printOrNot)
{
	map<string, vector<string>> res;

	transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

	Spliter Sp(cmd);
	string wd = Sp.getWord();
	if (wd == "create")
	{
		wd = Sp.getWord();
		if (wd == "table")
		{
			vector<attribute> Attr;
			string tableName = Sp.getWord();
			string primary = "";
			if (tableName == "")
			{
				cerr << "Syntax error: Table name required" << endl;
				res["#Error"].push_back("Table name required");
				return res;
			}
			string pars = Sp.getBrackets();
			if (pars == "")
			{
				cerr << "Syntax error: Attributes required" << endl;
				res["#Error"].push_back("Attributes required");
				return res;
			}
			vector<string> attrAndType = splitComma(pars);
			for (auto it : attrAndType)
			{
				if (it == "")
				{
					cerr << "Syntax error: Syntax error arround ',' " << endl;
					res["#Error"].push_back("Syntax error arround ',' ");
					return res;
				}
				Spliter tempSp(it);
				string attrName = tempSp.getWord();
				if (attrName == "primary")
				{
					if (primary != "")
					{
						cerr << "Syntax error: Multi primary keys" << endl;
						res["#Error"].push_back("Multi primary keys");
						return res;
					}
					wd = tempSp.getWord();
					if (wd != "key")
					{
						cerr << "Syntax error: Unknown specifier \"" << wd << "\"" << endl;
						res["#Error"].push_back("Unknown specifier \"" + wd + "\"");
						return res;
					}
					primary = tempSp.getBrackets();
					break;
				}
				string attrType = tempSp.getWord();
				int type;
				if (attrType == "char")
				{
					try { type = stoi(tempSp.getBrackets()) + 3; }
					catch (...)
					{
						cerr << "Syntax error: Syntax error arround \"char\"" << endl;
						res["#Error"].push_back("Syntax error arround \"char\"");
						return res;
					}
				}
				else if (attrType == "int") type = ATTR_TYPE_INT;
				else if (attrType == "float") type = ATTR_TYPE_FLOAT;
				else if (attrType == "double") type = ATTR_TYPE_DOUBLE;
				else
				{
					cerr << "Syntax error: Unknown attribute type \"" + attrType + "\"" << endl;
					res["#Error"].push_back("Unknown attribute type \"" + attrType + "\"");
					return res;
				}
				bool notNull = false;
				bool unique = false;
				while ((wd = tempSp.getWord()) != "")
				{
					if (wd == "notnull") notNull = true;
					else if (wd == "unique") unique = true;
					else if (wd == "primary")
					{
						if (primary != "")
						{
							cerr << "Syntax error: Multi primary keys" << endl;
							res["#Error"].push_back("Multi primary keys");
							return res;
						}
						primary = wd;
					}
					else
					{
						cerr << "Syntax error: Unknown specifier \"" << wd << "\"" << endl;
						res["#Error"].push_back("Unknown specifier \"" + wd + "\"");
						return res;
					}
				}
				Attr.push_back(attribute(attrName, type, notNull, false, unique));
			}
			set<string> checkName;
			for (int i = 0; i < Attr.size(); ++i)
			{
				attribute & it = Attr[i];
				if (it.name == primary)
					it.primaryKey = true, it.notNull = true, it.unique = true;
				if (!checkName.count(it.name)) checkName.insert(it.name);
				else
				{
					cerr << "Syntax error: Multidefined attribute \"" << wd << "\"" << endl;
					res["#Error"].push_back("Multidefined attribute \"" + wd + "\"");
					return res;
				}
			}
			if (!Sp.getSemicolon())
			{
				cerr << "Syntax error: Expected ';'" << endl;
				res["#Error"].push_back("Expected ';'");
				return res;
			}
			clock_t startTime = clock();
			res = sr.createTable(tableName, Attr);
			if (primary != "") sr.createIndex(tableName, primary, primary + "_default_idx");
			res["#time"].push_back(to_string((int)((clock() - startTime)*1000.0 / CLOCKS_PER_SEC)) + " ms");
			if (printOrNot == 1 || res.count("#Error")) printAll(res);
			return res;
		}
		if (wd == "index")
		{
			string indexName = Sp.getWord();
			wd = Sp.getWord();
			if (wd != "on")
			{
				cerr << "Syntax error: \"ON\" expected" << endl;
				res["#Error"].push_back("\"ON\" expected");
				return res;
			}
			string tableName = Sp.getWord();
			string attrName = Sp.getBrackets();
			clock_t startTime = clock();
			res = sr.createIndex(tableName, attrName, indexName);
			res["#time"].push_back(to_string((int)((clock() - startTime)*1000.0 / CLOCKS_PER_SEC)) + " ms");
			if (printOrNot == 1 || res.count("#Error")) printAll(res);
			return res;
		}
	}
	if (wd == "insert")
	{
		string tableName;
		wd = Sp.getWord();
		if (wd != "into")
		{
			cerr << "Syntax error: 'INTO' expected" << endl;
			res["#Error"].push_back("'INTO' expected");
			return res;
		}

		tableName = Sp.getWord();
		wd = Sp.getWord();
		if (wd != "values")
		{
			cerr << "Syntax error: 'VALUES' expected" << endl;
			res["#Error"].push_back("'VALUES' expected");
			return res;
		}
		wd = Sp.getBrackets();
		int cnt = 0;
		bool noAttrName = false;
		bool hasAttrName = false;
		table T = sr.getTable(tableName);
		vector<string> values = splitComma(wd);
		map<string, string> val;
		for (auto it : values)
		{
			Spliter tempSp(it);
			string str1 = tempSp.getWord();
			string str2 = tempSp.getWord();
			if (str2 == "") noAttrName = true;
			else hasAttrName = true;
			if (noAttrName && hasAttrName)
			{
				cerr << "Syntax error: Attribute name of value(s) is required." << endl;
				res["#Error"].push_back("Attribute name of value(s) is not required.");
				return res;
			}
			if (noAttrName) val[T.Attr[cnt++].name] = str1;
			else
			{
				if (T.AttrMap.count(str1) == 0)
				{
					cerr << "Syntax error: Attribute \"" + str1 + "\" is not in table \"" + tableName + "\"." << endl;
					res["#Error"].push_back("Attribute \"" + str1 + "\" is not in table \"" + tableName + "\".");
					return res;
				}
				val[str1] = str2;
			}
		}
		if (!Sp.getSemicolon())
		{
			cerr << "Syntax error: Expected ';'" << endl;
			res["#Error"].push_back("Expected ';'");
			return res;
		}
		clock_t startTime = clock();
		res = sr.insertRecord(tableName, val);
		res["#time"].push_back(to_string((int)((clock() - startTime)*1000.0 / CLOCKS_PER_SEC)) + " ms");
		if (printOrNot == 1 || res.count("#Error")) printAll(res);
		return res;
	}
	if (wd == "select")
	{
		vector<string> Attr;
		do
		{
			wd = Sp.getWord();
			if (wd != "from") Attr.push_back(wd);
		} while (wd != "from" && wd != "");
		if (wd == "")
		{
			cerr << "Syntax error: \"FROM\" is expected" << endl;
			res["#Error"].push_back("\"FROM\" is expected");
			return res;
		}
		string tableName = Sp.getWord();
		vector<valueComparison> vC;
		vector<logicalComparison> lC;
		wd = Sp.getWord();
		if (wd == "where")
		{
			string condStr = Sp.getUntilSemicolon();
			if (condStr.find("between") == string::npos)
			{
				Spliter tempSp(condStr);
				lC.push_back(logicalComparison(tempSp.getUntilWord()));
				while (!tempSp.end())
				{
					string str1, str2, str3;
					str1 = tempSp.getWord();
					if (str1 == "or" || str1 == "and")
					{
						if (str1 == "and") lC.back().op += "A";
						if (str1 == "or") lC.back().op += "O";
						lC.back().op += tempSp.getUntilWord();
						continue;
					}
					str2 = tempSp.getOp();
					str3 = tempSp.getWord();
					vC.push_back(valueComparison(str1, str3, str2));
					lC.push_back(logicalComparison(tempSp.getUntilWord()));
				}
			}
			else
			{
				Spliter tempSp(condStr);
				wd = tempSp.getWord();
				string bw = tempSp.getWord();
				if (bw != "between")
				{
					cerr << "Syntax error: Syntax error arround \"BETWEEN\"" << endl;
					res["#Error"].push_back("Syntax error arround \"BETWEEN\"");
					return res;
				}
				string str1 = tempSp.getWord();
				string andStr = tempSp.getWord();
				if (andStr != "and")
				{
					cerr << "Syntax error: Syntax error, \"AND\" expected" << endl;
					res["#Error"].push_back("Syntax error, \"AND\" expected");
					return res;
				}
				string str2 = tempSp.getWord();
				lC.push_back(logicalComparison(""));
				lC.push_back(logicalComparison("A"));
				lC.push_back(logicalComparison(""));
				vC.push_back(valueComparison(wd, str1, ">="));
				vC.push_back(valueComparison(wd, str2, "<="));
			}
		}
		if (!Sp.getSemicolon())
		{
			cerr << "Syntax error: Expected ';'" << endl;
			res["#Error"].push_back("Expected ';'");
			return res;
		}
		if (Attr.size() == 0)
		{
			cerr << "Syntax error: No attribute is selected." << endl;
			res["#Error"].push_back("Syntax error: No attribute is selected.");
			return res;
		}
		clock_t startTime = clock();
		res = sr.selectRecord(tableName, Attr, make_pair(vC, lC));
		res["#time"].push_back(to_string((int)((clock() - startTime)*1000.0 / CLOCKS_PER_SEC)) + " ms");
		if ((printOrNot == 1 || printOrNot == -1)) printAll(res);
		return res;
	}
	if (wd == "delete")
	{
		vector<string> Attr;
		wd = Sp.getWord();
		if (wd != "from")
		{
			cerr << "Syntax error: \"FROM\" is expected" << endl;
			res["#Error"].push_back("\"FROM\" is expected");
			return res;
		}
		string tableName = Sp.getWord();
		vector<valueComparison> vC;
		vector<logicalComparison> lC;
		wd = Sp.getWord();
		if (wd == "where")
		{
			string condStr = Sp.getUntilSemicolon();
			if (condStr.find("between") == string::npos)
			{
				Spliter tempSp(condStr);
				lC.push_back(logicalComparison(tempSp.getUntilWord()));
				while (!tempSp.end())
				{
					string str1, str2, str3;
					str1 = tempSp.getWord();
					if (str1 == "or" || str1 == "and")
					{
						if (str1 == "and") lC.back().op += "A";
						if (str1 == "or") lC.back().op += "O";
						lC.back().op += tempSp.getUntilWord();
						continue;
					}
					str2 = tempSp.getOp();
					str3 = tempSp.getWord();
					vC.push_back(valueComparison(str1, str3, str2));
					lC.push_back(logicalComparison(tempSp.getUntilWord()));
				}
			}
			else
			{
				Spliter tempSp(condStr);
				wd = tempSp.getWord();
				string bw = tempSp.getWord();
				if (bw != "between")
				{
					cerr << "Syntax error: Syntax error arround \"BETWEEN\"" << endl;
					res["#Error"].push_back("Syntax error arround \"BETWEEN\"");
					return res;
				}
				string str1 = tempSp.getWord();
				string str2 = tempSp.getWord();
				lC.push_back(logicalComparison(""));
				lC.push_back(logicalComparison("A"));
				lC.push_back(logicalComparison(""));
				vC.push_back(valueComparison(wd, str1, ">="));
				vC.push_back(valueComparison(wd, str2, "<="));
			}
		}
		if (!Sp.getSemicolon())
		{
			cerr << "Syntax error: Expected ';'" << endl;
			res["#Error"].push_back("Expected ';'");
			return res;
		}
		clock_t startTime = clock();
		res = sr.deleteRecord(tableName, make_pair(vC, lC));
		res["#time"].push_back(to_string((int)((clock() - startTime)*1000.0 / CLOCKS_PER_SEC)) + " ms");
		if (printOrNot == 1 || res.count("#Error")) printAll(res);
		return res;
	}
	if (wd == "drop")
	{
		string wd = Sp.getWord();
		if (wd == "table")
		{
			string tableName = Sp.getWord();
			if (!Sp.getSemicolon())
			{
				cerr << "Syntax error: Expected ';'" << endl;
				res["#Error"].push_back("Expected ';'");
				return res;
			}
			clock_t startTime = clock();
			res = sr.dropTable(tableName);
			res["#time"].push_back(to_string((int)((clock() - startTime)*1000.0 / CLOCKS_PER_SEC)) + " ms");
			if (printOrNot == 1 || res.count("#Error")) printAll(res);
			return res;
		}
		if (wd == "index")
		{
			string indexName = Sp.getWord();
			if (!Sp.getSemicolon())
			{
				cerr << "Syntax error: Expected ';'" << endl;
				res["#Error"].push_back("Expected ';'");
				return res;
			}
			clock_t startTime = clock();
			res = sr.dropIndex(indexName);
			res["#time"].push_back(to_string((int)((clock() - startTime)*1000.0 / CLOCKS_PER_SEC)) + " ms");
			if (printOrNot == 1 || res.count("#Error")) printAll(res);
			return res;
		}
	}
	if (wd == "execfile")
	{
		clock_t startTime = clock();
		string fileName = Sp.getWord();
		ifstream fin(fileName);
		if (!Sp.getSemicolon())
		{
			cerr << "Syntax error: Expected ';'" << endl;
			res["#Error"].push_back("Expected ';'");
			return res;
		}
		if (!fin.good())
		{
			cerr << "Syntax error: File \""+fileName+"\" does not exist." << endl;
			res["#Error"].push_back("Syntax error: File \""+fileName+"\" does not exist.");
			return res;
		}
		int l = 0;
		while (!fin.eof())
		{
			string str;
			getline(fin, str);
			cout << "Line " << l++ << ": " << str << endl;
			execute(str);
		}
		res["#time"].push_back(to_string((int)((clock() - startTime)*1000.0 / CLOCKS_PER_SEC)) + " ms");
		return res;
	}
	cerr << "Syntax error: unknown command." << endl;
	res["#Error"].push_back("Unknown command.");
	return res;
}

void interpreter::printLine(vector<int> & width)
{
	cout << '+';
	for (auto it : width)
		cout << string(it, '-') << '+';
	cout << endl;
}

void interpreter::printRecord(map<string, vector<string>> &res,
	vector<string> & Attrs, vector<int> & width, int x)
{
	cout << '|';
	for (int i = 0; i < (int)Attrs.size(); ++i)
		cout << setw(width[i]) << left << res[Attrs[i]][x] << '|';
	cout << endl;
}

void interpreter::printAll(map<string, vector<string>> res)
{
	vector<string> Attrs;
	vector<string> Status;
	vector<int> width;
	for (auto it : res)
		if (it.first[0] != '#') Attrs.push_back(it.first);
		else Status.push_back(it.first);
	for (auto it : Attrs)
	{
		int Max = 0;
		auto& val = res[it];
		for (auto e : val)
			Max = max((int)e.length(), Max);
		width.push_back(max(Max, (int)it.length()));
	}
	if (res.count("#SelectFlag") && res.count("#Error") == 0)
	{
		if (Attrs.size())
		{
			printLine(width);
			cout << '|';
			for (int i = 0; i < Attrs.size(); ++i)
				cout << setw(width[i]) << left << Attrs[i] << '|';
			cout << endl;
			printLine(width);
			int len = res[Attrs[0]].size();
			for (int i = 0; i < len; ++i)
				printRecord(res, Attrs, width, i), printLine(width);

			cout << len << " row(s) selected. " << endl;
		}
		else
		{
			cout << "+---+\n|N/A|\n+---+\n|N/A|\n+---+" << endl;
			cout << 0 << " row(s) selected. " << endl;
		}
	}

	for (auto it : Status)
	{
		for (auto jt : res[it])
			cout << it << ":  " << jt << endl;
	}
	cout << endl;
}
