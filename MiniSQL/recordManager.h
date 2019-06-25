#pragma once
#include "indexManager.h"
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <sstream>

using namespace std;

class recordManager
{
	typedef pair<int,int> PII;

	bufferManager & bm;
	indexManager & im;
	catalogManager & cm;

	char * createRecord(table & T,map<string,string> & dataList);

	bool checkPrimaryKey(table & T, attribute it, string val);

	pair<bool,bool> checkCond(table T, condition & cond);
	void flipOp(string & op);

public:
	recordManager(bufferManager & bmbm, indexManager & imim, catalogManager & cmcm):bm(bmbm),im(imim),cm(cmcm){}

    void insertRecord(const string & tableName, map<string,string> & dataList);
	int deleteRecord(const string & tableName, condition & cond);
	vector<PII> selectRecord(const string & tableName, condition & cond);
	vector<PII> selectPos(table & T, condition & cond);
	vector<PII> selectAllPos(table & T);
	vector<PII> selectPosNormally(table & T, condition & cond);

	void createTableFile(table T);
};

