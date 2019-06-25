#pragma once
#include "catalogManager.h"
#include "condition.h"
#include <iostream>
#include <map>

using namespace std;

class indexManager
{
	typedef pair<int,int> PII;
	// Index map of int, float, double and string data.
	// map<  pair< tableName, attrName >, 
	//       map<data,
	//           position<blockNumber, offset> > > 
	map<pair<string,string>, map<int,PII > > IDXint;
	map<pair<string,string>, map<float,PII > > IDXfloat;
	map<pair<string,string>, map<double,PII > > IDXdouble;
	map<pair<string,string>, map<string,PII > > IDXstring;

	catalogManager & cm; // reference variable

	// load index from file
	void loadIndex(table & T, string Attr);

	// write index back to file
	void writeIndex(table & T, string Attr, map<int,PII > &Mint,
		map<float,PII > &Mfloat, map<double,PII > &Mdouble, 
		map<string,PII > &Mstring, string fileName);

public:
	// write all indices back to file, like flush()
	void writeToFile();

	// create index in both memory and disk (not changing catalog file)
	void createIndex(table T, string Attr, string idx);

	// drop index in both memory and disk (not changing catalog file)
	void dropIndex(string idx);

	// check whether index is loaded from disk, if not, call 'loadIndex'
	void checkLoaded(table & T, attribute & it);

	// insert an index of record 
	void insertIndex(table &T, map<string,string> data, PII pos);

	// delete an index of record
	void deleteIndex(table &T, vector<PII > vec);

	// point selection 
	vector<PII> selectPosById(table & T, condition & cond);
	// range selection
	vector<PII> selectPosById_Range(table & T, condition & cond);

	indexManager(catalogManager & cmcm): cm(cmcm){}
};
