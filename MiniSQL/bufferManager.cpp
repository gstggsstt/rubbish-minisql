#include "pch.h"
#include "bufferManager.h"

string bufferManager::getStrFromNode(blockNode &nd, int begin, int end)
{
    if (end > BLOCK_SIZE)
        throw string("BufferManager: cross-block data access.");
    string temp;
    for (int i = begin; i < end && nd.data[i]; ++i)
        temp += nd.data[i];
    return temp;
}

void bufferManager::setStrInNode(blockNode &nd, int begin, int end, string str)
{
    if (end > BLOCK_SIZE)
        throw string("BufferManager: cross-block data access.");
    int i, j = 0;
    for (i = begin; i < (int)(begin + str.length()) && i < end; ++i)
        nd.data[i] = str[j++];
    for (; i < end; ++i)
        nd.data[i] = 0;
}

void bufferManager::setDataPackInNode(blockNode &nd, int begin, int len, char *data)
{
    if (begin + len > BLOCK_SIZE)
        throw string("BufferManager: cross-block data access.");
    for (int i = begin, j = 0; i < begin + len; ++i, ++j)
        nd.data[i] = data[j];
    return;
}

void bufferManager::popBlock()
{
    auto temp = T.begin();
    fm.writeBack(temp->second, M[temp->second].first);
    M.erase(temp->second);
    T.erase(temp);
    bufferUsed -= BLOCK_SIZE;
}

void bufferManager::pushBlock(pair<string, int> blockName)
{
    T[++currentStep] = blockName;
    M[blockName] = make_pair(fm.readIn(blockName), currentStep);
    bufferUsed += BLOCK_SIZE;
}

void bufferManager::loadBlock(pair<string, int> blockName)
{
    if (bufferUsed + BLOCK_SIZE > bufferSize)
        popBlock();
    pushBlock(blockName);
}

void bufferManager::refreshBlock(pair<string, int> blockName)
{
	if (lastRefreshed == blockName) return;
    T.erase(M[blockName].second);
    M[blockName].second = ++currentStep;
    T[currentStep] = blockName;
	lastRefreshed = blockName;
}

bufferManager::bufferManager(unsigned long buffer_Size, int deault_Buffer_Type) : fm()
{
    bufferSize = buffer_Size;
    bufferUsed = 0;
    currentStep = 0;
}

bufferManager::~bufferManager()
{
	flush();
}

void bufferManager::flush()
{
	while (!M.empty()) popBlock();
}

string bufferManager::getStrFromBlock(string file_Name, int block_Number, int begin, int end)
{
    auto blockName = make_pair(file_Name, block_Number);
    if (M.count(blockName) == 0)
        loadBlock(blockName);
    refreshBlock(blockName);
    return getStrFromNode(M[blockName].first, begin, end);
}

void bufferManager::setStrInBlock(string file_Name, int block_Number, int begin, int end, string str)
{
    auto blockName = make_pair(file_Name, block_Number);
    if (M.count(blockName) == 0)
        loadBlock(blockName);
    refreshBlock(blockName);
    setStrInNode(M[blockName].first, begin, end, str);
}

void bufferManager::setDataPackInBlock(string file_Name, int block_Number, int begin, int len, char *data)
{
    auto blockName = make_pair(file_Name, block_Number);
    if (M.count(blockName) == 0)
        loadBlock(blockName);
    refreshBlock(blockName);
    setDataPackInNode(M[blockName].first, begin, len, data);
}