#include "pch.h"
#include "recordManager.h"

char *recordManager::createRecord(table &T, map<string, string> &dataList)
{
	int len = 0;
	char *data = new char[T.recordLength];
	conditionChecker C;
	data[T.AttrPos["#nullList"]] = 0;
	for (int itt = 0; itt < (int)T.Attr.size(); ++itt)
	{
		auto it = T.Attr[itt];
		if (it.name[0] == '#') break;
		if (dataList.count(it.name))
		{
			string temp = dataList[it.name];
			if (it.type == ATTR_TYPE_INT)
			{
				if (C.getType(temp) != ATTR_TYPE_INT)
					throw string("Value type of \"" + it.name + "\" doesn't match.");
				*(int *)(data + len) = atoi(temp.c_str()), len += 4;
			}
			else if (it.type == ATTR_TYPE_FLOAT)
			{
				if (C.getType(temp) >0)
					throw string("Value type of \"" + it.name + "\" doesn't match.");
				*(float *)(data + len) = (float)atof(temp.c_str()), len += 4;
			}
			else if (it.type == ATTR_TYPE_DOUBLE)
			{
				if (C.getType(temp) >0)
					throw string("Value type of \"" + it.name + "\" doesn't match.");
				*(double *)(data + len) = (double)atof(temp.c_str()), len += 8;
			}
			else
			{
				if (C.getType(temp) <=0)
					throw string("Missing quotation(s) arround string.");
				if (it.type < temp.length()+1)
					throw string("String is too long, length of input is "+to_string(temp.length()-2)+
						" , while max length of \""+it.name+"\" is "+to_string(it.type-3)+" .");
				int i;
				for (i = 0; i < it.type - 1 && i < temp.length(); ++i)
					data[len++] = temp[i];
				for (; i < it.type; ++i)
					data[len++] = 0;
			}
		}
		else if (it.notNull)
		{
			delete[] data;
			throw string("Attribute \""+it.name+"\" is declared as NOTNULL, value of \""+it.name+"\" is needed.");
		}
		else
		{
			if (it.type == ATTR_TYPE_INT)
				*(int *)(data + len) = 0, len += 4;
			else if (it.type == ATTR_TYPE_FLOAT)
				*(float *)(data + len) = 0.0f, len += 4;
			else if (it.type == ATTR_TYPE_DOUBLE)
				*(double *)(data + len) = 0.0, len += 8;
			else
				for (int i = 0; i < it.type; ++i)
					data[len++] = 0;
			data[T.AttrPos["#nullList"] + itt / 8] |= 1 << (itt & 7);
		}
		if (it.unique && checkPrimaryKey(T, it, dataList[it.name]))
		{
			delete[] data;
			throw string("Conflict of UNIQUE constraint.");
		}
	}
	return data;
}

bool recordManager::checkPrimaryKey(table & T, attribute it, string val)
{
	condition temp;
	temp.first.push_back(valueComparison(it.name,val,"=="));
	temp.second.push_back(logicalComparison(""));
	temp.second.push_back(logicalComparison(""));
	return selectRecord(T.name, temp).size();
}

void recordManager::insertRecord(const string &tableName, map<string, string> &dataList)
{
	table T = cm.getTable(tableName);
	char *data = createRecord(T, dataList);
	T.insertIntoBlock(data);
	delete[] data;
	im.insertIndex(T, dataList, T.getFirstRecordPos());
}

int recordManager::deleteRecord(const string &tableName, condition &cond)
{
	table T = cm.getTable(tableName);
	vector<PII> selectedRecords = selectPos(T, cond);
	sort(selectedRecords.begin(), selectedRecords.end());
	for (auto it : selectedRecords)
		T.deleteFromBlock(it);
	im.deleteIndex(T, selectedRecords);
	return selectedRecords.size();
}

void recordManager::flipOp(string & op)
{
	if (op == "<") op = ">";
	else if (op == ">") op = "<";
	else if (op == "<=") op = ">=";
	else if (op == ">=") op = "<=";
}

pair<bool,bool> recordManager::checkCond(table T, condition & cond)
{
	conditionChecker C;
	int t1, t2;
	bool res1 = false, res2 = false;
	bool fl1 = false, fl2 = false;
	bool attrCompOrPureValComp = false;
	for (auto & it : cond.first)
	{
		if (T.AttrMap.count(it.val1)) t1 = T[it.val1].type;
		else                          t1 = C.getType(it.val1), fl1 = true;
		if (T.AttrMap.count(it.val2)) t2 = T[it.val2].type;
		else                          t2 = C.getType(it.val2), fl2 = true;
		if ((t1 <= 0) != (t2 <= 0))
			throw string("Comparison between different datatype.");
		if (fl1 && !fl2) swap(it.val1, it.val2), flipOp(it.op);
		if (fl1 == fl2) attrCompOrPureValComp = true;
		fl1 = false; fl2 = false;
	}

	if (cond.first.size() == 2 && !attrCompOrPureValComp)
	{
		if (cond.first[0].val1 == cond.first[1].val1 && T.AttrMap[cond.first[0].val1].index!="*NULL*")
		{
			if ((cond.first[0].op == "<" || cond.first[0].op == "<=") &&
				(cond.first[1].op == ">" || cond.first[1].op == ">="))
				swap(cond.first[0], cond.first[1]);
			if ((cond.first[0].op == ">" || cond.first[0].op == ">=") &&
				(cond.first[1].op == "<" || cond.first[1].op == "<="))
				res1 = true;
		}
	}

	if (cond.first.size() == 1 && T.AttrMap[cond.first[0].val1].index != "*NULL*" &&
		!attrCompOrPureValComp)
		res2 = true;

	return make_pair(res1, res2);
}


vector<recordManager::PII> recordManager::selectPos(table & T, condition & cond)
{
	pair<bool,bool> temp=checkCond(T, cond);
	if(cond.first.size()==0) return selectAllPos(T);
	if (temp.first) return im.selectPosById_Range(T, cond);
	if (temp.second) return im.selectPosById(T, cond);
	return selectPosNormally(T,cond);
}

vector<recordManager::PII> recordManager::selectPosNormally(table & T, condition & cond)
{
	conditionChecker C;
	vector<PII> vec;
	for (PII pos = T.getFirstRecordPos(); pos != make_pair(0, 0); pos = T.getNext(pos))
		if (C.check(T, T.getDataList(pos), cond))
			vec.push_back(pos);
	return vec;
}

vector<recordManager::PII> recordManager::selectAllPos(table &T)
{
	vector<PII> vec;
	for (PII pos = T.getFirstRecordPos(); pos != make_pair(0, 0); pos = T.getNext(pos))
		vec.push_back(pos);
	return vec;
}

vector<recordManager::PII> recordManager::selectRecord(const string & tableName, condition & cond)
{
	table T=cm.getTable(tableName);
	return selectPos(T,cond);
}

void recordManager::createTableFile(table T)
{
	T.createFile();
}
