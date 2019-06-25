#include "pch.h"
#include "interpreter.h"


interpreter::interpreter() { }
interpreter::~interpreter() { }
vector<string> interpreter::splitWords(string s)
{
	vector<string> words;
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	string tmp;
	tmp.clear();
	for (int i = 0; i < s.length(); i++)
	{
		if ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= '0' && s[i] <= '9') || s[i] == '.' || s[i] == '\'')
			tmp.push_back(s[i]);
		else if (s[i] == '*')
		{
			if (words.back() == "select")
			{
				tmp.push_back(s[i]);
				words.push_back(tmp);
				//				cout<<tmp<<endl;
				tmp.clear();
			}
			else
				tmp.push_back(s[i]);
		}
		else if (s[i] == '>' || s[i] == '<' || s[i] == '=')
		{
			if (s[i] == '<')
			{
				words.push_back(tmp);
				tmp.clear();
				tmp.push_back(s[i]);
				words.push_back(tmp);
				tmp.clear();
			}
			else if (s[i] == '>')
			{
				if (words.back() == "<")
					words[words.size() - 1].push_back('>');
				else
				{
					words.push_back(tmp);
					tmp.clear();
					tmp.push_back(s[i]);
					words.push_back(tmp);
					tmp.clear();
				}
			}
			else
			{
				if (words.back() == "<" || words.back() == ">")
					words[words.size() - 1].push_back('=');
				else
				{
					words.push_back(tmp);
					tmp.clear();
					tmp.push_back(s[i]);
					words.push_back(tmp);
					tmp.clear();
				}
			}
		}
		else
		{
			if (!tmp.empty())
			{
				words.push_back(tmp);
				//				cout<<tmp<<endl;
				tmp.clear();
			}
		}
	}
	if (!tmp.empty())
	{
		words.push_back(tmp);
		//		cout<<tmp<<endl;
		tmp.clear();
	}
	return words;
}

map<string, vector<string>> interpreter::execute(string cmd)
{
	int pos = 0;
	map<string, vector<string>> res;
	vector<string> words = splitWords(cmd);
	vector<valueComparison> vC;
	vector<logicalComparison> lC;
	vector<string> attrName, value(3), logic(3);
	string tableName, name, indexName;
	map<string, string> A;
	int type, primary;
	if (words[pos] == "create")
	{
		vector<attribute> attr;
		pos++;
		if (words[pos] == "table")
		{
			string tmp;
			words.clear();
			transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
			for (int k = 0; k < cmd.length(); k++)
			{
				if ((cmd[k] >= 'a'&&cmd[k] <= 'z') || (cmd[k] >= '0'&&cmd[k] <= '9') || cmd[k] == '.' || cmd[k] == '\'')
					tmp.push_back(cmd[k]);
				else if (cmd[k] == ',')
				{
					if (!tmp.empty())
					{
						words.push_back(tmp);
						tmp.clear();
						tmp.push_back(',');
						words.push_back(tmp);
						tmp.clear();
					}
					else
					{
						tmp.push_back(',');
						words.push_back(tmp);
						tmp.clear();
					}
				}
				else
				{
					if (!tmp.empty())
					{
						words.push_back(tmp);
						tmp.clear();
					}
				}
			}
			if (!tmp.empty())
			{
				words.push_back(tmp);
				tmp.clear();
			}
			pos = 2;
			tableName = words[pos];
			pos++;
			while (pos < words.size())
			{
				if (words[pos] == "primary")
				{
					pos += 2;
					for (int j = 0; j < attr.size(); j++)
					{
						if (attr[j].name == words[pos])
						{
							attr[j].primaryKey = 1;
							break;
						}
					}
					pos++;
				}
				else
				{
					int j = 0;
					while (pos < words.size() && words[pos] != ",")
					{
						pos++;
						j++;
					}
					int tmpType;
					string tmpName = words[pos - j];
					j--;
					if (words[pos - j] == "int") tmpType = ATTR_TYPE_INT, j--;
					else if (words[pos - j] == "float") tmpType = ATTR_TYPE_FLOAT, j--;
					else if (words[pos - j] == "double") tmpType = ATTR_TYPE_DOUBLE, j--;
					else
					{
						j--;
						tmpType = stoi(words[pos - j]);
						j--;
					}
					j--;
					int tmpPrimary = 0;
					int tmpNotNull = 0;
					while (j > 0)
					{
						if (words[pos - j] == "unique")
						{
							tmpPrimary = 1;
							j--;
						}
						else if (words[pos - j] == "not")
						{
							j--;
							tmpNotNull = 1;
							j--;
						}
					}
					attr.push_back(attribute(tmpName, tmpType, tmpNotNull, tmpPrimary));
					pos++;
				}
			}
			sr.createTable(tableName, attr);
		}
		else if (words[pos] == "index")
		{
			pos++;
			if (pos >= words.size())
			{
				cout << "Error 1064(42000): You have an error in your sql syntax\n";
				return res;
			}
			else
				indexName = words[pos];
			pos++;
			if (pos >= words.size() || words[pos] != "on")
			{
				cout << "Error 1064(42000): You have an error in your sql syntax\n";
				return res;
			}
			pos++;
			if (pos >= words.size())
			{
				cout << "Error 1064(42000): You have an error in your sql syntax\n";
				return res;
			}
			else
				tableName = words[pos];
			pos++;
			if (pos >= words.size())
			{
				cout << "Error 1064(42000): You have an error in your sql syntax\n";
				return res;
			}
			else
				name = words[pos];
			sr.createIndex(tableName, name, indexName);
		}
		else
		{
			cout << "Error 1064(42000): You have an error in your sql syntax\n";
			return res;
		}
	}
	else if (words[pos] == "select")
	{
		pos++;
		while (pos < words.size() && words[pos] != "from")
			attrName.push_back(words[pos++]);
		if (pos >= words.size())
		{
			cout << "Error 1064(42000): You have an error in your sql syntax(no keyword from)\n";
			return res;
		}
		else
		{
			pos++;
			if (pos >= words.size())
			{
				cout << "Error 1064(42000): You have an error in your sql syntax\n";
				return res;
			}
			else
				tableName = words[pos];
			pos++;
			if (words[pos] != "where")
			{
				cout << "Error 1064(42000): You have an error in your sql syntax(no keyword where)\n";
				return res;
			}
			else
			{
				pos++;
				value[0] = words[pos];
				pos++;
				if (words[pos] == "between")
				{
					vC.push_back(valueComparison(value[0], words[pos + 1], ">"));
					vC.push_back(valueComparison(value[0], words[pos + 3], "<"));
					lC.push_back(logicalComparison("", false));
					lC.push_back(logicalComparison("A", true));
					lC.push_back(logicalComparison("", false));
				}
				else
				{
					value[1] = words[pos++];
					value[2] = words[pos++];
					vC.push_back(valueComparison(value[0], value[2], value[1]));
					value.clear();
					if (pos < words.size())
					{
						if (words[pos] == "and")
						{
							lC.push_back(logicalComparison("", false));
							lC.push_back(logicalComparison("A", false));
							lC.push_back(logicalComparison("", false));
							pos++;
							value[0] = words[pos];
							value[1] = words[pos++];
							value[2] = words[pos++];
							vC.push_back(valueComparison(value[0], value[2], value[1]));
						}
						else if (words[pos] == "or")
						{
							lC.push_back(logicalComparison("", false));
							lC.push_back(logicalComparison("O", false));
							lC.push_back(logicalComparison("", false));
							pos++;
							value[0] = words[pos];
							value[1] = words[pos++];
							value[2] = words[pos++];
							vC.push_back(valueComparison(value[0], value[2], value[1]));
						}
					}
				}
			}
		}
		condition con = make_pair(vC, lC);
		res = sr.selectRecord(tableName, attrName, con);
	}
	else if (words[pos] == "insert")
	{
		pos++;
		if (words[pos] == "into")
		{
			if (!words[++pos].empty())
			{
				int i = 0;
				tableName = words[pos];
				table tab = sr.getTable(tableName);
				pos++;
				while (words[pos] != "values" && pos < words.size())
				{
					pos++;
					i++;
				}
				pos++;
				if (pos >= words.size())
				{
					cout << "Error 1064(42000): You have an error in your sql syntax(No keyword values)\n";
					return res;
				}
				else
				{
					if (i > 0)
					{
						for (int j = 0; j < i; j++)
						{
							A[words[pos + j]] = words[pos - i + j];
						}
					}
					else
					{
						for (int j = pos; j < words.size(); j++)
						{
							A[tab.Attr[j - pos].name] = words[j];
						}
					}
				}
			}
		}
		else
		{
			cout << "Error 1064(42000): You have an error in your sql syntax\n";
			return res;
		}
		sr.insertRecord(tableName, A);
	}
	else if (words[pos] == "delete")
	{
		pos++;
		if (words[pos] != "from")
		{
			cout << "Error 1064(42000): You have an error in your sql syntax(no keyword from)\n";
			return res;
		}
		else
		{
			pos++;
			tableName = words[pos];
			pos++;
			if (words[pos] != "where")
			{
				cout << "Error 1064(42000): You have an error in your sql syntax\n";
				return res;
			}
			else
			{
				value[1] = words[pos++];
				value[2] = words[pos++];
				vC.push_back(valueComparison(value[0], value[2], value[1]));
				value.clear();
				if (pos < words.size())
				{
					if (words[pos] == "and")
					{
						lC.push_back(logicalComparison("", false));
						lC.push_back(logicalComparison("A", false));
						lC.push_back(logicalComparison("", false));
						pos++;
						value[0] = words[pos];
						value[1] = words[pos++];
						value[2] = words[pos++];
						vC.push_back(valueComparison(value[0], value[2], value[1]));
					}
					else if (words[pos] == "or")
					{
						lC.push_back(logicalComparison("", false));
						lC.push_back(logicalComparison("O", false));
						lC.push_back(logicalComparison("", false));
						pos++;
						value[0] = words[pos];
						value[1] = words[pos++];
						value[2] = words[pos++];
						vC.push_back(valueComparison(value[0], value[2], value[1]));
					}
				}
			}
			condition con = make_pair(vC, lC);
			sr.selectRecord(tableName, attrName, con);
		}
	}
	else if (words[pos] == "drop")
	{
		pos++;
		if (words[pos] == "table")
		{
			pos++;
			if (!words[pos].empty())
			{
				tableName = words[pos];
				sr.dropTable(tableName);
			}
			else
				cout << "Error 1064(42000): You have an error in your sql syntax(No table name)\n";
		}
		else if (words[pos] == "index")
		{
			pos++;
			if (!words[pos].empty())
			{
				indexName = words[pos];
				sr.dropIndex(indexName);
			}
			else
				cout << "Error 1064(42000): You have an error in your sql syntax(No table name)\n";
		}
	}
	else
		cout << "Error 1064(42000): You have an error in your sql syntax\n";
	return res;
}
