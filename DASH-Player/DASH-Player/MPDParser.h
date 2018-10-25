#pragma once
#include<string>
#include "libdash.h"
#include <IMPD.h>
#include <vector>
#include <algorithm>
#include <qstring.h>
#include <qstringlist.h>

using namespace std;
using namespace dash::mpd;

class MPDParser
{
public:
	MPDParser();
	~MPDParser();
	bool parseMPD(string url);
	IMPD* getMPD();
	void separateAdaptationSets();
	void sortRepresentations();
	QStringList getVideoQualities();

private:
	IMPD *mpd;
	IPeriod *period;
	vector<IAdaptationSet*> videoAdaptationSets;
	vector<IAdaptationSet*> audioAdaptationSets;
	vector<IRepresentation*> representations;
	IAdaptationSet *adaptationSet;
	IRepresentation *representation;
	void loadAtributes();
	IRepresentation * getRepresentation(int bandwidth);
	
};

