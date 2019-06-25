#pragma once
#include "minisql.h"
#include <cstring>
class blockNode
{
public:
    char data[BLOCK_SIZE];
    blockNode() { memset(data, 0, sizeof(data)); }
    ~blockNode() {}
};


