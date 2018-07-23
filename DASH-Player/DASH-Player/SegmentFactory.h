#pragma once
#include <libdash.h>
#include <deque>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

using namespace dash::mpd;
using namespace dash::network;
using namespace std;

class SegmentFactory
{
public:
	SegmentFactory();
	SegmentFactory(IMPD * mpd);
	~SegmentFactory();
	deque<ISegment*> getSegments();
	deque<ISegment*> createSegments(int bandwidth = 0);
	
private:
	IMPD* mpd;
	IPeriod* period;
	IAdaptationSet* adaptationSet;
	IRepresentation* representation;
	IRepresentation * getRepresentation(int bandwidth = 0);
	IAdaptationSet* getAdaptationSet(string type);
	void findSegmentFolderPath();
	vector <IBaseUrl*> baseUrls;
	deque<ISegment*> segments;
};
