#pragma once
#include "bufferManager.h"
#include "table.h"
#include <iostream>
#include <vector>
#include <map>
#include <cstdio>
#include <sstream>

using namespace std;

class catalogManager
{
	public:
		const char* catalogFile = "catalog.txt";
		bufferManager & bm; // reference variable
		vector<string> dataStored;
		void readInfo();  // readInfo to dataStored
		void writeInfo(); // write dataStored to file
		void createTable(string tableName,vector<attribute> Attr); // add tableInfo into catalogFile

		// read from file, and turn table info into "table" object which is defined in "table.h"
		table getTable(string tableName);  
		// delete table info in catalog file
		void dropTable(string tableName);
		// find which table and attribute is idx established on
		pair<string,string> getAttrByIndex(string idx);
		// drop index info in catalog file
		void discardAttrIndex(string tableName, string attrName);
		// create index infor in catalog file
		void giveAttrIndex(string tableName, string Attr, string idx);

		catalogManager(bufferManager & bmbm) : bm(bmbm) {}
};

