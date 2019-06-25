#include "pch.h"
#include "table.h"

const attribute &table::operator[](string str)
{
	return AttrMap[str];
}

void table::createFile()
{
	setFirstRecordPos(make_pair(0,0));
	setFirstBlankPos(make_pair(0,0));
	setRest(make_pair(0,headLength));
}

pair<int, int> table::getFirstRecordPos()
{
	return make_pair(bm.getNumFromBlock<int>(fileName, 0, 0),
					 bm.getNumFromBlock<int>(fileName, 0, 4));
}

pair<int, int> table::getFirstBlankPos()
{
	return make_pair(bm.getNumFromBlock<int>(fileName, 0, 8),
					 bm.getNumFromBlock<int>(fileName, 0, 12));
}

pair<int, int> table::getRest()
{
	return make_pair(bm.getNumFromBlock<int>(fileName, 0, 16),
					 bm.getNumFromBlock<int>(fileName, 0, 20));
}

pair<int, int> table::getNext(pair<int, int> pos)
{
	return make_pair(bm.getNumFromBlock<int>(fileName, pos.first, pos.second + recordLength - 8),
					 bm.getNumFromBlock<int>(fileName, pos.first, pos.second + recordLength - 4));
}

pair<int, int> table::getPre(pair<int, int> pos)
{
	return make_pair(bm.getNumFromBlock<int>(fileName, pos.first, pos.second + recordLength - 16),
					 bm.getNumFromBlock<int>(fileName, pos.first, pos.second + recordLength - 12));
}

vector<bool> table::getNullList(pair<int, int> pos)
{
	vector<bool> vec;
	int len = AttrMap["#nullList"].type;
	for (int i = 0; i < (len + 3) / 4; ++i)
	{
		int temp = bm.getNumFromBlock<int>(fileName, pos.first, pos.second + recordLength - 16 - AttrMap["#nullList"].type + (i << 2));
		for (unsigned int j = 1; j && vec.size() < (len << 3); j <<= 1)
			vec.push_back((bool)(j & temp));
	}
	return vec;
}

void table::setFirstRecordPos(pair<int, int> pos)
{
	bm.setNumInBlock<int>(fileName, 0, 0, pos.first);
	bm.setNumInBlock<int>(fileName, 0, 4, pos.second);
}

void table::setFirstBlankPos(pair<int, int> pos)
{
	bm.setNumInBlock<int>(fileName, 0, 8, pos.first);
	bm.setNumInBlock<int>(fileName, 0, 12, pos.second);
}

void table::setRest(pair<int, int> pos)
{
	bm.setNumInBlock<int>(fileName, 0, 16, pos.first);
	bm.setNumInBlock<int>(fileName, 0, 20, pos.second);
}
void table::setAddRest(pair<int, int> pos)
{
	if (pos.second + recordLength + recordLength > BLOCK_SIZE)
		pos.first++, pos.second = 0;
	else
		pos.second += recordLength;
	bm.setNumInBlock<int>(fileName, 0, 16, pos.first);
	bm.setNumInBlock<int>(fileName, 0, 20, pos.second);
}

void table::setNext(pair<int, int> pos, pair<int, int> nextpos)
{
	if (pos == make_pair(0, 0)) return;
	bm.setNumInBlock<int>(fileName, pos.first, pos.second + recordLength - 8, nextpos.first);
	bm.setNumInBlock<int>(fileName, pos.first, pos.second + recordLength - 4, nextpos.second);
}

void table::setPre(pair<int, int> pos, pair<int, int> nextpos)
{
	if (pos == make_pair(0, 0)) return;
	bm.setNumInBlock<int>(fileName, pos.first, pos.second + recordLength - 16, nextpos.first);
	bm.setNumInBlock<int>(fileName, pos.first, pos.second + recordLength - 12, nextpos.second);
}

void table::setNullList(pair<int, int> pos, char *data)
{
	bm.setStrInBlock(fileName, pos.first, pos.second + recordLength - 16 - AttrMap["#nullList"].type, pos.second + recordLength - 16, data);
}

int table::getIntData(pair<int, int> pos, string A)
{
	auto it = AttrMap[A];
	return bm.getNumFromBlock<int>(fileName, pos.first, pos.second + AttrPos[it.name]);
}

float table::getFloatData(pair<int, int> pos, string A)
{
	auto it = AttrMap[A];
	return bm.getNumFromBlock<float>(fileName, pos.first, pos.second + AttrPos[it.name]);
}

double table::getDoubleData(pair<int, int> pos, string A)
{
	auto it = AttrMap[A];
	return bm.getNumFromBlock<double>(fileName, pos.first, pos.second + AttrPos[it.name]);
}

string table::getStringData(pair<int, int> pos, string A)
{
	auto it = AttrMap[A];
	return bm.getStrFromBlock(fileName, pos.first, pos.second + AttrPos[it.name],
							  pos.second + AttrPos[it.name] + it.type);
}

string table::getData(pair<int, int> pos, string A, int x)
{
	if (x!=-1 && 
		bm.getNumFromBlock<char>
			(fileName, pos.first, pos.second + AttrPos["#nullList"] + x / 8) & (1 << (x & 7)))
		return "UNDEFINED";
	auto it = AttrMap[A];
	if (it.type == ATTR_TYPE_INT)
	{
		int temp = bm.getNumFromBlock<int>(fileName, pos.first, pos.second + AttrPos[it.name]);
		ostringstream Cvt;
		Cvt << temp;
		return Cvt.str();
	}
	else if (it.type == ATTR_TYPE_FLOAT)
	{
		float temp = bm.getNumFromBlock<float>(fileName, pos.first, pos.second + AttrPos[it.name]);
		ostringstream Cvt;
		Cvt << temp;
		return Cvt.str();
	}
	else if (it.type == ATTR_TYPE_DOUBLE)
	{
		double temp = bm.getNumFromBlock<double>(fileName, pos.first, pos.second + AttrPos[it.name]);
		ostringstream Cvt;
		Cvt << temp;
		return Cvt.str();
	}
	else
	{
		return bm.getStrFromBlock(fileName, pos.first, pos.second + AttrPos[it.name],
								  pos.second + AttrPos[it.name] + it.type);
	}
}

map<string, string> table::getDataList(pair<int, int> pos)
{
	map<string, string> M;
	for(int i=0;i<Attr.size();++i)
	{
		attribute it = Attr[i];
		M[it.name] = getData(pos, it.name, i);
	}
	return M;
}

void table::giveAttrIndex(string attrName, string indexName)
{
	for (auto & it : Attr)
		if (it.name == attrName) { it.index = indexName; break; }
	AttrMap[attrName].index = indexName;
	return;
}

void table::discardAttrIndex(string attrName)
{
	for (auto & it : Attr)
		if (it.name == attrName) { it.index = "*NULL*"; break; }
	AttrMap[attrName].index = "*NULL*";
	return;
}

table::table(string nm, string fn, vector<attribute> vec, bufferManager & bmbm, int hL) : bm(bmbm)
{
	name = nm;
	fileName = fn;
	Attr = vec;
	headLength = max(hL,24);
	recordLength = 0;
	Attr.push_back(attribute("#nullList", (Attr.size() + 7) / 8, false, false));
	Attr.push_back(attribute("#preBlock", ATTR_TYPE_DOUBLE, false, false));
	Attr.push_back(attribute("#preOffset", ATTR_TYPE_DOUBLE, false, false));
	Attr.push_back(attribute("#nextBlock", ATTR_TYPE_DOUBLE, false, false));
	Attr.push_back(attribute("#nextOffset", ATTR_TYPE_DOUBLE, false, false));
	for (auto it : Attr)
	{
		AttrPos[it.name] = recordLength;
		AttrMap[it.name] = it;
		if (it.type == ATTR_TYPE_INT)
			recordLength += 4;
		else if (it.type == ATTR_TYPE_FLOAT)
			recordLength += 4;
		else if (it.type == ATTR_TYPE_DOUBLE)
			recordLength += 8;
		else
			recordLength += it.type;
	}
}
 void table::insertIntoBlock(char *data)
{
	pair<int,int> firstRecord = getFirstRecordPos();
	pair<int,int> firstBlank = getFirstBlankPos();
	if (firstBlank != make_pair(0, 0))
	{
		pair<int,int> nextBlank = getNext(firstBlank);
		setFirstBlankPos(nextBlank);
		setPre(nextBlank, make_pair(0, 0));
		bm.setDataPackInBlock(fileName, firstBlank.first, firstBlank.second, AttrPos["#preBlock"], data);
		setNext(firstBlank, firstRecord);
		setPre(firstRecord, firstBlank);
		setFirstRecordPos(firstBlank);
	}
	else
	{
		pair<int,int> restPos = getRest();
		bm.setDataPackInBlock(fileName, restPos.first, restPos.second, AttrPos["#preBlock"], data);
		setNext(restPos, firstRecord);
		setPre(firstRecord, restPos);
		setAddRest(restPos);
		setFirstRecordPos(restPos);
		setPre(restPos, make_pair(0, 0));
	}
}

void table::deleteFromBlock(pair<int,int> pos)
{
	char *zero = new char[recordLength - 16];
	memset(zero, 0, sizeof(char) * (recordLength - 16));
	pair<int,int> firstBlank = getFirstBlankPos();
	pair<int,int> firstRecord = getFirstRecordPos();
	if (pos == firstRecord)
	{
		pair<int,int> next = getNext(pos);
		setPre(next, make_pair(0, 0));
		setFirstRecordPos(next);
		setNext(pos, firstBlank);
		setPre(firstBlank, pos);
		setPre(pos, make_pair(0, 0));
		setFirstBlankPos(pos);
	}
	else
	{
		pair<int,int> pre = getPre(pos);
		pair<int,int> next = getNext(pos);
		setNext(pre, next);
		setPre(next, pre);
		setNext(pos, firstBlank);
		setPre(firstBlank, pos);
		setFirstBlankPos(pos);
		setPre(pos, make_pair(0, 0));
	}
}
