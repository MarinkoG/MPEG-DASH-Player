#pragma once

#include "Frame.h"
#include <IMPD.h>
#include <string.h>
#include <IDASHManager.h>
#include <qobject.h>
#include "libdash.h"
#include <deque>
#include "SegmentFactory.h"
#include "SegmentDownloader.h"
#include <fstream>
#include <iostream>

using namespace	std;
using namespace dash::mpd;

class MediaEngine : public QObject
{
	Q_OBJECT

public:
	MediaEngine(Frame *video);
	~MediaEngine();
	bool parseMPD(string url);
	IMPD* getMPD();
	bool start();
	bool createSegments(IMPD* mpd);
	bool downloadSegments();
	static void print(string string);

private:
	Frame *video;
	IMPD *mpd;
	deque<ISegment*> segments;
	deque<ISegment*> downloadedSegments;
	SegmentFactory* segmentFactory;
	SegmentDownloader* segmentDownloader;

public slots:
	void saveSegment();
};
