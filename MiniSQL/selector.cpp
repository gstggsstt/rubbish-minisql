#include "pch.h"
#include "selector.h"

selector::RES selector::createIndex(string tableName, string Attr, string idx)
{
	map<string, vector<string>> r;
	try
	{
		pair<string, string> temp = cm.getAttrByIndex(idx);
		if(temp.first!="*NULL*")
		{
			r["#Error"].push_back("Index \""+idx+"\" has already existed");
			r["#Status"].push_back("failed");
			return r;
		}
		table T = cm.getTable(tableName);
		if (T.AttrMap.count(Attr) == 0)
		{
			r["#Error"].push_back("Unknown attribute \""+Attr+"\" on table \""+tableName+"\"");
			r["#Status"].push_back("failed");
			return r;
		}
		if (T[Attr].index != "*NULL*")
		{
			r["#Error"].push_back("Index on \"" + Attr + "\" has already been created.");
			r["#Status"].push_back("failed");
			return r;
		}
		if (!T[Attr].unique)
		{
			r["#Error"].push_back("Creating index on non-unique attribute \""+Attr+"\".");
			r["#Status"].push_back("failed");
			return r;
		}
		im.createIndex(cm.getTable(tableName), Attr, idx);
	}
	catch (string Excpt)
	{
		r["#Error"].push_back(Excpt);
		r["#Status"].push_back("failed");
		return r;
	}
	catch (...)
	{
		r["#Error"].push_back("Unknown interal error, please contact us.");
		r["#Status"].push_back("failed");
		return r;
	}
	r["#Status"].push_back("successed");
	return r;
}
selector::RES selector::dropIndex(string idx)
{
	map<string, vector<string>> r;
	try
	{
		im.dropIndex(idx);
		dropFileList.push_back(idx + ".index");
	}
	catch (string Excpt)
	{
		r["#Error"].push_back(Excpt);
		r["#Status"].push_back("failed");
		return r;
	}
	catch (...)
	{
		r["#Error"].push_back("Unknown interal error, please contact us.");
		r["#Status"].push_back("failed");
		return r;
	}
	r["#Status"].push_back("successed");
	return r;
}

selector::RES selector::selectRecord(string tableName, vector<string> Attr, condition cond)
{
	map<string, vector<string>> r;
	try
	{
		table T = cm.getTable(tableName);
		if (T.name == "*NULL*")
		{
			r["#Error"].push_back("Table \"" + tableName + "\" is not found");
			r["#Status"].push_back("failed");
			return r;
		}
		set<string> selectedAttr;
		if (Attr[0] != "*")
		{
			for (auto A : Attr)
			{
				if (T.AttrMap.count(A) == 0)
				{
					r["#Error"].push_back("Unknown attribute \"" + A + "\" on table\"" + tableName + "\"");
					r["#Status"].push_back("failed");
					return r;
				}
				selectedAttr.insert(A);
			}
		}
		else
			for (auto A : T.Attr) if (A.name[0] != '#')selectedAttr.insert(A.name);
		auto temp = rm.selectRecord(tableName, cond);
		for (auto it : temp)
		{
			auto dataList = T.getDataList(it);
			for (auto ele : dataList)
				if (selectedAttr.count(ele.first))
					r[ele.first].push_back(ele.second);
		}
	}
	catch (string Excpt)
	{
		r["#Error"].push_back(Excpt);
		r["#Status"].push_back("failed");
		return r;
	}
	catch (...)
	{
		r["#Error"].push_back("Unknown interal error, please contact us.");
		r["#Status"].push_back("failed");
		return r;
	}
	r["#SelectFlag"].push_back("true");
	r["#Status"].push_back("successed");
	return r;
}
selector::RES selector::createTable(string tableName, vector<attribute> Attr)
{
	map<string, vector<string>> r;
	try
	{
		if (cm.getTable(tableName).name != "*NULL*")
		{
			r["#Error"].push_back("Table existed.");
			r["#Status"].push_back("failed");
			return r;
		}
		cm.createTable(tableName, Attr);
		rm.createTableFile(cm.getTable(tableName));
	}
	catch (string Excpt)
	{
		r["#Error"].push_back(Excpt);
		r["#Status"].push_back("failed");
		return r;
	}
	catch (...)
	{
		r["#Error"].push_back("Unknown interal error, please contact us.");
		r["#Status"].push_back("failed");
		return r;
	}
	r["#Status"].push_back("successed");
	return r;
}
selector::RES selector::dropTable(string tableName)
{
	map<string, vector<string>> r;
	try
	{
		if (cm.getTable(tableName).name == "*NULL*")
		{
			r["#Error"].push_back("Table \""+tableName+"\" does not exist.");
			r["#Status"].push_back("failed");
			return r;
		}
		table T = cm.getTable(tableName);
		dropFileList.push_back(T.fileName);
		for (auto it : T.Attr)
		{
			if (it.index != "*NULL*") dropFileList.push_back(it.index + ".index");
		}
		cm.dropTable(tableName);
	}
	catch (string Excpt)
	{
		r["#Error"].push_back(Excpt);
		r["#Status"].push_back("failed");
		return r;
	}
	catch (...)
	{
		r["#Error"].push_back("Unknown interal error, please contact us.");
		r["#Status"].push_back("failed");
		return r;
	}
	r["#Status"].push_back("successed");
	return r;
}
selector::RES selector::insertRecord(string tableName, map<string, string> & dataList)
{
	map<string, vector<string>> r;
	try
	{
		if (cm.getTable(tableName).name == "*NULL*")
		{
			r["#Error"].push_back("Table \""+tableName+"\" does not exist.");
			r["#Status"].push_back("failed");
			return r;
		}
		table T = getTable(tableName);
		for (auto it : dataList)
		{
			if (T.AttrMap.count(it.first) == 0)
				r["#Warning"].push_back("Ignored \"" + it.first + "\", no such attribute in table \"" + tableName + "\".");
		}
		rm.insertRecord(tableName, dataList);
	}
	catch (string Excpt)
	{
		r["#Error"].push_back(Excpt);
		r["#Status"].push_back("failed");
		return r;
	}
	catch (...)
	{
		r["#Error"].push_back("Unknown interal error, please contact us.");
		r["#Status"].push_back("failed");
		return r;
	}
	r["#Status"].push_back("successed");
	return r;
}
selector::RES selector::deleteRecord(string tableName, condition cond)
{
	map<string, vector<string>> r;
	try
	{
		table T = cm.getTable(tableName);
		if (T.name == "*NULL*")
		{
			r["#Error"].push_back("Table \""+tableName+"\" does not exist.");
			r["#Status"].push_back("failed");
			return r;
		}
		r["#Delete"].push_back(to_string(rm.deleteRecord(tableName, cond)) + " row(s) affected.");
	}
	catch (string Excpt)
	{
		r["#Error"].push_back(Excpt);
		r["#Status"].push_back("failed");
		return r;
	}
	catch (...)
	{
		r["#Error"].push_back("Unknown interal error, please contact us.");
		r["#Status"].push_back("failed");
		return r;
	}
	r["#Status"].push_back("successed");
	return r;
}
table selector::getTable(string tableName)
{
	return cm.getTable(tableName);
}

selector::selector() :bm(new bufferManager), cm(*bm), im(cm), rm(*bm, im, cm) {}

selector::~selector()
{
	delete bm;
	for (auto it : dropFileList)
	{
		int i;
		for (i = 0; i < 20; ++i)
			if (!remove(it.c_str())) break;
		if (i == 20) cerr << "Failed to remove file \"" << it << "\", please remove it manually." << endl;
	}
}
