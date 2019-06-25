#include "pch.h"
#include "attribute.h"

attribute::attribute(string nm, int tp, bool nN, bool pr, bool unq, string idx)
{
	name=nm; type=tp;
	notNull=nN;
	primaryKey=pr;
	index=idx;
	unique = unq;
}
