#pragma once
#include "minisql.h"
#include "blockNode.h"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>

using namespace std;

class fileManager
{
	// Records filename and its file pointer.
    class fileNode
    {
    public:
        string fileName;
        FILE *fp;

		// Open file, if file does not exist, create new one.
        void open() { fopen_s(&fp,fileName.c_str(), "rb+");
			if (fp == NULL) fopen_s(&fp,fileName.c_str(), "wb+"); }

		// Close file stream.
		void close() { if (fp) fclose(fp); }

		// Copy constructor
		fileNode(const fileNode & temp) { fileName = temp.fileName; fp = temp.fp; }

        fileNode(string fn="") { fileName = fn; fp = NULL; }
        ~fileNode() { if (fp) fclose(fp); }
    };

public:
    map<string, fileNode> M;

	// Read a block from file to memory.
	// pair<FileName, BlockNumber>  --TO--> BlockData
    blockNode readIn(pair<string, int> id);

	// Write a block back to file.
	// pair<FileName, BlockNumber>, BlockData
    void writeBack(pair<string, int> id, blockNode nd);

	fileManager();
	~fileManager();
};

