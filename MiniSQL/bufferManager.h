#pragma once
#include "fileManager.h"
#include "blockNode.h"
#include <map>
#include <set>

#define BUFFER_TYPE_LRU 0 // least recent used
//#define BUFFER_TYPE_MRU 1 // most recent used // DON'T USE!
//#define BUFFER_TYPE_IMM 2 // loss-immediate   // DON'T USE!

using namespace std;

class bufferManager
{
protected:
	fileManager fm;
	pair<string, int> lastRefreshed;
	int bufferUsed;

	// This turns 
	string getStrFromNode(blockNode &nd, int begin, int end);
	void setStrInNode(blockNode &nd, int begin, int end, string str);

	void setDataPackInNode(blockNode &nd, int begin, int len, char *data);


	void popBlock();
	void pushBlock(pair<string, int> blockName);
	void loadBlock(pair<string, int> blockName);

	void refreshBlock(pair<string, int> blockName);

public:
	int bufferSize; // Max buffersize allowed.
	unsigned int currentStep; // Used buffer size, when buffer is full, pop LRU block out.

	//  map<pair<FileName, BlockNumber>, pair<DataPack, Tag>>
	map<pair<string, int>, pair<blockNode, unsigned int>> M;  

	// map<Tab, pair<FileName, BlockNumber>>
	map<unsigned int, pair<string, int>> T;
	// default bufferSize=256KB
	bufferManager(unsigned long buffer_Size=131072, int deault_Buffer_Type = BUFFER_TYPE_LRU);
	~bufferManager();

	void flush();

	string getStrFromBlock(string file_Name, int block_Number, int begin, int end);
	void setStrInBlock(string file_Name, int block_Number, int begin, int end, string str);
	void setDataPackInBlock(string file_Name, int block_Number, int begin, int len, char *data);

	// In following fuction templates, _tp could be int, float and double.

	template<typename _tp>
	_tp getNumFromNode(blockNode &nd, int off)
	{
		if (off + sizeof(_tp) < BLOCK_SIZE)
			return *(_tp *)(nd.data + off);
        throw string("BufferManager: cross-block data access.");
	}

	template<typename _tp>
	void setNumInNode(blockNode &nd, int off, _tp num)
	{
		if (off + sizeof(_tp) < BLOCK_SIZE)
			*(_tp *)(nd.data + off) = num;
        else throw string("BufferManager: cross-block data access.");
	}

	template<typename _tp>
	_tp getNumFromBlock(string file_Name, int block_Number, int offset)
	{
		auto blockName = make_pair(file_Name, block_Number);
		if (M.count(blockName) == 0)
			loadBlock(blockName);
		refreshBlock(blockName);
		return getNumFromNode<_tp>(M[blockName].first, offset);
	}

	template<typename _tp>
	void setNumInBlock(string file_Name, int block_Number, int offset, _tp num)
	{
		auto blockName = make_pair(file_Name, block_Number);
		if (M.count(blockName) == 0)
			loadBlock(blockName);
		refreshBlock(blockName);
		setNumInNode<_tp>(M[blockName].first, offset, num);
	}
};
