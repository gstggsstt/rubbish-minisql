#pragma once
#include "recordManager.h"
#include <string>
#include <vector>

using namespace std;

class selector
{
	typedef pair<int,int> PII;
	typedef map<string,vector<string> > RES;

	vector<string> dropFileList;

protected:
	bufferManager *bm;
	indexManager im;
	catalogManager cm;
	recordManager rm;

public:
	selector();
	void flush()
	{
		bm->flush();
		im.writeToFile();
		for (auto it : dropFileList)
		{
			int i;
			for (i = 0; i < 20; ++i)
				if (!remove(it.c_str())) break;
			if (i == 20) cerr << "Failed to remove file \"" << it << "\", please remove it manually." << endl;
		}
		dropFileList.clear();
	}
	RES createIndex(string tableName, string Attr, string idx);
	RES dropIndex(string idx);
	RES selectRecord(string tableName, vector<string> Attr, condition con);
	RES createTable(string tableName, vector<attribute> Attr);
	RES dropTable(string tableName);
	RES insertRecord(string tableName, map<string, string> & dataList);
	RES deleteRecord(string tableName, condition cond);
	table getTable(string tableName);
	~selector();
};

