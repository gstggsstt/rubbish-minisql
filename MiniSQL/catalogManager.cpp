#include "pch.h"
#include "catalogManager.h"
#include "Spliter.h"

void catalogManager::readInfo()
{
	dataStored.clear();
	string s; char c;
	FILE* fp; fopen_s(&fp,catalogFile, "r+");
	if (!fp) fopen_s(&fp,catalogFile, "w+");
	while (true)
	{
		s.clear();
		while ((c = fgetc(fp)) != ';' && c != EOF) s += c;
		if (c != EOF) s += ';', dataStored.push_back(s);
		else break;
	}
	fclose(fp);
}

void catalogManager::writeInfo()
{
	FILE* fp;
	fopen_s(&fp,catalogFile, "w+");
	for (int i = 0; i < dataStored.size(); i++)
		fprintf(fp, "%s", dataStored[i].c_str());
	dataStored.clear();
	fclose(fp);
}

void catalogManager::createTable(string tableName, vector<attribute> attr)
{
	readInfo();
	string s;
	ostringstream sout;
	sout << tableName << " ";
	for (int i = 0; i < attr.size(); i++)
	{
		if (attr[i].name[0] == '#') continue;
		sout << attr[i].name << " ";
		sout << attr[i].type << " ";
		sout << (attr[i].notNull ? "1" : "0") << " ";
		sout << (attr[i].primaryKey ? "1" : "0") << " ";
		sout << (attr[i].unique ? "1" : "0") << " ";
		sout << attr[i].index << " ";
	}
	sout << ";" << endl;
	s = sout.str();
	dataStored.push_back(s);
	writeInfo();
}

table catalogManager::getTable(string tableName)
{
	readInfo();
	vector<string> words;
	string s;
	int ii;
	for (ii = 0; ii < dataStored.size(); ii++)
	{
		if (dataStored[ii].find(tableName+" ") == 0)
		{
			s = dataStored[ii];
			break;
		}
	}
	if(ii<dataStored.size())
	{
		Spliter Sp(s); string temp;
		while ((temp = Sp.getWord()) != "")words.push_back(temp);
	}
	vector<attribute> at;
	for (int i = 1; i * 6 < words.size(); i++)
		at.push_back(attribute(words[6 * i - 5], stoi(words[6 * i - 4]), !!stoi(words[6 * i - 3]),
						!!stoi(words[6 * i - 2]), !!stoi(words[6 * i - 1]), words[6 * i]));
	if (words.size() == 0) return table("*NULL*", "", at, bm);
	return table(words[0], words[0] + ".sqldata", at, bm);
}
void catalogManager::dropTable(string s)
{
	readInfo();
	int ii;
	for (ii = 0; ii < dataStored.size(); ii++)
	{
		if (dataStored[ii].find_first_of(s) == 0)
			break;
	}
	if (ii < dataStored.size())
	{
		dataStored[ii].swap(dataStored.back());
		dataStored.pop_back();
	}
	writeInfo();
}

pair<string, string> catalogManager::getAttrByIndex(string idx)
{
	readInfo();
	for (auto it : dataStored)
	{
		Spliter Sp(it);
		table temp = getTable(Sp.getWord());
		for (auto it : temp.Attr)
			if (it.index == idx) return make_pair(temp.name, it.name);
	}
	return make_pair("*NULL*", "*NULL*");
}

void catalogManager::discardAttrIndex(string tableName, string attrName)
{
	table T = getTable(tableName);
	T.discardAttrIndex(attrName);
	dropTable(tableName);
	createTable(tableName, T.Attr);
}
void catalogManager::giveAttrIndex(string tableName, string attrName, string idx)
{
	table T = getTable(tableName);
	T.giveAttrIndex(attrName,idx);
	dropTable(tableName);
	createTable(tableName, T.Attr);
}
