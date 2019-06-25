#include "pch.h"
#include "fileManager.h"


fileManager::fileManager() { }

fileManager::~fileManager() { }

blockNode fileManager::readIn(pair<string, int> id)
{
	if (M.count(id.first) == 0)
		M[id.first] = fileNode(id.first);
	M[id.first].open();
	FILE *fin = M[id.first].fp;
	blockNode nd;
	fseek(fin, id.second * BLOCK_SIZE, SEEK_SET);
	fread(nd.data, BLOCK_SIZE, 1, fin);
	M[id.first].close();
	return nd;
}

void fileManager::writeBack(pair<string, int> id, blockNode nd)
{
	if (M.count(id.first) == 0)
		M[id.first] = fileNode(id.first);
	M[id.first].open();
	FILE *fout = M[id.first].fp;
	fseek(fout, id.second * BLOCK_SIZE, SEEK_SET);
	fwrite(nd.data, BLOCK_SIZE, 1, fout);
	fflush(fout);
	M[id.first].close();
	M.erase(id.first);
}
