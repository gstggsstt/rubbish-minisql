#include "pch.h"
#include "indexManager.h"

#define EPS (1e-6)

void indexManager::loadIndex(table &T, string Attr)
{
	string fileName = T.AttrMap[Attr].index + ".index";
	FILE * temp;
	fopen_s(&temp, fileName.c_str(), "r");
	if (temp == NULL)
		throw string("Index file does not exist.");
	fclose(temp);
	ifstream fin(fileName);
	map<int, PII> Mint;
	map<float, PII> Mfloat;
	map<double, PII> Mdouble;
	map<string, PII> Mstring;
	int tp = T.AttrMap[Attr].type;
	while (!fin.eof())
	{
		string str;
		getline(fin, str);
		if (str == "") break;
		auto p1 = str.find_last_of(':');
		auto p2 = str.find_last_of(',');
		string sub1, sub2, sub3;
		sub1 = str.substr(0, p1);
		sub2 = str.substr(p1 + 1, p2-p1-1);
		sub3 = str.substr(p2 + 1, str.length()-p2-1);
		PII pos = make_pair(stoi(sub2), stoi(sub3));
		if (tp == ATTR_TYPE_INT)
			Mint[stoi(sub1)] = pos;
		else if (tp == ATTR_TYPE_FLOAT)
		{
			int temp = stoi(sub1);
			Mfloat[*(float *)&temp] = pos;
		}
		else if (tp == ATTR_TYPE_DOUBLE)
		{
			long long temp = stoll(sub1);
			Mdouble[*(double *)&temp] = pos;
		}
		else
			Mstring[sub1] = pos;
	}
	if (tp == ATTR_TYPE_INT)
		IDXint[make_pair(T.name, Attr)] = Mint;
	else if (tp == ATTR_TYPE_FLOAT)
		IDXfloat[make_pair(T.name, Attr)] = Mfloat;
	else if (tp == ATTR_TYPE_DOUBLE)
		IDXdouble[make_pair(T.name, Attr)] = Mdouble;
	else
		IDXstring[make_pair(T.name, Attr)] = Mstring;
	fin.close();
}

void indexManager::writeToFile()
{

	for (auto MM : IDXint)
	{
		table T = cm.getTable(MM.first.first);
		string fileName = T.AttrMap[MM.first.second].index+".index";
		ofstream fout(fileName);
		for (auto it : MM.second)
			fout << it.first << ":" << it.second.first << "," << it.second.second << endl;
		fout.close();
	}
	for (auto MM : IDXfloat)
	{
		table T = cm.getTable(MM.first.first);
		string fileName = T.AttrMap[MM.first.second].index + ".index";
		ofstream fout(fileName);
		for (auto it : MM.second)
			fout << *(int *)&it.first << ":" << it.second.first << "," << it.second.second << endl;
		fout.close();
	}
	for (auto MM : IDXdouble)
	{
		table T = cm.getTable(MM.first.first);
		string fileName = T.AttrMap[MM.first.second].index + ".index";
		ofstream fout(fileName);
		for (auto it : MM.second)
			fout << *(long long *)&it.first << ":" << it.second.first << "," << it.second.second << endl;
		fout.close();
	}
	for (auto MM : IDXstring)
	{
		table T = cm.getTable(MM.first.first);
		string fileName = T.AttrMap[MM.first.second].index + ".index";
		ofstream fout(fileName);
		for (auto it : MM.second)
			fout << it.first << ":" << it.second.first << "," << it.second.second << endl;
		fout.close();
	}
}

void indexManager::writeIndex(table &T, string Attr, map<int, PII> &Mint,
							  map<float, PII> &Mfloat, map<double, PII> &Mdouble,
							  map<string, PII> &Mstring, string fileName)
{
	int tp = T.AttrMap[Attr].type;
	if (tp == ATTR_TYPE_INT)
		IDXint[make_pair(T.name, Attr)] = Mint;
	else if (tp == ATTR_TYPE_FLOAT)
		IDXfloat[make_pair(T.name, Attr)] = Mfloat;
	else if (tp == ATTR_TYPE_DOUBLE)
		IDXdouble[make_pair(T.name, Attr)] = Mdouble;
	else
		IDXstring[make_pair(T.name, Attr)] = Mstring;
}

void indexManager::createIndex(table T, string Attr, string idx = "")
{
	if (idx == "")
		idx = T.name + "_" + Attr;
	string fileName = idx + ".index";
	FILE * temp;
	fopen_s(&temp, fileName.c_str(), "r");
	if (temp != NULL)
	{
		fclose(temp);
		throw string("Index file has already existed.");
	}

	map<int, PII> Mint;
	map<float, PII> Mfloat;
	map<double, PII> Mdouble;
	map<string, PII> Mstring;
	int tp = T.AttrMap[Attr].type;

	if (tp == ATTR_TYPE_INT)
		for (PII pos = T.getFirstRecordPos(); pos != make_pair(0, 0); pos = T.getNext(pos))
			Mint.insert(make_pair(T.getIntData(pos, Attr), pos));
	else if (tp == ATTR_TYPE_FLOAT)
		for (PII pos = T.getFirstRecordPos(); pos != make_pair(0, 0); pos = T.getNext(pos))
			Mfloat.insert(make_pair(T.getFloatData(pos, Attr), pos));
	else if (tp == ATTR_TYPE_DOUBLE)
		for (PII pos = T.getFirstRecordPos(); pos != make_pair(0, 0); pos = T.getNext(pos))
			Mdouble.insert(make_pair(T.getDoubleData(pos, Attr), pos));
	else
		for (PII pos = T.getFirstRecordPos(); pos != make_pair(0, 0); pos = T.getNext(pos))
			Mstring.insert(make_pair(T.getStringData(pos, Attr), pos));

	cm.giveAttrIndex(T.name, Attr, idx);
	writeIndex(T, Attr, Mint, Mfloat, Mdouble, Mstring, fileName);
}

void indexManager::dropIndex(string idx)
{
	string fileName = idx + ".index";
	FILE * temp;
	fopen_s(&temp, fileName.c_str(), "r");
	if (temp == NULL)
		throw string("Index file does not exist.");
	fclose(temp);
	pair<string, string> A = cm.getAttrByIndex(idx);
	table T = cm.getTable(A.first);
	int tp = T.AttrMap[A.second].type;
	if (tp == ATTR_TYPE_INT)
		IDXint.erase(A);
	else if (tp == ATTR_TYPE_FLOAT)
		IDXfloat.erase(A);
	else if (tp == ATTR_TYPE_DOUBLE)
		IDXdouble.erase(A);
	else
		IDXstring.erase(A);
	cm.discardAttrIndex(A.first, A.second);
}

void indexManager::checkLoaded(table &T, attribute &it)
{
	if (it.type == ATTR_TYPE_INT && !IDXint.count(make_pair(T.name, it.name)))
		loadIndex(T, it.name);
	else if (it.type == ATTR_TYPE_FLOAT && !IDXfloat.count(make_pair(T.name, it.name)))
		loadIndex(T, it.name);
	else if (it.type == ATTR_TYPE_DOUBLE && !IDXdouble.count(make_pair(T.name, it.name)))
		loadIndex(T, it.name);
	else if (it.type>0 && !IDXstring.count(make_pair(T.name, it.name)))
		loadIndex(T, it.name);
}

void indexManager::insertIndex(table &T, map<string, string> data, PII pos)
{
	for (auto it : T.Attr)
		if (it.index != "*NULL*")
		{
			checkLoaded(T, it);
			if (it.type == ATTR_TYPE_INT)
				IDXint[make_pair(T.name, it.name)].insert(make_pair(stoi(data[it.name]), pos));
			else if (it.type == ATTR_TYPE_FLOAT)
				IDXfloat[make_pair(T.name, it.name)].insert(make_pair((float)stof(data[it.name]), pos));
			else if (it.type == ATTR_TYPE_DOUBLE)
				IDXdouble[make_pair(T.name, it.name)].insert(make_pair(stof(data[it.name]), pos));
			else
				IDXstring[make_pair(T.name, it.name)].insert(make_pair(data[it.name], pos));
		}
}

void indexManager::deleteIndex(table &T, vector<PII> vec)
{
	for (auto it : T.Attr)
		if (it.index != "*NULL*")
		{
			checkLoaded(T, it);
			for (auto e : vec)
			{
				if (it.type == ATTR_TYPE_INT)
					IDXint[make_pair(T.name, it.name)].erase(T.getIntData(e, it.name));
				else if (it.type == ATTR_TYPE_FLOAT)
					IDXfloat[make_pair(T.name, it.name)].erase(T.getFloatData(e, it.name));
				else if (it.type == ATTR_TYPE_DOUBLE)
					IDXdouble[make_pair(T.name, it.name)].erase(T.getDoubleData(e, it.name));
				else
					IDXstring[make_pair(T.name, it.name)].erase(T.getStringData(e, it.name));
			}
		}
}

vector<indexManager::PII> indexManager::selectPosById(table &T, condition &cond)
{
	if (cond.first.size() == 1)
	{
		checkLoaded(T, T.AttrMap[cond.first[0].val1]);
		int type = T.AttrMap[cond.first[0].val1].type;
		if (cond.first[0].op == "==" || cond.first[0].op =="=")
			if (type == ATTR_TYPE_INT)
			{
				vector<PII> vec;
				auto L = IDXint[make_pair(T.name, cond.first[0].val1)].lower_bound(stoi(cond.first[0].val2));
				auto R = IDXint[make_pair(T.name, cond.first[0].val1)].upper_bound(stoi(cond.first[0].val2));
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else if (type == ATTR_TYPE_FLOAT)
			{
				vector<PII> vec;
				auto L = IDXfloat[make_pair(T.name, cond.first[0].val1)].lower_bound((float)stof(cond.first[0].val2)*(1-EPS));
				auto R = IDXfloat[make_pair(T.name, cond.first[0].val1)].upper_bound((float)stof(cond.first[0].val2)*(1+EPS));
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else if (type == ATTR_TYPE_DOUBLE)
			{
				vector<PII> vec;
				auto L = IDXdouble[make_pair(T.name, cond.first[0].val1)].lower_bound(stof(cond.first[0].val2)*(1-EPS));
				auto R = IDXdouble[make_pair(T.name, cond.first[0].val1)].upper_bound(stof(cond.first[0].val2)*(1+EPS));
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else
			{
				vector<PII> vec;
				auto L = IDXstring[make_pair(T.name, cond.first[0].val1)].lower_bound(cond.first[0].val2);
				auto R = IDXstring[make_pair(T.name, cond.first[0].val1)].upper_bound(cond.first[0].val2);
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
		else if (cond.first[0].op == "<")
			if (type == ATTR_TYPE_INT)
			{
				vector<PII> vec;
				auto L = IDXint[make_pair(T.name, cond.first[0].val1)].begin();
				auto R = IDXint[make_pair(T.name, cond.first[0].val1)].lower_bound(stoi(cond.first[0].val2));
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else if (type == ATTR_TYPE_FLOAT)
			{
				vector<PII> vec;
				auto L = IDXfloat[make_pair(T.name, cond.first[0].val1)].begin();
				auto R = IDXfloat[make_pair(T.name, cond.first[0].val1)].lower_bound((float)stof(cond.first[0].val2)*(1-EPS));
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else if (type == ATTR_TYPE_DOUBLE)
			{
				vector<PII> vec;
				auto L = IDXdouble[make_pair(T.name, cond.first[0].val1)].begin();
				auto R = IDXdouble[make_pair(T.name, cond.first[0].val1)].lower_bound(stof(cond.first[0].val2)*(1-EPS));
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else
			{
				vector<PII> vec;
				auto L = IDXstring[make_pair(T.name, cond.first[0].val1)].begin();
				auto R = IDXstring[make_pair(T.name, cond.first[0].val1)].lower_bound(cond.first[0].val2);
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
		else if (cond.first[0].op == "<=")
			if (type == ATTR_TYPE_INT)
			{
				vector<PII> vec;
				auto L = IDXint[make_pair(T.name, cond.first[0].val1)].begin();
				auto R = IDXint[make_pair(T.name, cond.first[0].val1)].upper_bound(stoi(cond.first[0].val2));
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else if (type == ATTR_TYPE_FLOAT)
			{
				vector<PII> vec;
				auto L = IDXfloat[make_pair(T.name, cond.first[0].val1)].begin();
				auto R = IDXfloat[make_pair(T.name, cond.first[0].val1)].upper_bound((float)stof(cond.first[0].val2)*(1+EPS));
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else if (type == ATTR_TYPE_DOUBLE)
			{
				vector<PII> vec;
				auto L = IDXdouble[make_pair(T.name, cond.first[0].val1)].begin();
				auto R = IDXdouble[make_pair(T.name, cond.first[0].val1)].upper_bound(stof(cond.first[0].val2)*(1+EPS));
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else
			{
				vector<PII> vec;
				auto L = IDXstring[make_pair(T.name, cond.first[0].val1)].begin();
				auto R = IDXstring[make_pair(T.name, cond.first[0].val1)].upper_bound(cond.first[0].val2);
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
		else if (cond.first[0].op == ">")
			if (type == ATTR_TYPE_INT)
			{
				vector<PII> vec;
				auto L = IDXint[make_pair(T.name, cond.first[0].val1)].upper_bound(stoi(cond.first[0].val2));
				auto R = IDXint[make_pair(T.name, cond.first[0].val1)].end();
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else if (type == ATTR_TYPE_FLOAT)
			{
				vector<PII> vec;
				auto L = IDXfloat[make_pair(T.name, cond.first[0].val1)].upper_bound((float)stof(cond.first[0].val2)*(1+EPS));
				auto R = IDXfloat[make_pair(T.name, cond.first[0].val1)].end();
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else if (type == ATTR_TYPE_DOUBLE)
			{
				vector<PII> vec;
				auto L = IDXdouble[make_pair(T.name, cond.first[0].val1)].upper_bound(stof(cond.first[0].val2)*(1+EPS));
				auto R = IDXdouble[make_pair(T.name, cond.first[0].val1)].end();
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else
			{
				vector<PII> vec;
				auto L = IDXstring[make_pair(T.name, cond.first[0].val1)].upper_bound(cond.first[0].val2);
				auto R = IDXstring[make_pair(T.name, cond.first[0].val1)].end();
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
		else if (cond.first[0].op == ">=")
			if (type == ATTR_TYPE_INT)
			{
				vector<PII> vec;
				auto L = IDXint[make_pair(T.name, cond.first[0].val1)].lower_bound(stoi(cond.first[0].val2));
				auto R = IDXint[make_pair(T.name, cond.first[0].val1)].end();
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else if (type == ATTR_TYPE_FLOAT)
			{
				vector<PII> vec;
				auto L = IDXfloat[make_pair(T.name, cond.first[0].val1)].lower_bound((float)stof(cond.first[0].val2)*(1-EPS));
				auto R = IDXfloat[make_pair(T.name, cond.first[0].val1)].end();
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else if (type == ATTR_TYPE_DOUBLE)
			{
				vector<PII> vec;
				auto L = IDXdouble[make_pair(T.name, cond.first[0].val1)].lower_bound(stof(cond.first[0].val2)*(1-EPS));
				auto R = IDXdouble[make_pair(T.name, cond.first[0].val1)].end();
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
			else
			{
				vector<PII> vec;
				auto L = IDXstring[make_pair(T.name, cond.first[0].val1)].lower_bound(cond.first[0].val2);
				auto R = IDXstring[make_pair(T.name, cond.first[0].val1)].end();
				for(auto it=L;it!=R;++it) vec.push_back((*it).second);
				return vec;
			}
		else
		{
			if (type == ATTR_TYPE_INT)
			{
				vector<PII> vec;
				for (auto it : IDXint[make_pair(T.name, cond.first[0].val1)])
					if (it.first != stoi(cond.first[0].val2))
						vec.push_back(it.second);
				return vec;
			}
			else if (type == ATTR_TYPE_FLOAT)
			{
				vector<PII> vec;
				for (auto it : IDXfloat[make_pair(T.name, cond.first[0].val1)])
					if (fabs(it.first - (float)stof(cond.first[0].val2)) < (it.first + (float)stof(cond.first[0].val2))*EPS)
						vec.push_back(it.second);
				return vec;
			}
			else if (type == ATTR_TYPE_DOUBLE)
			{
				vector<PII> vec;
				for (auto it : IDXdouble[make_pair(T.name, cond.first[0].val1)])
					if (fabs(it.first - stof(cond.first[0].val2)) < (it.first + stof(cond.first[0].val2))*EPS)
						vec.push_back(it.second);
				return vec;
			}
			else
			{
				vector<PII> vec;
				for (auto it : IDXstring[make_pair(T.name, cond.first[0].val1)])
					if (it.first != cond.first[0].val2)
						vec.push_back(it.second);
				return vec;
			}
		}
	}
}

vector<indexManager::PII> indexManager::selectPosById_Range(table &T, condition &cond)
{
	checkLoaded(T, T.AttrMap[cond.first[0].val1]);
	int type = T.AttrMap[cond.first[0].val1].type;
	if (type == ATTR_TYPE_INT)
	{
		vector<PII> vec;
		auto L = IDXint[make_pair(T.name, cond.first[0].val1)].begin();
		auto R = IDXint[make_pair(T.name, cond.first[0].val1)].end();
		if (cond.first[0].op == ">")
			L = IDXint[make_pair(T.name, cond.first[0].val1)].upper_bound(stoi(cond.first[0].val2));
		if (cond.first[0].op == ">=")
			L = IDXint[make_pair(T.name, cond.first[0].val1)].lower_bound(stoi(cond.first[0].val2));
		if (cond.first[1].op == "<")
			R = IDXint[make_pair(T.name, cond.first[1].val1)].lower_bound(stoi(cond.first[1].val2));
		if (cond.first[1].op == "<=")
			R = IDXint[make_pair(T.name, cond.first[1].val1)].upper_bound(stoi(cond.first[1].val2));
		for (auto it = L; it != R; ++it)
			vec.push_back(it->second);
		return vec;
	}
	else if (type == ATTR_TYPE_FLOAT)
	{
		vector<PII> vec;
		auto L = IDXfloat[make_pair(T.name, cond.first[0].val1)].begin();
		auto R = IDXfloat[make_pair(T.name, cond.first[0].val1)].end();
		if (cond.first[0].op == ">")
			L = IDXfloat[make_pair(T.name, cond.first[0].val1)].upper_bound((float)stof(cond.first[0].val2)*(1+EPS));
		if (cond.first[0].op == ">=")
			L = IDXfloat[make_pair(T.name, cond.first[0].val1)].lower_bound((float)stof(cond.first[0].val2)*(1-EPS));
		if (cond.first[1].op == "<")
			R = IDXfloat[make_pair(T.name, cond.first[1].val1)].lower_bound((float)stof(cond.first[1].val2)*(1-EPS));
		if (cond.first[1].op == "<=")
			R = IDXfloat[make_pair(T.name, cond.first[1].val1)].upper_bound((float)stof(cond.first[1].val2)*(1+EPS));
		for (auto it = L; it != R; ++it)
			vec.push_back(it->second);
		return vec;
	}
	else if (type == ATTR_TYPE_DOUBLE)
	{
		vector<PII> vec;
		auto L = IDXdouble[make_pair(T.name, cond.first[0].val1)].begin();
		auto R = IDXdouble[make_pair(T.name, cond.first[0].val1)].end();
		if (cond.first[0].op == ">")
			L = IDXdouble[make_pair(T.name, cond.first[0].val1)].upper_bound(stof(cond.first[0].val2)*(1+EPS));
		if (cond.first[0].op == ">=")
			L = IDXdouble[make_pair(T.name, cond.first[0].val1)].lower_bound(stof(cond.first[0].val2)*(1-EPS));
		if (cond.first[1].op == "<")
			R = IDXdouble[make_pair(T.name, cond.first[1].val1)].lower_bound(stof(cond.first[1].val2)*(1-EPS));
		if (cond.first[1].op == "<=")
			R = IDXdouble[make_pair(T.name, cond.first[1].val1)].upper_bound(stof(cond.first[1].val2)*(1+EPS));
		for (auto it = L; it != R; ++it)
			vec.push_back(it->second);
		return vec;
	}
	vector<PII> vec;
	auto L = IDXstring[make_pair(T.name, cond.first[0].val1)].begin();
	auto R = IDXstring[make_pair(T.name, cond.first[0].val1)].end();
	if (cond.first[0].op == ">")
		L = IDXstring[make_pair(T.name, cond.first[0].val1)].upper_bound(cond.first[0].val2);
	if (cond.first[0].op == ">=")
		L = IDXstring[make_pair(T.name, cond.first[0].val1)].lower_bound(cond.first[0].val2);
	if (cond.first[1].op == "<")
		R = IDXstring[make_pair(T.name, cond.first[1].val1)].lower_bound(cond.first[1].val2);
	if (cond.first[1].op == "<=")
		R = IDXstring[make_pair(T.name, cond.first[1].val1)].upper_bound(cond.first[1].val2);
	for (auto it = L; it != R; ++it)
		vec.push_back(it->second);
	return vec;
}
