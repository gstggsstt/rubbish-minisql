#pragma once
#include "attribute.h"
#include "bufferManager.h"
#include <algorithm>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

class table
{
public:
	string name;
	string fileName;
	vector<attribute> Attr;  // order is very important, which decides the store order in disk
	int headLength;
	int recordLength;

	map<string,int> AttrPos;
	map<string,attribute> AttrMap;

	bufferManager &bm;

	const attribute& operator[](string str);

	void createFile();

	void giveAttrIndex(string attrName, string indexName);
	void discardAttrIndex(string attrName);

	pair<int,int> getFirstRecordPos();
	pair<int,int> getFirstBlankPos();
	pair<int,int> getRest();
	pair<int,int> getNext(pair<int,int> pos);
	pair<int,int> getPre(pair<int,int> pos);
	vector<bool> getNullList(pair<int,int> pos);
	void setFirstRecordPos(pair<int,int> pos);
	void setFirstBlankPos(pair<int,int> pos);
	void setAddRest(pair<int,int> pos);
	void setRest(pair<int,int> pos);
	void setNext(pair<int,int> pos,pair<int,int> nextpos);
	void setPre(pair<int,int> pos,pair<int,int> nextpos);
	void setNullList(pair<int,int> pos, char *data);

	int getIntData(pair<int,int> pos,string A);
	float getFloatData(pair<int,int> pos,string A);
	double getDoubleData(pair<int,int> pos,string A);
	string getStringData(pair<int,int> pos, string A);

	string getData(pair<int, int> pos, string A, int x=-1);

	map<string,string> getDataList(pair<int,int> pos);

	void insertIntoBlock(char * data);
	void deleteFromBlock(pair<int,int> pos);

	// ORDER OF ELEMENTS IN VEC IS VERY IMPORTANT
	table(string nm, string fn, vector<attribute> vec,
		bufferManager & bmbm, int hL=24);
};

